Scriptname nPFT_MCM extends SKI_ConfigBase  

Int Property kPFT_All                = -1 AutoReadOnly
Int Property kPFT_Combat             = 0 AutoReadOnly
Int Property kPFT_Location           = 1 AutoReadOnly
Int Property kPFT_GuardsPursuing     = 2 AutoReadOnly
Int Property kPFT_OverEncumbered     = 3 AutoReadOnly
Int Property kPFT_WorldspaceTravel   = 4 AutoReadOnly

bool Location_Enabled
bool OverEncumbered_Enabled
bool Combat_Enabled
bool Guards_Enabled
bool WorldspaceTravel_Enabled

int swOID_Location_Enabled
int swOID_OverEncumbered_Enabled
int swOID_Combat_Enabled
int swOID_Guards_Enabled
int swOID_WorldspaceTravel_Enabled

string swDesc_Location_Enabled = "From any location"
string swDesc_OverEncumbered_Enabled = "When over-encumbered"
string swDesc_Combat_Enabled = "While in combat"
string swDesc_Guards_Enabled = "While pursued by guards"
string swDesc_WorldspaceTravel_Enabled = "Allow worldspace fast travel"

bool pluginLoaded = false
bool _initialized = false

function OnOptionSelect(int option)
	if option == swOID_Location_Enabled
		Location_Enabled = !Location_Enabled
		self.SetToggleOptionValue(option, Location_Enabled, false)
		UnrestrictedFastTravel.SetOverride(self.kPFT_Location, Location_Enabled)
	elseif option == swOID_OverEncumbered_Enabled
		OverEncumbered_Enabled = !OverEncumbered_Enabled
		self.SetToggleOptionValue(option, OverEncumbered_Enabled, false)
		UnrestrictedFastTravel.SetOverride(self.kPFT_OverEncumbered, OverEncumbered_Enabled)
	elseif option == swOID_Combat_Enabled
		Combat_Enabled = !Combat_Enabled
		self.SetToggleOptionValue(option, Combat_Enabled, false)
		UnrestrictedFastTravel.SetOverride(self.kPFT_Combat, Combat_Enabled)
	elseif option == swOID_Guards_Enabled
		Guards_Enabled = !Guards_Enabled
		self.SetToggleOptionValue(option, Guards_Enabled, false)
		UnrestrictedFastTravel.SetOverride(self.kPFT_GuardsPursuing, Guards_Enabled)
	elseif option == swOID_WorldspaceTravel_Enabled
		WorldspaceTravel_Enabled = !WorldspaceTravel_Enabled
		self.SetToggleOptionValue(option, WorldspaceTravel_Enabled, false)
		UnrestrictedFastTravel.SetOverride(self.kPFT_WorldspaceTravel, WorldspaceTravel_Enabled)
	endif
endfunction

function OnPageReset(string page)
	self.SetCursorFillMode(self.TOP_TO_BOTTOM)
	self.SetTitleText(ModName)
		
	if !pluginLoaded
		self.AddHeaderOption("SKSE64 plugin not detected", OPTION_FLAG_DISABLED)
	else		
		self.AddHeaderOption("Allow fast travel", 0)
		swOID_Location_Enabled = self.AddToggleOption(swDesc_Location_Enabled, Location_Enabled, 0)
		swOID_OverEncumbered_Enabled = self.AddToggleOption(swDesc_OverEncumbered_Enabled, OverEncumbered_Enabled, 0)
		swOID_Combat_Enabled = self.AddToggleOption(swDesc_Combat_Enabled, Combat_Enabled, 0)
		swOID_Guards_Enabled = self.AddToggleOption(swDesc_Guards_Enabled, Guards_Enabled, 0)
		self.AddEmptyOption()
		self.AddHeaderOption("Additional options", 0)
		swOID_WorldspaceTravel_Enabled = self.AddToggleOption(swDesc_WorldspaceTravel_Enabled, WorldspaceTravel_Enabled, 0)
		;self.AddEmptyOption()
	endif	
endfunction

function RunChecks()
	pluginLoaded = UnrestrictedFastTravel.GetVersion() > 0
endfunction

function ProcessPluginOptions()
	if _initialized
		UnrestrictedFastTravel.SetOverride(self.kPFT_Combat, Combat_Enabled)
		UnrestrictedFastTravel.SetOverride(self.kPFT_Location, Location_Enabled)
		UnrestrictedFastTravel.SetOverride(self.kPFT_GuardsPursuing, Guards_Enabled)
		UnrestrictedFastTravel.SetOverride(self.kPFT_OverEncumbered, OverEncumbered_Enabled)
		UnrestrictedFastTravel.SetOverride(self.kPFT_WorldspaceTravel, WorldspaceTravel_Enabled)
	else
		_initialized = true
		Combat_Enabled = UnrestrictedFastTravel.HasOverride(self.kPFT_Combat)
		Location_Enabled = UnrestrictedFastTravel.HasOverride(self.kPFT_Location)
		Guards_Enabled = UnrestrictedFastTravel.HasOverride(self.kPFT_GuardsPursuing)
		OverEncumbered_Enabled = UnrestrictedFastTravel.HasOverride(self.kPFT_OverEncumbered)
		WorldspaceTravel_Enabled = UnrestrictedFastTravel.HasOverride(self.kPFT_WorldspaceTravel)
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