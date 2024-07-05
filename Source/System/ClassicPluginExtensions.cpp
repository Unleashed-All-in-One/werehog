#include "ClassicPluginExtensions.h"
#include "..\EvilStateMachine\Posture\EvilPostureIntro.h"
#include "..\EvilStateMachine\Posture\EvilStateAttackPosture_byList.h"
#include "..\EvilStateMachine\State\EvilStateAttackAction_byList.h"
#include "..\EvilStateMachine\State\EvilStateArmSwing.h"
bool m_PlayedCameraIntro;
HOOK(int, __fastcall, ProcMsgRestart, 0xE76810, int* This, void* Edx, int* a2)
{
	m_PlayedCameraIntro = false;
	return originalProcMsgRestart(This, Edx, a2);
}
HOOK(char, __stdcall, SetupPlayerStageIntro, 0xDFCE30, void* StateCrouching)
{
	if (BlueBlurCommon::IsClassic())
	{
		//2 = readygo crouch
		//*((DWORD*)a2 + 1329) = 2;
		if (!m_PlayedCameraIntro)
		{
			Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_PostureStateMachine.ChangeState<Evil::CStartWerehogPosture>();
			m_PlayedCameraIntro = true;
		}
		return 1;
	}
	else
		return originalSetupPlayerStageIntro(StateCrouching);
}
void RegisterCustomStates(Sonic::Player::CPlayerSpeedContext* context)
{
	static bool added = false;
	if (added) return;

	if (!added)
	{
		//Postures
		context->m_pPlayer->m_PostureStateMachine.RegisterStateFactory<Evil::CStartWerehogPosture>();
		context->m_pPlayer->m_PostureStateMachine.RegisterStateFactory<Evil::CStateAttackAction_byList_Posture>();

		//States
		context->m_pPlayer->m_StateMachine.RegisterStateFactory<Evil::CStateAttackAction_byList>();
		context->m_pPlayer->m_StateMachine.RegisterStateFactory<Evil::CStateArmSwing>();
		//added = true;
	}
}
HOOK(void*, __fastcall, CPlayerCreator_CCreate_Execute, 0x00D96110, void* This)
{
	void* result = originalCPlayerCreator_CCreate_Execute(This);
	RegisterCustomStates(Sonic::Player::CPlayerSpeedContext::GetInstance());
	return result;
}
void __declspec(naked) ASM_ClassicPlayStageIntro()
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
void ClassicPluginExtensions::registerPatches()
{
	INSTALL_HOOK(ProcMsgRestart);
	INSTALL_HOOK(SetupPlayerStageIntro);
	INSTALL_HOOK(CPlayerCreator_CCreate_Execute);

	//Classic doesn't use the function that Modern uses to play the intros
	//which is why Classic doesn't have any "READY GO" or anything of the sort
	WRITE_JUMP(0x00DC69B1, ASM_ClassicPlayStageIntro);
}