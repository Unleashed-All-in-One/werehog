extern "C" __declspec(dllexport) void Init(ModInfo * modInfo)
{
	MessageBoxA(NULL, "", "", 0);

	Configuration::Read();
	Time::Install();
	//std::string modDir = modInfo->CurrentMod->Path;
	/*size_t pos = modDir.find_last_of("\\/");
	if (pos != std::string::npos)
	{
		modDir.erase(pos + 1);
	}*//*
	Configuration::modPath = modDir + STAGE_LIST_FILE;*/
	
	//ArchivePatcher::Install();
	CustomAnimationManager::Install();
	ClassicPluginExtensions::Install();
	EnemyBase::Install();
	XMLParser::Install(modInfo->CurrentMod->Path);
	EvilSonic::Install();
	Testing::Install();
}
extern "C" void __declspec(dllexport) OnFrame()
{
	/*auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (inputPtr->IsTapped(Sonic::eKeyState_Y))
		Time::freeze = !Time::freeze;*/
}