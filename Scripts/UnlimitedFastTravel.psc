Scriptname UnlimitedFastTravel Hidden

Int Function GetScriptVersion() global
    return 1
EndFunction

Int Function GetVersion() Native Global

Bool Function HasOverride(Int aiCode) Native Global
Function SetOverride(Int aiCode, Bool bValue) Native Global