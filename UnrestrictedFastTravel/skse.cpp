#include "pch.h"

namespace SKSE
{
    constexpr size_t MAX_TRAMPOLINE_BRANCH = 256;
    constexpr size_t MAX_TRAMPOLINE_CODEGEN = 512;

    PluginHandle g_pluginHandle = kPluginHandle_Invalid;

    SKSEMessagingInterface* g_messaging;
    SKSEPapyrusInterface* g_papyrus;

    bool Query(const SKSEInterface* skse, PluginInfo* info)
    {
        gLog.OpenRelative(CSIDL_MYDOCUMENTS, PLUGIN_LOG_PATH);
        gLog.SetLogLevel(IDebugLog::LogLevel::Debug);

        info->infoVersion = PluginInfo::kInfoVersion;
        info->name = PLUGIN_NAME;
        info->version = MAKE_PLUGIN_VERSION(
            PLUGIN_VERSION_MAJOR,
            PLUGIN_VERSION_MINOR,
            PLUGIN_VERSION_REVISION);

        if (skse->isEditor)
        {
            MsgFatalError("Loaded in editor, marking as incompatible");
            return false;
        }

        if (skse->runtimeVersion < MIN_SKSE_VERSION)
        {
            MsgFatalError("Unsupported runtime version %d.%d.%d.%d, expected >= %d.%d.%d.%d",
                GET_EXE_VERSION_MAJOR(skse->runtimeVersion),
                GET_EXE_VERSION_MINOR(skse->runtimeVersion),
                GET_EXE_VERSION_BUILD(skse->runtimeVersion),
                GET_EXE_VERSION_SUB(skse->runtimeVersion),
                GET_EXE_VERSION_MAJOR(MIN_SKSE_VERSION),
                GET_EXE_VERSION_MINOR(MIN_SKSE_VERSION),
                GET_EXE_VERSION_BUILD(MIN_SKSE_VERSION),
                GET_EXE_VERSION_SUB(MIN_SKSE_VERSION));
            return false;
        }

        g_pluginHandle = skse->GetPluginHandle();

        return true;
    }

    bool Initialize(const SKSEInterface* skse)
    {

        g_messaging = (SKSEMessagingInterface*)skse->QueryInterface(kInterface_Messaging);
        if (g_messaging == NULL) {
            MsgFatalError("Could not get messaging interface");
            return false;
        }

        if (g_messaging->interfaceVersion < 2) {
            MsgFatalError("Messaging interface too old (%d expected %d)", g_messaging->interfaceVersion, 2);
            return false;
        }

        g_papyrus = (SKSEPapyrusInterface*)skse->QueryInterface(kInterface_Papyrus);
        if (g_papyrus == NULL) {
            MsgFatalError("Couldn't get papyrus interface.");
            return false;
        }

        if (g_papyrus->interfaceVersion < 1)
        {
            MsgFatalError("Papyrus interface too old (%d expected %d)", g_papyrus->interfaceVersion, 1);
            return false;
        }

        auto iface = static_cast<SKSETrampolineInterface*>(skse->QueryInterface(kInterface_Trampoline));

        if (!Hook::InitBranchTrampoline(skse, iface, MAX_TRAMPOLINE_BRANCH))
        {
            MsgFatalError("Could not create branch trampoline.");
            return false;
        }

        if (!Hook::InitLocalTrampoline(skse, iface, MAX_TRAMPOLINE_CODEGEN))
        {
            MsgFatalError("Could not create codegen trampoline.");
            return false;
        }

        return true;
    }
}