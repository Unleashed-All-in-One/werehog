
Sonic::Player::CSonicStateStartCrouching* this_is_a_bad_hack_please_fix;
bool Extensions_cameraAnimTempExecuted;
HOOK(char, __fastcall, DEF010, 0xDEF010, Sonic::Player::CSonicStateStartCrouching* This)
{
	auto returned = originalDEF010(This);
	this_is_a_bad_hack_please_fix = This;
	return returned;
}
HOOK(void, __fastcall, DEF180, 0x00DEF180, Sonic::Player::CPlayerSpeedContext::CStateSpeedBase* This)
{
}
HOOK(char, __fastcall, DEF0A0, 0xDEF0A0, Sonic::Player::CSonicStateStartCrouching* This)
{
	this_is_a_bad_hack_please_fix = This;
	if (Extensions_cameraAnimTempExecuted)
	{
		return originalDEF0A0(This);
	}
	//why does this make the cam-anim work?????
	return 0;
}

HOOK(char, __stdcall, Camtest, 0xDFCE30, Sonic::Player::CPlayerSpeedContext::CStateSpeedBase* a2)
{
	//modern calls 00DFC410
	//classic calls unset homing attack
	//2 = readygo crouch
	*((DWORD*)a2 + 1329) = 2;
	return originalCamtest(a2);
}
void __declspec(naked) TestJumpNew()
{
	static uint32_t RedRingCollectedCheckReturnAddress = 0x00DC69B6;
	static uint32_t sub_E71A50 = 0xE71A50;
	static uint32_t sub_DFCE30 = 0xDFCE30;
	__asm
	{
		call[sub_E71A50]
		push    esi
		call[sub_DFCE30]
		jmp[RedRingCollectedCheckReturnAddress]
	}
}
void Extensions_AddTestState(Sonic::Player::CPlayerSpeedContext* context)
{
	static bool added = false;
	if (added) return;

	if (!added)
	{
		context->m_pPlayer->m_StateMachine.RegisterStateFactory<Sonic::Player::CSonicStateStartCrouching>();
		added = true;
	}
}
HOOK(void*, __fastcall, Extensions_InitializePlayer, 0x00D96110, void* This)
{
	void* result = originalExtensions_InitializePlayer(This);
	auto context = Sonic::Player::CPlayerSpeedContext::GetInstance();    // Hack: there's a better way to do this but whatever. This writes to the singleton anyway.

	Extensions_AddTestState(context);
	return result;
}
void ClassicPluginExtensions::Install()
{
	WRITE_MEMORY(0x015DBAA0, char, "evilsonic_dashS");
	INSTALL_HOOK(Camtest);
	WRITE_JUMP(0x00DC69B1, TestJumpNew);
	INSTALL_HOOK(Extensions_InitializePlayer);
	INSTALL_HOOK(DEF010);
	//INSTALL_HOOK(DEF0A0);
	//INSTALL_HOOK(DEF180);
}