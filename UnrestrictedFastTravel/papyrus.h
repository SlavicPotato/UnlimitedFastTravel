#pragma once

namespace UFT
{
    enum PFTOverrideCodes : SInt32
    {
        kPFT_All = -1,
        kPFT_Combat,
        kPFT_TakingDamage,
        kPFT_Location,
        kPFT_GuardsPursuing,
        kPFT_OverEncumbered,
        kPFT_InAir,
        kPFT_WorldspaceTravel,
        kPFT_ScriptCondition,
        kPFT_VampireFeeding,
        kPFT_Unk01,
        kPFT_Dragon
    };

    extern bool RegisterFunctions(VMClassRegistry* registry);
}