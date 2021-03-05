Scriptname nPFT_MCM extends SKI_ConfigBase  

Int Property kPFT_All                = -1 AutoReadOnly
Int Property kPFT_Combat             = 0 AutoReadOnly
Int Property kPFT_TakingDamage       = 1 AutoReadOnly
Int Property kPFT_Location           = 2 AutoReadOnly
Int Property kPFT_GuardsPursuing     = 3 AutoReadOnly
Int Property kPFT_OverEncumbered     = 4 AutoReadOnly
Int Property kPFT_InAir              = 5 AutoReadOnly
Int Property kPFT_WorldspaceTravel   = 6 AutoReadOnly
Int Property kPFT_ScriptCondition    = 7 AutoReadOnly
Int Property kPFT_AIDriven           = 8 AutoReadOnly
Int Property kPFT_Unk01              = 9 AutoReadOnly
Int Property kPFT_Dragon             = 10 AutoReadOnly

bool Location_Enabled = true
bool OverEncumbered_Enabled = true
bool Combat_Enabled = true
bool TakingDamage_Enabled = true
bool Guards_Enabled = true
bool InAir_Enabled = false
bool WorldspaceTravel_Enabled = false
bool ScriptCondition_Enabled = false
bool Dragon_Enabled = false
bool AIDriven_Enabled = false

int swOID_Location_Enabled
int swOID_OverEncumbered_Enabled
int swOID_Combat_Enabled
int swOID_TakingDamage_Enabled
int swOID_Guards_Enabled
int swOID_InAir_Enabled
int swOID_WorldspaceTravel_Enabled
int swOID_ScriptCondition_Enabled
int swOID_Dragon_Enabled
int swOID_AIDriven_Enabled

string swDesc_Location = "From any location"
string swDesc_OverEncumbered = "When over-encumbered"
string swDesc_Combat = "While in combat"
string swDesc_TakingDamage = "While taking damage"
string swDesc_Guards = "While pursued by guards"
string swDesc_InAir = "While jumping or falling"
string swDesc_WorldspaceTravel = "Allow worldspace fast travel"
string swDesc_ScriptCondition = "When disabled via scripts/console"
string swDesc_Dragon = "Remove dragon riding restrictions (caution)"
string swDesc_AIDriven = "Allow when player AI driven"

bool pluginLoaded = false
bool _initialized = false

function OnOptionSelect(int option)
	if option == swOID_Location_Enabled
		Location_Enabled = !Location_Enabled
		self.SetToggleOptionValue(option, Location_Enabled, false)
		UnlimitedFastTravel.SetOverride(self.kPFT_Location, Location_Enabled)
	elseif option == swOID_OverEncumbered_Enabled
		OverEncumbered_Enabled = !OverEncumbered_Enabled
		self.SetToggleOptionValue(option, OverEncumbered_Enabled, false)
		UnlimitedFastTravel.SetOverride(self.kPFT_OverEncumbered, OverEncumbered_Enabled)
	elseif option == swOID_Combat_Enabled
		Combat_Enabled = !Combat_Enabled
		self.SetToggleOptionValue(option, Combat_Enabled, false)
		UnlimitedFastTravel.SetOverride(self.kPFT_Combat, Combat_Enabled)
	elseif option == swOID_TakingDamage_Enabled
		TakingDamage_Enabled = !TakingDamage_Enabled
		self.SetToggleOptionValue(option, TakingDamage_Enabled, false)
		UnlimitedFastTravel.SetOverride(self.kPFT_TakingDamage, TakingDamage_Enabled)
	elseif option == swOID_Guards_Enabled
		Guards_Enabled = !Guards_Enabled
		self.SetToggleOptionValue(option, Guards_Enabled, false)
		UnlimitedFastTravel.SetOverride(self.kPFT_GuardsPursuing, Guards_Enabled)
	elseif option == swOID_InAir_Enabled
		InAir_Enabled = !InAir_Enabled
		self.SetToggleOptionValue(option, InAir_Enabled, false)
		UnlimitedFastTravel.SetOverride(self.kPFT_InAir, InAir_Enabled)
	elseif option == swOID_WorldspaceTravel_Enabled
		WorldspaceTravel_Enabled = !WorldspaceTravel_Enabled
		self.SetToggleOptionValue(option, WorldspaceTravel_Enabled, false)
		UnlimitedFastTravel.SetOverride(self.kPFT_WorldspaceTravel, WorldspaceTravel_Enabled)
	elseif option == swOID_ScriptCondition_Enabled
		ScriptCondition_Enabled = !ScriptCondition_Enabled
		self.SetToggleOptionValue(option, ScriptCondition_Enabled, false)
		UnlimitedFastTravel.SetOverride(self.kPFT_ScriptCondition, ScriptCondition_Enabled)
	elseif option == swOID_Dragon_Enabled
		Dragon_Enabled = !Dragon_Enabled
		self.SetToggleOptionValue(option, Dragon_Enabled, false)
		UnlimitedFastTravel.SetOverride(self.kPFT_Dragon, Dragon_Enabled)
	elseif option == swOID_AIDriven_Enabled
		AIDriven_Enabled = !AIDriven_Enabled
		self.SetToggleOptionValue(option, AIDriven_Enabled, false)
		UnlimitedFastTravel.SetOverride(self.kPFT_AIDriven, AIDriven_Enabled)
	endif
