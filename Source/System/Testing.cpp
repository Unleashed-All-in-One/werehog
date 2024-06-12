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
//int __stdcall GAMEOBJECT_ADDCOLLIDER(Sonic::CObjectBase *gameObject, int stringSymbolName, int havokShape, int staticAddress, char flagA, int isContactPhantom)

HOOK(int, __stdcall, AddCollider, 0x000D5E090, DWORD* gameObject, const Hedgehog::Base::CStringSymbol& symbol, DWORD* havokShape, int* staticAdd, char flagA, int isContact)
{
	std::string check(symbol.GetValue());
	if (check == "VolumeEventTrigger")
	{
		printf("t");
	}
	return originalAddCollider(gameObject, symbol, havokShape, staticAdd, flagA, isContact);
}
void Testing::registerPatches()
{
	//INSTALL_HOOK(AddCollider);
	//INSTALL_HOOK(Hedgehog_Base_CSharedString_operator);
}