#include "pch.h"

namespace UFT
{
    using namespace JITASM;
    using namespace Patching;

    typedef bool(__cdecl* isOverEncumbered_t)(void* actor);
    static auto IsOverEncumbered_O = IAL::Addr<isOverEncumbered_t>(36457);

    typedef bool(__cdecl* guardsPursuing_t)(void* p1, Actor* actor, int32_t p3, int8_t p4);
    static auto GuardsPursuing_O = IAL::Addr<guardsPursuing_t>(40314);

    static auto unkGlob1 = IAL::Addr<void**>(514167);

    typedef void(__cdecl* unkfunc1_t)(void* p1, Actor* actor, uint64_t p3);
    typedef void(__cdecl* unkfunc2_t)(Actor* actor, uint8_t p3);

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

    static bool AllowCombatTravelHook()
    {
        return pft_state.combat;
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
        pft_state.location = true;
        pft_state.worldspace_travel = false;
    }

    static void ApplyPatches()
    {
        struct FastTravelInject : JITASM {
            FastTravelInject(uintptr_t retnOKAddr, uintptr_t retnBadBaseAddr, uintptr_t callAddr)
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

        Message("Patching combat..");
        {
            uintptr_t target = ftCheckFunc + 0xAC;

            FastTravelInject code(target - 0x73, target, uintptr_t(AllowCombatTravelHook));
            g_branchTrampoline.Write5Branch(target, code.get());
        }

        Message("Patching location..");
        {
            uintptr_t target = ftCheckFunc + 0x226;

            FastTravelInject code(target + 0x55, target + 0x4, uintptr_t(AllowLocationTravelHook));
            g_branchTrampoline.Write6Branch(target, code.get());
            //safe_memset(target + 0x6, 0xCC, 3);
        }
    }

    static void InstallHooks()
    {
        Message("Over-encumbered hooks..");
        g_branchTrampoline.Write5Call(ftFunc + 0x4A, uintptr_t(IsOverEncumbered_Hook));
        g_branchTrampoline.Write5Call(ftCheckFunc + 0x108, uintptr_t(IsOverEncumbered_Hook));

        Message("Guard pursuit hook..");
        g_branchTrampoline.Write5Call(ftCheckFunc + 0xDE, uintptr_t(GuardsPursuing_Hook));
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