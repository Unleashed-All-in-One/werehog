

class CCameraController : public Hedgehog::Universe::TStateMachine<Sonic::CCamera>::TState
{
public:
	int m_Int1;
	int weirdNumber;
	int m_Int2;
	int field04;
	Hedgehog::Math::CQuaternion m_Quaternion1;
	int m_Int3;
	float m_Float1;
	int m_Int4;
	float m_Float2;
	Hedgehog::Math::CQuaternion m_Quaternion2;
	Hedgehog::Math::CQuaternion m_Quaternion3;
	int m_Int5;
	float m_Float3;
	int m_Int6;
	float m_Float4;
	Hedgehog::Math::CQuaternion m_Quaternion4;
	int m_Int7;
	CCameraController()
	{
		*reinterpret_cast<size_t*>(this) = 0x0169F600;
		//m_pStateMachine = Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera().get();
	}
	
};
struct MotionCameraSub1
{
	char field00;
	char field01;
	char field02;
};
class CMotionCameraController : public CCameraController
{
public:
	Hedgehog::Base::CSharedString m_CameraName;
	boost::shared_ptr<Hedgehog::Motion::CCameraMotionData> m_MotionData;
	int m_IntPointsA3;
	Hedgehog::math::CMatrix44 m_Matrix44;
	float m_PlaybackSpeed;
	MotionCameraSub1 m_Byte1;
	__declspec(align(2)) float m_FloatFromMotionData1;
	float m_FloatFromMotionData2;
	float m_FloatFromMotionData3Framerate;
	int m_Int1;
	int m_Int2;
	MotionCameraSub1 m_Unknown1;
	__declspec(align(2)) int m_Int3;


	CMotionCameraController(boost::shared_ptr<Hedgehog::Motion::CCameraMotionData> motionData, float playbackSpeed)
	{
		*reinterpret_cast<size_t*>(this) = 0x0169F994;
		m_PlaybackSpeed = playbackSpeed;
		//CameraMotionData *__stdcall Construct_Sonic::CMotionCameraController(CameraMotionData *a1, boost::Void *a2, int a3, int a4)
		
		//CMotionCameraControllerC(this, &motionData, 0, 1.0f);
	}
};
BB_ASSERT_OFFSETOF(CMotionCameraController, m_CameraName, 0xE0);
BB_ASSERT_OFFSETOF(CMotionCameraController, m_Int3, 0x150);
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
		}
		int m_Mode;
		BB_INSERT_PADDING(0x64);
		/*boost::shared_ptr<boost::shared_ptr<int>>*/ boost::shared_ptr<int> m_pMotionCameraController;

	};
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, m_Mode, 0x68);
	BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, m_pMotionCameraController, 0xD0);
	//BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, char6c, 0x6C);
	//BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, Time, 0x70);
	//BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, float74, 0x74);
	//BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, State, 0x78);
	//BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, AnimationName, 0x7C);
	//BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, matrix, 0x80);
	//BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, CameraController, 0xD0);
	//BB_ASSERT_OFFSETOF(CSonicStateStartCrouching, Mystery, 0x88);
}
Sonic::Player::CSonicStateStartCrouching* this_is_a_bad_hack_please_fix;

struct MsgPopCameraController : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01682168);
	int* m_pCameraController{};
	BB_INSERT_PADDING(0x5);	
	Hedgehog::base::CSharedString *	m_CameraName{};
	float m_TransitionTime{};
};
BB_ASSERT_OFFSETOF(MsgPopCameraController, m_pCameraController, 0x10);
BB_ASSERT_OFFSETOF(MsgPopCameraController, m_CameraName, 0x1C);
BB_ASSERT_OFFSETOF(MsgPopCameraController, m_TransitionTime, 0x20);

