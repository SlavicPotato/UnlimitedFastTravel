#include "pch.h"

static bool Initialize(const SKSEInterface* a_skse)
{
	gLog.Message(
		"Initializing '%s' version %s (runtime %u.%u.%u.%u)",
		PLUGIN_NAME,
		PLUGIN_VERSION_VERSTRING,
		GET_EXE_VERSION_MAJOR(a_skse->runtimeVersion),
		GET_EXE_VERSION_MINOR(a_skse->runtimeVersion),
		GET_EXE_VERSION_BUILD(a_skse->runtimeVersion),
		GET_EXE_VERSION_SUB(a_skse->runtimeVersion));

	if (!IAL::IsLoaded())
	{
		MsgFatalError("Address library could not be loaded");
		return false;
	}

	if (IAL::HasBadQuery())
	{
		MsgFatalError("One or more addresses could not be retrieved from the database");
		return false;
	}

	auto& skse = ISKSE::GetSingleton();

	if (!skse.QueryInterfaces(a_skse))
	{
		MsgFatalError("Could not query SKSE interfaces");
		return false;
	}

	if (!skse.CreateTrampolines(a_skse))
	{
		return false;
	}

	UFT::Initialize();

	return true;
}

extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface* a_skse, PluginInfo* a_info)
	{
		return ISKSE::GetSingleton().Query(a_skse, a_info);
	}

	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
		if (IAL::IsAE())
		{
			auto& iskse = ISKSE::GetSingleton();

			iskse.SetPluginHandle(a_skse->GetPluginHandle());
			iskse.OpenLog();
		}

		bool ret = Initialize(a_skse);

		if (!ret)
		{
			WinApi::MessageBoxError(
				PLUGIN_NAME,
				"Plugin initialization failed, see log for more info");
		}

		IAL::Release();

		return ret;
	}

	SKSEPluginVersionData SKSEPlugin_Version = {
		SKSEPluginVersionData::kVersion,

		MAKE_PLUGIN_VERSION(
			PLUGIN_VERSION_MAJOR,
			PLUGIN_VERSION_MINOR,
			PLUGIN_VERSION_REVISION),
		PLUGIN_NAME,
		PLUGIN_AUTHOR,
		"n/a",
		SKSEPluginVersionData::kVersionIndependent_AddressLibraryPostAE,
		{ RUNTIME_VERSION_1_6_318, RUNTIME_VERSION_1_6_323, 0 },
		0,
	};
};