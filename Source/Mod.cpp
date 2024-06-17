extern "C" __declspec(dllexport) void Init(ModInfo * modInfo)
{
#if _DEBUG
	MessageBoxA(NULL, "Attach Debugger", "SUC WEREHOG", 0);
#endif

	//System
	Configuration::Read();
	Time::registerPatches();
	EvilGlobal::initializeValues();
	ArchiveTreePatcher::applyPatches();
	CustomAnimationManager::registerPatches();
	ClassicPluginExtensions::registerPatches();
	XMLParser::Install(modInfo->CurrentMod->Path);

	//EvilStateMachine
	EnemyBase::registerPatches();
	EvilSonic::registerPatches();


	Testing::registerPatches();
}