class containertest
{
public:
	CMotionCameraController* boost;
	containertest(CMotionCameraController* inboost){
		boost = inboost;
	}
};
struct MsgPushCameraController : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01682150);
	CMotionCameraController* cameraControler;
	boost::shared_ptr<CMotionCameraController>* camNonboost;
	DWORD* alwaysMissing;
	DWORD* dword1C;
	Hedgehog::Base::CSharedString* cameraName;
	DWORD* alwaysZero;
	int id;
	bool bool1;
	bool bool2;
	float zero;
	bool bool3;
};
BB_ASSERT_OFFSETOF(MsgPushCameraController, cameraControler, 0x10);
BB_ASSERT_OFFSETOF(MsgPushCameraController, camNonboost, 0x14);
BB_ASSERT_OFFSETOF(MsgPushCameraController, cameraName, 0x20);
BB_ASSERT_OFFSETOF(MsgPushCameraController, id, 0x28);
BB_ASSERT_OFFSETOF(MsgPushCameraController, alwaysZero, 0x24);
bool Extensions_cameraAnimTempExecuted;
int f = 1;
static inline BB_FUNCTION_PTR(void, __stdcall, CMotionCameraControllerC, 0x010F6730,
	CMotionCameraController* a1, boost::shared_ptr<Hedgehog::Motion::CCameraMotionData>* a2, int a3,
	int a4);
