#pragma once

#define PLUGIN_NAME                 "UnlimitedFastTravel"
#define PLUGIN_AUTHOR               "SlavicPotato"

#include "version.h"

#define PLUGIN_BASE_PATH            "Data\\SKSE\\Plugins"

constexpr char* PLUGIN_LOG_PATH = "My Games\\Skyrim Special Edition\\SKSE\\" PLUGIN_NAME ".log";

#define MIN_SKSE_VERSION            RUNTIME_VERSION_1_5_39

#define Msg(fmt, ...) gLog.Message(__FILE__ ":" STR(__LINE__) " <" __FUNCTION__ "> " fmt, __VA_ARGS__)
#define MsgError(fmt, ...) gLog.Error(__FILE__ ":" STR(__LINE__) " <" __FUNCTION__ "> " fmt, __VA_ARGS__)
#define MsgFatalError(fmt, ...) gLog.FatalError(__FILE__ ":" STR(__LINE__) " <" __FUNCTION__ "> " fmt, __VA_ARGS__)
