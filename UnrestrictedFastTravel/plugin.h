#pragma once

#define PLUGIN_NAME                 "UnlimitedFastTravel"
#define PLUGIN_AUTHOR               "SlavicPotato"

#include "version.h"

#define PLUGIN_BASE_PATH            "Data\\SKSE\\Plugins"

constexpr char* PLUGIN_LOG_PATH = "\\My Games\\Skyrim Special Edition\\SKSE\\" PLUGIN_NAME ".log";
constexpr char* PLUGIN_CBP_CONFIG = PLUGIN_BASE_PATH "\\CBPConfig.txt";

#define MIN_SKSE_VERSION            RUNTIME_VERSION_1_5_39

#define Message(fmt, ...) _MESSAGE(__FILE__ ":" STR(__LINE__) " <" __FUNCTION__ "> " fmt, __VA_ARGS__)
#define MsgError(fmt, ...) _ERROR(__FILE__ ":" STR(__LINE__) " <" __FUNCTION__ "> " fmt, __VA_ARGS__)
#define MsgFatalError(fmt, ...) _FATALERROR(__FILE__ ":" STR(__LINE__) " <" __FUNCTION__ "> " fmt, __VA_ARGS__)
