extern "C" __declspec(dllexport) void Init(ModInfo * modInfo)
{
#if _DEBUG
	MessageBoxA(NULL, "Attach Debugger", "SUC WEREHOG", 0);
#endif

	Configuration::Read();
	Time::registerPatches();
	//std::string modDir = modInfo->CurrentMod->Path;
	/*size_t pos = modDir.find_last_of("\\/");
	if (pos != std::string::npos)
	{
		modDir.erase(pos + 1);
	}*//*
	Configuration::modPath = modDir + STAGE_LIST_FILE;*/
	
	//ArchivePatcher::registerPatches();
	EvilGlobal::initializeValues();
	CustomAnimationManager::registerPatches();
	ClassicPluginExtensions::registerPatches();
	EnemyBase::registerPatches();
	XMLParser::Install(modInfo->CurrentMod->Path);
	EvilSonic::registerPatches();
	Testing::registerPatches();
}
extern "C" void __declspec(dllexport) OnFrame()
{
	/*auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (inputPtr->IsTapped(Sonic::eKeyState_Y))
		Time::freeze = !Time::freeze;*/
}