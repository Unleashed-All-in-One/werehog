﻿#define WIN32_LEAN_AND_MEAN

#include <BlueBlur.h>
#include <BlueBlurCustom.h>

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

// HE1ML
#include <ModLoader.h>

// Other
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

//System
#include "Configuration.h"
#include "System\Time.h"
#include "System\Temporary.h"	
#include "System\Testing.h"	
#include "System\CustomAnimationManager.h"	
#include "System\ConversionAPI.h"
#include "System\XMLParser.h"
#include "System\ArchiveTreePatcher.h"
#include "System\EvilConfiguration.h"
#include "TemporaryUntilBB\CeramicSA_AnimatorPose.h"

#include "AttackHitbox.h"
#include "CameraMotionForBlueBlur.h"	/*
#include "EvilStateMachine\Posture\EvilPostureIntro.h"
#include "EvilStateMachine\Posture\EvilStateAttackPosture_byList.h"
#include "EvilStateMachine\State\EvilStateAttackAction_byList.h"
#include "EvilStateMachine\State\EvilStateArmSwing.h"*/