#include "pch.h"

namespace UFT
{
	using namespace JITASM;
	using namespace Patching;

	typedef bool (*isOverEncumbered_t)(Actor*);
	static auto IsOverEncumbered_O = IAL::Addr<isOverEncumbered_t>(36457, 37453);

	typedef bool (*guardsPursuing_t)(void*, Actor*, int32_t, int8_t);
	static auto GuardsPursuing_O = IAL::Addr<guardsPursuing_t>(40314, 41324);

	typedef bool (*takingDamage_t)(MagicTarget*);
	static auto TakingDamage_O = IAL::Addr<takingDamage_t>(33736, 34520);

	typedef bool (*enemiesNear_t)(void* procManager, uint64_t);
	static auto EnemiesNear_O = IAL::Addr<enemiesNear_t>(40388, 41402);

	typedef bool (*inAir_t)(PlayerCharacter*);
	static auto InAir_O = IAL::Addr<inAir_t>(36259, 37243);

	static auto processManager = IAL::Addr<void**>(514167, 400315);

	typedef void (*stopCombatAlarmOnActor_t)(void* procManager, Actor*, bool dontEndAlarm);

	static auto StopCombatAlarmOnActor = IAL::Addr<stopCombatAlarmOnActor_t>(40330, 41340);

	typedef bool (*mmIsOnFlyingMount_t)(PlayerCharacter*);
	static auto mmIsOnFlyingMount_O = IAL::Addr<mmIsOnFlyingMount_t>(36877, 37901);

	static auto ftCheckFunc = IAL::Addr<uintptr_t>(39372, 40444);
	static auto ftFunc = IAL::Addr<uintptr_t>(39373, 40445);
	static auto mmarkerCreateFunc = IAL::Addr<uintptr_t>(52224, 53111);

	static auto unkGlob01 = IAL::Addr<void**>(514960, 401100);

	static FastTravelEventHandler g_fastTravelEventHandler;

	PFTState pft_state;

	static bool IsOverEncumbered_Hook(Actor* actor)
	{
		// Actor check probably unnecessary, should always be player
		if (pft_state.over_encumbered &&
		    actor == *g_thePlayer)
		{
			return false;
		}

		return IsOverEncumbered_O(actor);
	}

	static bool GuardsPursuing_Hook(void* p1, Actor* actor, int32_t p3, int8_t p4)
	{
		if (pft_state.guards_pursuing)
		{
			return false;
		}

		return GuardsPursuing_O(p1, actor, p3, p4);
	}

	static bool TakingDamage_Hook(MagicTarget* p1)
	{
		if (pft_state.taking_damage)
		{
			return false;
		}

		return TakingDamage_O(p1);
	}

	static bool EnemiesNear_Hook(void* procManager, uint64_t p2)
	{
		if (pft_state.combat)
		{
			return false;
		}

		return EnemiesNear_O(procManager, p2);
	}

	static bool LocationTravel_Hook()
	{
		bool allow = pft_state.location;

		if (allow && !pft_state.worldspace_travel)
		{
			auto player = *g_thePlayer;

			if (player)
			{
				auto worldspace = player->currentWorldSpace;

				if (worldspace)
				{
					return (worldspace->flags & TESWorldSpace::kCantFastTravel) == 0;
				}
			}
		}

		return allow;
	}

	static bool InAir_Hook(PlayerCharacter* p1)
	{
		if (pft_state.in_air)
		{
			return false;
		}

		return InAir_O(p1);
	}

	static bool ScriptCond_Hook(PlayerCharacter* player)
	{
		if (pft_state.script_cond)
		{
			return true;
		}

		return (player->unkBD9 & PlayerCharacter::kFastTravelEnabled) != 0;
	}

	static bool AIDriven_Hook(PlayerCharacter* player)
	{
		if (pft_state.ai_driven)
		{
			return false;
		}

		return (player->unkBDA & PlayerCharacter::kAIDriven) != 0;
	}

#ifdef _UFT_ENABLE_UNKNOWN
	static bool Unk01Cond_Hook()
	{
		return pft_state.unk01;
	}
#endif

