#define WIN32_LEAN_AND_MEAN

#include <BlueBlur.h>

// Detours
#include <Windows.h>
#include "detours\include\detours.h"

// std
#include <stdint.h>
#include <stdio.h>
#include <array>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <filesystem>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <algorithm>	
#include <stdio.h>

// Other
#include <ModLoader.h>
#include <INIReader.h>
#include <json/json.h>
#include <Helpers.h>
#include <Common.h>
#include <StringHelper.h>
#include <rapidxml/rapidxml.hpp>
#include <winuser.h>

#include <HavokEssentials.h>
#include "ScoreGenerationsAPI.h"


#define DEBUG_DRAW_TEXT_DLL_IMPORT	
#include "DebugDrawText.h"	
#include "Temporary.h"	
#include "Testing.h"	
#include "EnemyBase.h"	
#include "CustomAnimationManager.h"	
#include "ClassicPluginExtensions.h"	

#include "ArchiveTreePatcher.h"
#include "Configuration.h"
#include "XMLParser.h"
#include "EvilSonic.h"