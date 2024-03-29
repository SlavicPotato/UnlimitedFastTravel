#pragma once

namespace UFT
{
    enum PFTOverrideCodes : std::int32_t
    {
        kPFT_All = -1,
        kPFT_Combat = 0,
        kPFT_TakingDamage = 1,
        kPFT_Location = 2,
        kPFT_GuardsPursuing = 3,
        kPFT_OverEncumbered = 4,
        kPFT_InAir = 5,
        kPFT_WorldspaceTravel = 6,
        kPFT_ScriptCondition = 7,
        kPFT_AIDriven = 8,
#ifdef _UFT_ENABLE_UNKNOWN
        kPFT_Unk01 = 9,
#endif
        kPFT_Dragon = 10
    };

    extern bool RegisterFunctions(VMClassRegistry* registry);
}