#pragma once
namespace Evil
{
	class CStartWerehogPosture : public Sonic::Player::CPlayerSpeedPosture3DCommon
	{
	private:
		float m_PreviousFrame;
		SharedPtrTypeless m_SoundHandle;
		bool m_IsLeavingPosture;
		boost::shared_ptr<Hedgehog::Motion::CCameraMotionData> motion;
		bool m_InitializedPosture;
	public:
		static constexpr const char* ms_StateName = "StartCrouchingPosture";


		int GetMainDisplayActorID()
		{
			Sonic::Player::CPlayerSpeedContext* playerSpeedContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			if (playerSpeedContext != nullptr)
			{
				static uint32_t functionPtr = 0xE71DC0;
				__asm
				{
					mov eax, playerSpeedContext
					call[functionPtr]
				}
			}
			else
				return -1;
		}
		Sonic::Player::CPlayerSpeedContext* GetContext() const
		{
			return static_cast<Sonic::Player::CPlayerSpeedContext*>(m_pContext);
		}
		void EnterState() override
		{
			GetContext()->ChangeAnimation("Evilsonic_start");
			GetContext()->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::eStateFlag_OutOfControl] = true;


			int e = GetMainDisplayActorID();
			if (e != -1)
			{
				Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(e, boost::make_shared<Sonic::Message::MsgStartEventHUD>());
			}
		}
		void UpdateState() override
		{
			const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
			GetContext()->m_pPlayer->SendMessageImm(GetContext()->m_pPlayer->m_ActorID, spAnimInfo);
			DebugDrawText::log(std::format("###START###_AnimFrame = {0}", spAnimInfo->m_Frame).c_str(), 0);

			if (!m_IsLeavingPosture)
			{
				if (!m_InitializedPosture)
				{
					m_InitializedPosture = true;
					auto database = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spDatabase;
					hh::mot::CMotionDatabaseWrapper wrapper(database.get());
					motion = wrapper.GetCameraMotionData("evil_camera");

					EvilGlobal::m_spMotionCameraController = boost::make_shared<Sonic::CMotionCameraController>(motion, 0.65f);
					EvilGlobal::m_spMotionCameraController->m_TransformMatrix = Sonic::Player::CPlayerSpeedContext::GetInstance()->m_StartTransform.m_Matrix.matrix();
					auto message1 = boost::make_shared<Sonic::Message::MsgPushCameraController>(EvilGlobal::m_spMotionCameraController, 1005);
					Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->SendMessage(Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_ActorID, message1);
					return;
				}
				if (EvilGlobal::m_spMotionCameraController != nullptr)
				{
					DebugDrawText::log("###CAM_ANIM_START###\n", 0);

					DebugDrawText::log(std::format("PlaybackSpeed: {0}", EvilGlobal::m_spMotionCameraController->m_PlaybackSpeed).c_str(), 0);
					DebugDrawText::log(std::format("m_FloatFromMotionData1: {0}", EvilGlobal::m_spMotionCameraController->m_FloatFromMotionData1).c_str(), 0);
					DebugDrawText::log(std::format("m_MaxFrameCount: {0}", EvilGlobal::m_spMotionCameraController->m_MaxFrameCount).c_str(), 0);
					DebugDrawText::log(std::format("m_ElapsedFrameCount: {0}", EvilGlobal::m_spMotionCameraController->m_ElapsedFrameCount).c_str(), 0);
					DebugDrawText::log(std::format("m_ElapsedTime: {0}", EvilGlobal::m_spMotionCameraController->m_ElapsedTime).c_str(), 0);
					DebugDrawText::log(std::format("m_Int3: {0}", EvilGlobal::m_spMotionCameraController->m_Int3).c_str(), 0);
					DebugDrawText::log(std::format("m_AnimationFramerate: {0}", EvilGlobal::m_spMotionCameraController->m_AnimationFramerate).c_str(), 0);
					DebugDrawText::log(std::format("m_Field236: {0}", EvilGlobal::m_spMotionCameraController->m_Field236).c_str(), 0);
					DebugDrawText::log(std::format("\n", EvilGlobal::m_spMotionCameraController->m_AnimationFramerate).c_str(), 0);

					//m_spMotionCameraController->m_PlaybackSpeed = 1;
					//m_spMotionCameraController->UpdateState();			
				}
				//Werehog scream on GO
				if (spAnimInfo->m_Frame >= 85.0f && spAnimInfo->m_Frame < 87.0f)
				{
					if (!m_SoundHandle)
					{
						Common::PlaySoundStaticCueName(m_SoundHandle, "V_WHG_012");
						EvilGlobal::m_spMotionCameraController->m_PlaybackSpeed = 0.5f;
					}
				}
				//Unleashed seems to kill the cam anim a little bit before the actual anim ends
				if (spAnimInfo->m_Frame >= 118 && spAnimInfo->m_Frame < 120)
				{
					if (EvilGlobal::m_spMotionCameraController.get() != nullptr)
					{
						auto message = boost::make_shared<Sonic::Message::MsgPopCameraController>();
						message->m_pCameraController = EvilGlobal::m_spMotionCameraController.get();
						message->m_CameraName = (Hedgehog::base::CSharedString*)(0x013E0DC0);
						message->m_TransitionTime = 2;
						Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->SendMessage(Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_ActorID, message);
					}
				}
				//If anim doesn't proceed then it means its over, finish the posture
				if (spAnimInfo->m_Frame == m_PreviousFrame)
				{
					GetContext()->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
					Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::eStateFlag_OutOfControl] = false;
					GetContext()->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::eStateFlag_OutOfControl] = false;
					m_IsLeavingPosture = true;
					return;
				}

				if (std::strstr(spAnimInfo->m_Name.c_str(), "Evilsonic_start") == nullptr)
					GetContext()->ChangeAnimation("Evilsonic_start");
			}
			m_PreviousFrame = spAnimInfo->m_Frame;
		}

		virtual void UnknownStateFunction(void* a1) {}
		BB_VIRTUAL_FUNCTION_PTR(void, TransformPlayer, 0x0E34550, (float, deltaTime), (bool, UpdateYaw))
	};
}