endfunction

function OnPageReset(string page)
	self.SetTitleText(ModName)
	self.SetCursorFillMode(self.TOP_TO_BOTTOM)
		
	if !pluginLoaded
		self.AddHeaderOption("SKSE64 plugin not detected", OPTION_FLAG_DISABLED)
	else		
		self.AddHeaderOption("Allow fast travel")
		swOID_Combat_Enabled = self.AddToggleOption(swDesc_Combat, Combat_Enabled)
		swOID_TakingDamage_Enabled = self.AddToggleOption(swDesc_TakingDamage, TakingDamage_Enabled)
		swOID_Guards_Enabled = self.AddToggleOption(swDesc_Guards, Guards_Enabled)
		swOID_Location_Enabled = self.AddToggleOption(swDesc_Location, Location_Enabled)
		swOID_OverEncumbered_Enabled = self.AddToggleOption(swDesc_OverEncumbered, OverEncumbered_Enabled)
		swOID_InAir_Enabled = self.AddToggleOption(swDesc_InAir, InAir_Enabled)
		swOID_ScriptCondition_Enabled = self.AddToggleOption(swDesc_ScriptCondition, ScriptCondition_Enabled)
		self.AddEmptyOption()
		self.SetCursorPosition(1)
		self.AddHeaderOption("Additional options")
		swOID_WorldspaceTravel_Enabled = self.AddToggleOption(swDesc_WorldspaceTravel, WorldspaceTravel_Enabled)
		swOID_Dragon_Enabled = self.AddToggleOption(swDesc_Dragon, Dragon_Enabled)
		swOID_AIDriven_Enabled = self.AddToggleOption(swDesc_AIDriven, AIDriven_Enabled)
		;self.AddEmptyOption()
	endif	
endfunction

function RunChecks()
	pluginLoaded = UnlimitedFastTravel.GetVersion() > 0
endfunction

function ProcessPluginOptions()
	if _initialized
		UnlimitedFastTravel.SetOverride(self.kPFT_Combat, Combat_Enabled)
		UnlimitedFastTravel.SetOverride(self.kPFT_TakingDamage, TakingDamage_Enabled)
		UnlimitedFastTravel.SetOverride(self.kPFT_Location, Location_Enabled)
		UnlimitedFastTravel.SetOverride(self.kPFT_GuardsPursuing, Guards_Enabled)
		UnlimitedFastTravel.SetOverride(self.kPFT_OverEncumbered, OverEncumbered_Enabled)
		UnlimitedFastTravel.SetOverride(self.kPFT_InAir, InAir_Enabled)
		UnlimitedFastTravel.SetOverride(self.kPFT_WorldspaceTravel, WorldspaceTravel_Enabled)
		UnlimitedFastTravel.SetOverride(self.kPFT_ScriptCondition, ScriptCondition_Enabled)
		UnlimitedFastTravel.SetOverride(self.kPFT_Dragon, Dragon_Enabled)
		UnlimitedFastTravel.SetOverride(self.kPFT_AIDriven, AIDriven_Enabled)
	else
		_initialized = true
		Combat_Enabled = UnlimitedFastTravel.HasOverride(self.kPFT_Combat)
		TakingDamage_Enabled = UnlimitedFastTravel.HasOverride(self.kPFT_TakingDamage)
		Location_Enabled = UnlimitedFastTravel.HasOverride(self.kPFT_Location)
		Guards_Enabled = UnlimitedFastTravel.HasOverride(self.kPFT_GuardsPursuing)
		OverEncumbered_Enabled = UnlimitedFastTravel.HasOverride(self.kPFT_OverEncumbered)
		InAir_Enabled = UnlimitedFastTravel.HasOverride(self.kPFT_InAir)
		WorldspaceTravel_Enabled = UnlimitedFastTravel.HasOverride(self.kPFT_WorldspaceTravel)
		ScriptCondition_Enabled = UnlimitedFastTravel.HasOverride(self.kPFT_ScriptCondition)
		Dragon_Enabled = UnlimitedFastTravel.HasOverride(self.kPFT_Dragon)
		AIDriven_Enabled = UnlimitedFastTravel.HasOverride(self.kPFT_AIDriven)
	endif
endfunction

function DoInitialization()
	self.RunChecks()
	if pluginLoaded
		self.ProcessPluginOptions()
	endif
endfunction

function OnConfigInit()
	self.DoInitialization()
endfunction

function PostLoadGame()
	self.DoInitialization()
endfunction