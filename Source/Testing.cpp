HOOK(void, __fastcall, testCHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{

	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (inputPtr->IsTapped(Sonic::eKeyState_DpadLeft))
	{

	}
	originaltestCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
}

//Hedgehog::Base::CSharedString *__thiscall Hedgehog::Base::CSharedString::operator=(Hedgehog::Base::CSharedString *this, Hedgehog::Base::CSharedString *a2)

HOOK(Hedgehog::Base::CSharedString*, __fastcall, Hedgehog_Base_CSharedString_operator, 0x00662010, const Hedgehog::Base::CSharedString& This, void* Edx, const Hedgehog::Base::CSharedString& a2)
{
	return originalHedgehog_Base_CSharedString_operator(This, Edx, a2);
}

void Testing::Install()
{
	INSTALL_HOOK(Hedgehog_Base_CSharedString_operator);
}