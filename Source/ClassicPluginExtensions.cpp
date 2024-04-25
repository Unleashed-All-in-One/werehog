

namespace Sonic::Player
{
	class CSonicStateStartCrouching : public Sonic::Player::CPlayerSpeedContext::CStateSpeedBase
	{

	public:
		static constexpr const char* ms_StateName = "StartCrouching";
		static inline uint32_t pCtor = 0x00DEF710;

		static void fCtor(CSonicStateStartCrouching* This)
		{
			__asm
			{
				mov eax, This
				call[pCtor]
			}
		}

		CSonicStateStartCrouching(const bb_null_ctor&) : Sonic::Player::CPlayerSpeedContext::CStateSpeedBase(bb_null_ctor{}) {}

		CSonicStateStartCrouching() : CSonicStateStartCrouching(bb_null_ctor{})
		{
			fCtor(this);

			Hedgehog::Base::CSharedString camAnimName = "sn_start_normal_b";
			Hedgehog::Base::CSharedString v24 = "StartEventDash";
			//HOOK(char, __fastcall, DEF010, 0xDEF010, Sonic::Player::CSonicStateStartCrouching * This)

			//CSonicStartCrouching *state, Hedgehog::Base::CSharedString *a2, int a3, Hedgehog::Base::CSharedString *camAnimName, float startPoint, CSonicStartCrouching *a6
			//FUNCTION_PTR(void, __stdcall, PlaySonicCamAnim, 0xDEF410, CSonicStateStartCrouching * state, const Hedgehog::Base::CSharedString * a2, int a3, const Hedgehog::Base::CSharedString * camAnim, float startPoint, CSonicStateStartCrouching * a6);
			//PlaySonicCamAnim(this, &v24, 0, &camAnimName, 0.0, this);
		}
		int m_Mode;
		char char6c;
		float Time;
		float float74;
		void* State;
		Hedgehog::Base::CSharedString AnimationName;
		Hedgehog::Math::CMatrix* matrix;
		void* CameraController;
		void* Mystery;
	};
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, m_Mode, 0x68);
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, char6c, 0x6C);
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, Time, 0x70);
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, float74, 0x74);
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, State, 0x78);
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, AnimationName, 0x7C);
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, matrix, 0x80);
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, CameraController, 0x84);
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, Mystery, 0x88);
	BB_ASSERT_SIZEOF(CSonicStateStartCrouching, 0x8C);
}
Sonic::Player::CSonicStateStartCrouching* this_is_a_bad_hack_please_fix;

struct MsgPopCameraController : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01682168);
	BYTE gap4[12];
	DWORD dword10;
	DWORD dword14;
	DWORD dword18;
	DWORD dword1C;
	DWORD dword20;
	BYTE byte24;
	BYTE byte25;
	BYTE byte26;
};
bool Extensions_cameraAnimTempExecuted;
class CStartWerehogPosture : public Sonic::Player::CPlayerSpeedPosture3DCommon
{
public:
	static constexpr const char* ms_StateName = "StartCrouchingPosture";
	float prev;
	SharedPtrTypeless sound;
	bool isExiting;
	Sonic::Player::CPlayerSpeedContext* GetContext() const
	{
		return static_cast<Sonic::Player::CPlayerSpeedContext*>(m_pContext);
	}
	void EnterState() override
	{
		GetContext()->ChangeAnimation("Evilsonic_start");
		GetContext()->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::eStateFlag_OutOfControl] = true;
	}
	void UpdateState() override
	{
		const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
		GetContext()->m_pPlayer->SendMessageImm(GetContext()->m_pPlayer->m_ActorID, spAnimInfo);
		DebugDrawText::log(std::format("###START###_AnimFrame = {0}", spAnimInfo->m_Frame).c_str(), 0);
		//if it isnt playing the anim for some reason, force it to play now
		
		if (spAnimInfo->m_Frame >= 85.0f && spAnimInfo->m_Frame < 87.0f)
		{
			if(!sound)
			Common::PlaySoundStaticCueName(sound, "V_WHG_012");
		}
		if (spAnimInfo->m_Frame == prev)
		{
			FUNCTION_PTR(char, __thiscall, F_DEF0A0, 0XDEF0A0, Sonic::Player::CSonicStateStartCrouching * This);
			Extensions_cameraAnimTempExecuted = true;
			F_DEF0A0(this_is_a_bad_hack_please_fix);
			GetContext()->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
			Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_ActorID, boost::make_shared<MsgPopCameraController>());
			Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_ActorID, boost::make_shared<MsgResetCamera>());
			Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::eStateFlag_OutOfControl] = false;
			isExiting = true;
			GetContext()->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::eStateFlag_OutOfControl] = false;
			return;
		}
		else
		{
			if (isExiting)
				return;
			if (std::strstr(spAnimInfo->m_Name.c_str(), "Evilsonic_start") == nullptr)
				GetContext()->ChangeAnimation("Evilsonic_start");
		}
		prev = spAnimInfo->m_Frame;
	}

	virtual void UnknownStateFunction(void* a1) {}
	BB_VIRTUAL_FUNCTION_PTR(void, TransformPlayer, 0x0E34550, (float, deltaTime), (bool, UpdateYaw))
};

HOOK(char, __fastcall, DEF010, 0xDEF010, Sonic::Player::CSonicStateStartCrouching* This)
{
	if (!BlueBlurCommon::IsClassic())
	{
		return originalDEF010(This);;
	}
	auto returned = originalDEF010(This);
	this_is_a_bad_hack_please_fix = This;
	return returned;
}
HOOK(void, __fastcall, CSonicStartCrouching_StateUpdate, 0x00DEF180, Sonic::Player::CPlayerSpeedContext::CStateSpeedBase* This)
{
	if (!BlueBlurCommon::IsClassic())
	{
		return originalCSonicStartCrouching_StateUpdate(This);
	}
}
HOOK(char, __fastcall, StateCrouch_End, 0xDEF0A0, Sonic::Player::CSonicStateStartCrouching* This)
{
	if (!BlueBlurCommon::IsClassic())
	{
		return originalStateCrouch_End(This);
	}
	this_is_a_bad_hack_please_fix = This;
	if (Extensions_cameraAnimTempExecuted)
	{
		//Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_ActorID;
		//return originalStateCrouch_End(This);
	}
	Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_PostureStateMachine.ChangeState<CStartWerehogPosture>();
	//why does this make the cam-anim work?????
	return 0;
}

HOOK(char, __stdcall, Camtest, 0xDFCE30, Sonic::Player::CPlayerSpeedContext::CStateSpeedBase* a2)
{
	if (!BlueBlurCommon::IsClassic())
	{
		return originalCamtest(a2);
	}
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
void __declspec(naked) TestJumpNew51()
{
	static uint32_t RedRingCollectedCheckReturnAddress = 0x00EA3F95;
	static uint32_t sub_E71A50 = 0xE71A50;
	static uint32_t sub_DFCE30 = 0xDFCE30;
	__asm
	{
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

		context->m_pPlayer->m_PostureStateMachine.RegisterStateFactory<CStartWerehogPosture>();
		//added = true;
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
	WRITE_JUMP(0x00EA3F85, TestJumpNew51);
	//disable all of this below to disable classic cam anims
	INSTALL_HOOK(Extensions_InitializePlayer);
	INSTALL_HOOK(DEF010);
	INSTALL_HOOK(StateCrouch_End);
	INSTALL_HOOK(CSonicStartCrouching_StateUpdate);
}