#include "pch.h"

namespace UFT
{
    using namespace JITASM;
    using namespace Patching;

    typedef bool(__cdecl* isOverEncumbered_t)(void*);
    static auto IsOverEncumbered_O = IAL::Addr<isOverEncumbered_t>(36457);

    typedef bool(__cdecl* guardsPursuing_t)(void*, Actor*, int32_t, int8_t);
    static auto GuardsPursuing_O = IAL::Addr<guardsPursuing_t>(40314);

    typedef bool(__cdecl* takingDamage_t)(void*);
    static auto TakingDamage_O = IAL::Addr<takingDamage_t>(33736);

    typedef bool(__cdecl* inCombat_t)(void*, uint64_t);
    static auto InCombat_O = IAL::Addr<inCombat_t>(40388);

    typedef bool(__cdecl* inAir_t)(void*);
    static auto InAir_O = IAL::Addr<inAir_t>(36259);

    static auto unkGlob1 = IAL::Addr<void**>(514167);

    typedef void(__cdecl* unkfunc1_t)(void*, Actor*, uint64_t);
    typedef void(__cdecl* unkfunc2_t)(Actor*, uint8_t);

    static auto unkfunc1 = IAL::Addr<unkfunc1_t>(40330);
    static auto unkfunc2 = IAL::Addr<unkfunc2_t>(36465);

    static auto ftCheckFunc = IAL::Addr<uintptr_t>(39372);
    static auto ftFunc = IAL::Addr<uintptr_t>(39373);

    static FastTravelEventHandler g_fastTravelEventHandler;

    PFTState pft_state;

    static bool IsOverEncumbered_Hook(Actor* actor)
    {
        if (pft_state.over_encumbered &&
            actor == *g_thePlayer)
        {
            return false;
        }

        return IsOverEncumbered_O(actor);
    }

    static bool GuardsPursuing_Hook(void* p1, Actor* actor, int32_t p3, int8_t p4)
    {
        if (pft_state.guards_pursuing) {
            return false;
        }

        return GuardsPursuing_O(p1, actor, p3, p4);
    }

    static bool TakingDamage_Hook(void* p1)
    {
        if (pft_state.taking_damage) {
            return false;
        }

        return TakingDamage_O(p1);
    }

    static bool InCombat_Hook(void *p1, uint64_t p2)
    {
        if (pft_state.combat) {
            return false;
        }

        return InCombat_O(p1, p2);
    }

    static bool AllowLocationTravelHook()
    {
        bool allow = pft_state.location;

        if (allow && !pft_state.worldspace_travel)
        {
            auto player = *g_thePlayer;
            if (player->parentCell != nullptr) {
                return (player->parentCell->cellFlags & TESObjectCELL::kIsInteriorCell) ==
                    TESObjectCELL::kIsInteriorCell;
            }
        }

        return allow;
    }

    static bool InAir_Hook(void *p1)
    {
        if (pft_state.in_air) {
            return false;
        }

        return InAir_O(p1);
    }

    static void MessageHandler(SKSEMessagingInterface::Message* message)
    {
        if (message->type == SKSEMessagingInterface::kMessage_DataLoaded) {
            GetEventDispatcherList()->fastTravelEndEventDispatcher.AddEventSink(&g_fastTravelEventHandler);
            Message("Fast travel event sink added");
        }
    }

    static void InitializeData()
    {
        pft_state.over_encumbered = true;
        pft_state.guards_pursuing = true;
        pft_state.combat = true;
        pft_state.taking_damage = true;
        pft_state.location = true;
        pft_state.in_air = false;
        pft_state.worldspace_travel = false;
    }

    static void ApplyPatches()
    {
        struct FastTravelLocationInject : JITASM {
            FastTravelLocationInject(uintptr_t retnOKAddr, uintptr_t retnBadBaseAddr, uintptr_t callAddr)
                : JITASM()
            {
                Xbyak::Label retnOKLabel;
                Xbyak::Label retnBlockLabel;
                Xbyak::Label retnBlockNMLabel;
                Xbyak::Label callLabel;

                Xbyak::Label blockTravel;
                Xbyak::Label blockTravelNoMsg;

                call(ptr[rip + callLabel]);
                test(al, al);
                je(blockTravel);
                mov(bl, al);
                jmp(ptr[rip + retnOKLabel]);
                L(blockTravel);
                test(dil, dil);
                jne(blockTravelNoMsg);
                jmp(ptr[rip + retnBlockLabel]);
                L(blockTravelNoMsg);
                jmp(ptr[rip + retnBlockNMLabel]);

                L(retnOKLabel);
                dq(retnOKAddr);

                L(retnBlockLabel);
                dq(retnBadBaseAddr + 0x5);

                L(retnBlockNMLabel);
                dq(retnBadBaseAddr + 0x16);

                L(callLabel);
                dq(callAddr);
            }
        };

        Message("Location..");
        {
            uintptr_t target = ftCheckFunc + 0x226;

            FastTravelLocationInject code(target + 0x55, target + 0x4, uintptr_t(AllowLocationTravelHook));
            g_branchTrampoline.Write6Branch(target, code.get());
            //safe_memset(target + 0x6, 0xCC, 3);
        }

    }

    static void InstallHooks()
    {
        Message("In combat..");
        g_branchTrampoline.Write5Call(ftCheckFunc + 0xA3, uintptr_t(InCombat_Hook));

        Message("Guard pursuit..");
        g_branchTrampoline.Write5Call(ftCheckFunc + 0xDE, uintptr_t(GuardsPursuing_Hook));

        Message("Taking damage..");
        g_branchTrampoline.Write5Call(ftCheckFunc + 0x135, uintptr_t(TakingDamage_Hook));

        Message("Over-encumbered..");
        g_branchTrampoline.Write5Call(ftFunc + 0x4A, uintptr_t(IsOverEncumbered_Hook));
        g_branchTrampoline.Write5Call(ftCheckFunc + 0x108, uintptr_t(IsOverEncumbered_Hook));

        Message("In air..");
        g_branchTrampoline.Write5Call(ftCheckFunc + 0x185, uintptr_t(InAir_Hook));
    }

    void Initialize()
    {
        if (!SKSE::g_messaging->RegisterListener(SKSE::g_pluginHandle, "SKSE", MessageHandler)) {
            MsgError("Registering message listener failed");
        }

        if (!SKSE::g_papyrus->Register(RegisterFunctions)) {
            MsgError("Registering papyrus functions failed");
        }

        InitializeData();
        ApplyPatches();
        InstallHooks();

        Message("Init done");
    }

    auto FastTravelEventHandler::ReceiveEvent(TESFastTravelEndEvent* evn, EventDispatcher<TESFastTravelEndEvent>* dispatcher)
        -> EventResult
    {
        if (pft_state.combat) {
            auto player = *g_thePlayer;

            if (player->IsInCombat())
            {
                unkfunc1(*unkGlob1, player, 0);
                player->Unk_CombatAlarm();
                unkfunc2(player, 0);
            }
        }

        return kEvent_Continue;
    };


}