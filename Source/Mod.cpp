extern "C" __declspec(dllexport) void Init(ModInfo * modInfo)
{
	MessageBoxA(NULL, "", "", 0);

	Configuration::Read();

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
//bool isClassic = 0;


extern "C" void __declspec(dllexport) OnFrame()
{
	/*if (Sonic::CInputState::GetInstance()->GetPadState().IsDown(Sonic::eKeyState_DpadRight))
	{
		isClassic = !isClassic;
		Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage("Player", boost::make_shared<MsgChangePlayer>(isClassic ? 1 : 0));
		Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage("Player", boost::make_shared<MsgChangeHudMode>(3));

	}*/
}