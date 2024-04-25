
float Configuration::m_deltaTime;
HOOK(void*, __fastcall, UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	Configuration::setDeltaTime(elapsedTime);
	return originalUpdateApplication(This, Edx, elapsedTime, a3);
}

void Configuration::Read()
{
	INSTALL_HOOK(UpdateApplication);
}