	static bool DragonCond_Hook()
	{
		return pft_state.dragon;
	}

	static bool DragonCondMM_Hook(PlayerCharacter* player)
	{
		if (pft_state.dragon)
		{
			return false;
		}

		return mmIsOnFlyingMount_O(player);
	}

	static void MessageHandler(SKSEMessagingInterface::Message* message)
	{
		if (message->type == SKSEMessagingInterface::kMessage_DataLoaded)
		{
			ScriptEventSourceHolder::GetSingleton()->AddEventSink<TESFastTravelEndEvent>(
				std::addressof(g_fastTravelEventHandler));

			Msg("Fast travel event sink added");
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
		pft_state.script_cond = false;
		pft_state.dragon = false;
#ifdef _UFT_ENABLE_MOSTLY_USELESS
		pft_state.ai_driven = false;
#endif
#ifdef _UFT_ENABLE_UNKNOWN
		pft_state.unk01 = false;
#endif
	}

	static void ApplyPatches()
	{
		Msg("Location ..");
		{
			struct FastTravelLocationInject : JITASM
			{
				FastTravelLocationInject(
					uintptr_t retnOKAddr,
					uintptr_t retnBadBaseAddr,
					uintptr_t callAddr) :
					JITASM(ISKSE::GetLocalTrampoline())
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

			uintptr_t target = ftCheckFunc + (IAL::IsAE() ? 0x23B : 0x226);

			FastTravelLocationInject code(target + 0x55, target + 0x4, uintptr_t(LocationTravel_Hook));
			ISKSE::GetBranchTrampoline().Write6Branch(target, code.get());
		}

		struct FlagConditionInject : JITASM
		{
			FlagConditionInject(uintptr_t targetAddr, uintptr_t callAddr) :
				JITASM(ISKSE::GetLocalTrampoline())
			{
				Xbyak::Label retnLabel;
				Xbyak::Label callLabel;

				mov(rcx, rsi);
				call(ptr[rip + callLabel]);
				test(al, al);
				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(targetAddr + 0x7);

				L(callLabel);
				dq(callAddr);
			}
		};

		Msg("Script/console ..");
		{
			uintptr_t target = ftCheckFunc + (IAL::IsAE() ? 0x16D : 0x158);

			FlagConditionInject code(target, uintptr_t(ScriptCond_Hook));
			ISKSE::GetBranchTrampoline().Write6Branch(target, code.get());
		}

		Msg("Dragon ..");
		{
			struct DragonConditionInject : JITASM
			{
				DragonConditionInject(uintptr_t targetAddr, uintptr_t callAddr) :
					JITASM(ISKSE::GetLocalTrampoline())
				{
					Xbyak::Label retnCont;
					Xbyak::Label retnSkip;
					Xbyak::Label callLabel;

					Xbyak::Label skipLabel;

					call(ptr[rip + callLabel]);
					test(al, al);
					jne(skipLabel);

					// mov rcx, qword ss:[rsp+0x68] (mountedActor)
					// test rcx, rcx
					db(reinterpret_cast<Xbyak::uint8*>(targetAddr), 0x8);
					jmp(ptr[rip + retnCont]);

					L(skipLabel);
					jmp(ptr[rip + retnSkip]);

					L(retnCont);
					dq(targetAddr + 0x8);

					L(retnSkip);
					dq(targetAddr + (IAL::IsAE() ? 0xEA : 0xDE));

					L(callLabel);
					dq(callAddr);
				}
			};

			uintptr_t target = ftCheckFunc + 0x50;

			DragonConditionInject code(target, uintptr_t(DragonCond_Hook));
			ISKSE::GetBranchTrampoline().Write6Branch(target, code.get());
		}

		Msg("AI Driven ..");
		{
			uintptr_t target = ftCheckFunc + (IAL::IsAE() ? 0x259 : 0x244);

			FlagConditionInject code(target, uintptr_t(AIDriven_Hook));
			ISKSE::GetBranchTrampoline().Write6Branch(target, code.get());
		}

#ifdef _UFT_ENABLE_UNKNOWN
		Msg("Unk01 ..");
		{
			struct Unk01Inject : JITASM
			{
				Unk01Inject(uintptr_t targetAddr, uintptr_t callAddr) :
					JITASM(ISKSE::GetLocalTrampoline())
				{
					Xbyak::Label retnCont;
					Xbyak::Label retnSkip;
					Xbyak::Label callLabel;
					Xbyak::Label unkGlob01Label;

					Xbyak::Label skipLabel;

					call(ptr[rip + callLabel]);
					test(al, al);
					jne(skipLabel);
					mov(rbx, ptr[rip + unkGlob01Label]);
					mov(rbx, ptr[rbx]);
					jmp(ptr[rip + retnCont]);
					L(skipLabel);
					jmp(ptr[rip + retnSkip]);

					L(retnCont);
					dq(targetAddr + 0x7);

					L(retnSkip);
					dq(targetAddr + 0x23);

					L(callLabel);
					dq(callAddr);

					L(unkGlob01Label);
					dq(uintptr_t(unkGlob01));
				}
			};

			uintptr_t target = ftCheckFunc + (IAL::IsAE() ? 0x1E5 : 0x1D0);

			Unk01Inject code(target, uintptr_t(Unk01Cond_Hook));
			ISKSE::GetBranchTrampoline().Write6Branch(target, code.get());
		}
#endif
	}

	static void InstallHooks()
	{
		Msg("In combat ..");
		ISKSE::GetBranchTrampoline().Write5Call(ftCheckFunc + (IAL::IsAE() ? 0xA6 : 0xA3), uintptr_t(EnemiesNear_Hook));

		Msg("Guard pursuit ..");
		ISKSE::GetBranchTrampoline().Write5Call(ftCheckFunc + (IAL::IsAE() ? 0xE1 : 0xDE), uintptr_t(GuardsPursuing_Hook));

		Msg("Taking damage ..");
		ISKSE::GetBranchTrampoline().Write5Call(ftCheckFunc + (IAL::IsAE() ? 0x141 : 0x135), uintptr_t(TakingDamage_Hook));

		Msg("Over-encumbered ..");
		ISKSE::GetBranchTrampoline().Write5Call(ftFunc + 0x4A, uintptr_t(IsOverEncumbered_Hook));
		ISKSE::GetBranchTrampoline().Write5Call(ftCheckFunc + (IAL::IsAE() ? 0x114 : 0x108), uintptr_t(IsOverEncumbered_Hook));

		Msg("In air ..");
		ISKSE::GetBranchTrampoline().Write5Call(ftCheckFunc + (IAL::IsAE() ? 0x19A : 0x185), uintptr_t(InAir_Hook));

		Msg("Dragon (map markers) ..");
		ISKSE::GetBranchTrampoline().Write5Call(mmarkerCreateFunc + (IAL::IsAE() ? 0xF3 : 0xF0), uintptr_t(DragonCondMM_Hook));
	}

	void Initialize()
	{
		auto& skse = ISKSE::GetSingleton();

		auto mi = skse.GetInterface<SKSEMessagingInterface>();

		if (!mi->RegisterListener(skse.GetPluginHandle(), "SKSE", MessageHandler))
		{
			MsgError("Registering message listener failed");
		}

		auto pi = skse.GetInterface<SKSEPapyrusInterface>();

		if (!pi->Register(RegisterFunctions))
		{
			MsgError("Registering papyrus functions failed");
		}

		InitializeData();
		ApplyPatches();
		InstallHooks();

		Msg("Done");
	}

	auto FastTravelEventHandler::ReceiveEvent(
		const TESFastTravelEndEvent* evn,
		BSTEventSource<TESFastTravelEndEvent>* dispatcher)
		-> EventResult
	{
		if (pft_state.combat)
		{
			auto player = *g_thePlayer;

			if (player && player->IsInCombat())
			{
				if (auto pm = *processManager)
				{
					StopCombatAlarmOnActor(pm, player, false);
				}
			}
		}

		return EventResult::kContinue;
	}
}