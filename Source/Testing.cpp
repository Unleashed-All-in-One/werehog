HOOK(void, __fastcall, testCHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{

	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (inputPtr->IsTapped(Sonic::eKeyState_DpadLeft))
	{

	}
	originaltestCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
}


void Testing::Install()
{

}