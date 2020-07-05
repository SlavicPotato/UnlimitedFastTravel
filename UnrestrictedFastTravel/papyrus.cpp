#include "pch.h"

namespace UFT
{
    static bool PP_HasOverride(StaticFunctionTag* base, SInt32 c)
    {
        switch (c) {
        case PFTOverrideCodes::kPFT_Combat:
            return pft_state.combat;
        case PFTOverrideCodes::kPFT_TakingDamage:
            return pft_state.taking_damage;
        case PFTOverrideCodes::kPFT_Location:
            return pft_state.location;
        case PFTOverrideCodes::kPFT_GuardsPursuing:
            return pft_state.guards_pursuing;
        case PFTOverrideCodes::kPFT_OverEncumbered:
            return pft_state.over_encumbered;
        case PFTOverrideCodes::kPFT_InAir:
            return pft_state.in_air;
        case PFTOverrideCodes::kPFT_WorldspaceTravel:
            return pft_state.worldspace_travel;
        case PFTOverrideCodes::kPFT_ScriptCondition:
            return pft_state.script_cond;
        case PFTOverrideCodes::kPFT_Dragon:
            return pft_state.dragon;
#ifdef _UFT_ENABLE_MOSTLY_USELESS
        case PFTOverrideCodes::kPFT_VampireFeeding:
            return pft_state.vamp_feed;
#endif
#ifdef _UFT_ENABLE_UNKNOWN
        case PFTOverrideCodes::kPFT_Unk01:
            return pft_state.unk01;
#endif
        }
        return false;
    }

    static void PP_SetOverride(StaticFunctionTag* base, SInt32 c, bool v)
    {
        switch (c) {
        case PFTOverrideCodes::kPFT_All:
            pft_state.combat = v;
            pft_state.taking_damage = v;
            pft_state.location = v;
            pft_state.guards_pursuing = v;
            pft_state.over_encumbered = v;
            pft_state.in_air = v;
            pft_state.script_cond = v;
            break;
        case PFTOverrideCodes::kPFT_Combat:
            pft_state.combat = v;
            break;
        case PFTOverrideCodes::kPFT_TakingDamage:
            pft_state.taking_damage = v;
            break;
        case PFTOverrideCodes::kPFT_Location:
            pft_state.location = v;
            break;
        case PFTOverrideCodes::kPFT_GuardsPursuing:
            pft_state.guards_pursuing = v;
            break;
        case PFTOverrideCodes::kPFT_OverEncumbered:
            pft_state.over_encumbered = v;
            break;
        case PFTOverrideCodes::kPFT_InAir:
            pft_state.in_air = v;
            break;
        case PFTOverrideCodes::kPFT_WorldspaceTravel:
            pft_state.worldspace_travel = v;
            break;
        case PFTOverrideCodes::kPFT_ScriptCondition:
            pft_state.script_cond = v;
            break;
        case PFTOverrideCodes::kPFT_Dragon:
            pft_state.dragon = v;
            break;
#ifdef _UFT_ENABLE_MOSTLY_USELESS
        case PFTOverrideCodes::kPFT_VampireFeeding:
            pft_state.vamp_feed = v;
            break;
#endif
#ifdef _UFT_ENABLE_UNKNOWN
        case PFTOverrideCodes::kPFT_Unk01:
            pft_state.unk01 = v;
            break;
#endif
        }
    }

    static UInt32 PP_GetVersion(StaticFunctionTag* base)
    {
        return MAKE_PLUGIN_VERSION(
            PLUGIN_VERSION_MAJOR,
            PLUGIN_VERSION_MINOR,
            PLUGIN_VERSION_REVISION);
    }

    bool RegisterFunctions(VMClassRegistry* registry)
    {
        registry->RegisterFunction(
            new NativeFunction1<StaticFunctionTag, bool, SInt32>("HasOverride", "UnlimitedFastTravel", PP_HasOverride, registry));
        registry->RegisterFunction(
            new NativeFunction2<StaticFunctionTag, void, SInt32, bool>("SetOverride", "UnlimitedFastTravel", PP_SetOverride, registry));
        registry->RegisterFunction(
            new NativeFunction0<StaticFunctionTag, UInt32>("GetVersion", "UnlimitedFastTravel", PP_GetVersion, registry));

        return true;
    }
}