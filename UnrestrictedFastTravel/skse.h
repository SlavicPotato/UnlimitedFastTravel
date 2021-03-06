#pragma once

namespace SKSE
{
    extern PluginHandle g_pluginHandle;

    extern SKSEMessagingInterface* g_messaging;
    extern SKSEPapyrusInterface* g_papyrus;

    extern bool Query(const SKSEInterface* skse, PluginInfo* info);
    extern bool Initialize(const SKSEInterface* skse);
}