class CStartWerehogPosture : public Sonic::Player::CPlayerSpeedPosture3DCommon
{
public:
	static constexpr const char* ms_StateName = "StartCrouchingPosture";
	float prev;
	SharedPtrTypeless sound;
	bool isExiting;
	DWORD* camData;
	boost::shared_ptr<CMotionCameraController> sharedptr;
	CMotionCameraController* fff; boost::shared_ptr<Hedgehog::Motion::CCameraMotionData> motion;
	bool init;
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
		if(!init)
		{
			init = true;
			auto database = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spDatabase;
			hh::mot::CMotionDatabaseWrapper wrapper(database.get());
			motion = (wrapper.GetCameraMotionData("sn_start_normal_a"));
			//CameraMotionData *__stdcall Construct_Sonic::CMotionCameraController(CameraMotionData *a1, boost::Void *a2, int a3, int a4)
			fff = new CMotionCameraController(motion, 1);
			sharedptr = boost::make_shared<CMotionCameraController>(motion, 1);
			CMotionCameraControllerC(sharedptr.get(), &motion, 0, 1.0f);
			auto message1 = boost::make_shared<MsgPushCameraController>();
			
			message1->cameraControler = sharedptr.get();
			message1->id = 1005;
			message1->camNonboost = &sharedptr;
			message1->cameraName = (Hedgehog::base::CSharedString*)(0x013E0DC0);
			message1->zero = 30.0f;
			//message1->m_CameraName = (Hedgehog::base::CSharedString*)(0x013E0DC0);
			Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->SendMessage(Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_ActorID, message1);
			sharedptr->m_pStateMachine;
			return;
		}
		if(sharedptr != nullptr)
		{
			sharedptr->m_PlaybackSpeed = 1;
			sharedptr->UpdateState();			
		}
		const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
		GetContext()->m_pPlayer->SendMessageImm(GetContext()->m_pPlayer->m_ActorID, spAnimInfo);
		DebugDrawText::log(std::format("###START###_AnimFrame = {0}", spAnimInfo->m_Frame).c_str(), 0);
		//if it isnt playing the anim for some reason, force it to play now
		//void __stdcall PlaySonicCamAnim(SonicCrouchMotionCamThing *state, Hedgehog::Base::CSharedString *a2, int a3, Hedgehog::Base::CSharedString *camAnimName, float startPoint, CSonicStartCrouching *a6)
		if (spAnimInfo->m_Frame >= 85.0f && spAnimInfo->m_Frame < 87.0f)
		{
			if(!sound)
			Common::PlaySoundStaticCueName(sound, "V_WHG_012");
		}
		if (spAnimInfo->m_Frame == prev)
		{
			FUNCTION_PTR(char, __thiscall, F_DEF0A0, 0XDEF0A0, Sonic::Player::CSonicStateStartCrouching * This);
			Extensions_cameraAnimTempExecuted = true;
			auto message = boost::make_shared<MsgPopCameraController>();
			if(this_is_a_bad_hack_please_fix->m_pMotionCameraController != nullptr)
			{
				auto test = (int*)sharedptr.get();
				//void* t2 = *(void**)test;
				message->m_pCameraController = test;
			}
			else
				F_DEF0A0(this_is_a_bad_hack_please_fix);
			message->m_CameraName = (Hedgehog::base::CSharedString *)(0x013E0DC0);
			message->m_TransitionTime = 1.5f;
			//message->Interpolation = &f;
			//message->CAMERA = this_is_a_bad_hack_please_fix->m_pMotionCameraController.get();
			//message->CAMERA2 = &f;
			Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->SendMessage(Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_ActorID, message);
			GetContext()->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
			//Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_ActorID, boost::make_shared<MsgPopCameraController>());
			//Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_ActorID, boost::make_shared<MsgResetCamera>());
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
	/*if (!BlueBlurCommon::IsClassic())
	{*/
	
		return originalStateCrouch_End(This);
	//}
	//else
	//{
	//	if (!Extensions_cameraAnimTempExecuted)
	//	{
	//		this_is_a_bad_hack_please_fix = This;
	//		
	//		//Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_ActorID;
	//		//return originalStateCrouch_End(This);
	//	}
	//	
	//}	
	//why does this make the cam-anim work?????
	return 0;
}
struct FFF
{
	BYTE _gap[0x531];
	int m_Mode;
};
HOOK(char, __stdcall, Camtest, 0xDFCE30, Sonic::Player::CSonicStateStartCrouching* a2)
{
	if (BlueBlurCommon::IsClassic())
	{
		//modern calls 00DFC410
		//classic calls unset homing attack
		//2 = readygo crouch
		*((DWORD*)a2 + 1329) = 2;
		if (!Extensions_cameraAnimTempExecuted)
		{
			Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_PostureStateMachine.ChangeState<CStartWerehogPosture>();
			Extensions_cameraAnimTempExecuted = true;
		}
	}
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

	//if (!BlueBlurCommon::IsClassic())
	//{
		Extensions_AddTestState(context);
	//}
	
	return result;
}
HOOK(volatile signed __int32*, __fastcall, sub_10FA770, 0x10FA770, int* This, void* Edx, MsgPushCameraController* message)
{
	void* mes = *(void**)message->cameraControler;
	//void* mes2 = *(void**)message->camNonboost.get();
	return originalsub_10FA770(This, Edx, message);
}
//volatile signed __int32 *__thiscall sub_10FA490(int this, PopCameraControllerMessage *message)
HOOK(volatile signed __int32*, __fastcall, sub_10FA490, 0x10FA490, int* This, void* Edx, MsgPopCameraController* message)
{
	if(message->m_CameraName != nullptr)
	{
		DebugDrawText::log((*message->m_CameraName).c_str(), 10);
	}
	if(message->m_pCameraController != nullptr)
	{
		//void* table = *(void**)message->pController;
		printf("");		
	}
	return originalsub_10FA490(This, Edx, message);
}
//MotionCameraController *__stdcall Construct_Sonic::CMotionCameraController(MotionCameraController *a1, boost::Void *a2, int a3, int a4)

void ClassicPluginExtensions::registerPatches()
{
	INSTALL_HOOK(sub_10FA770);
	//INSTALL_HOOK(PlaySonicCamAnim);
	INSTALL_HOOK(Camtest);
	WRITE_JUMP(0x00DC69B1, TestJumpNew);
	//WRITE_JUMP(0x00EA3F85, TestJumpNew51); //messes with object physics
	//disable all of this below to disable classic cam anims
	INSTALL_HOOK(Extensions_InitializePlayer);
	INSTALL_HOOK(DEF010);
	INSTALL_HOOK(StateCrouch_End);
	INSTALL_HOOK(CSonicStartCrouching_StateUpdate);
}