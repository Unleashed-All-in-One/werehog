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
	XMLParser::Install(modInfo->CurrentMod->Path);
	evSonic::Install();
}
