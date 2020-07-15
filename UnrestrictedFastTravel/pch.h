#ifndef PCH_H
#define PCH_H

#include <ShlObj.h>

#include <atomic>

#include "ext/ICommon.h"
#include "ext/IHook.h"
#include "ext/JITASM.h"
#include "ext/Patching.h"

#include "skse64/PluginAPI.h"
#include "skse64/GameReferences.h"
#include "skse64/GameEvents.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64/PapyrusArgs.h"

#define _UFT_ENABLE_MOSTLY_USELESS
#define _UFT_ENABLE_UNKNOWN

#include "plugin.h"
#include "skse.h"
#include "papyrus.h"
#include "uft.h"

#endif //PCH_H
