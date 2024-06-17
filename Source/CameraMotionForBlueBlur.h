#pragma once
namespace Sonic
{
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
		}
	};

	class CMotionCameraController : public CCameraController
	{
	public:
		Hedgehog::Base::CSharedString m_CameraName;
		boost::shared_ptr<Hedgehog::Motion::CCameraMotionData> m_MotionData;
		int m_Field236;
		Hedgehog::math::CMatrix44 m_TransformMatrix;
		float m_PlaybackSpeed;
		BB_INSERT_PADDING(4);
		float m_FloatFromMotionData1;
		float m_MaxFrameCount;
		float m_AnimationFramerate;
		float m_ElapsedFrameCount;
		float m_ElapsedTime;
		BB_INSERT_PADDING(4);
		int m_Field336;

		CMotionCameraController(boost::shared_ptr<Hedgehog::Motion::CCameraMotionData> motionData, float playbackSpeed = 1.0f)
		{
			BB_FUNCTION_PTR(void, __stdcall, CMotionCameraControllerC, 0x010F6730,
				CMotionCameraController * a1, boost::shared_ptr<Hedgehog::Motion::CCameraMotionData>*a2, int a3,
				int a4);
			CMotionCameraControllerC(this, &motionData, 0, 1);
			this->m_PlaybackSpeed = playbackSpeed;
		}
	};
	BB_ASSERT_OFFSETOF(CMotionCameraController, m_CameraName, 0xE0);
	BB_ASSERT_OFFSETOF(CMotionCameraController, m_Field236, 0xEC);
	BB_ASSERT_OFFSETOF(CMotionCameraController, m_Field336, 0x150);
}

namespace Sonic::Message
{
	struct MsgPopCameraController : public Hedgehog::Universe::MessageTypeSet
	{
	public:
		HH_FND_MSG_MAKE_TYPE(0x01682168);
		CMotionCameraController* m_pCameraController;
		int m_Unk4;
		int m_Unk5;
		Hedgehog::base::CSharedString* m_CameraName;
		float m_TransitionTime{};
		bool m_Unk1;
		bool m_Unk2;
		bool m_Unk3;
	};
	BB_ASSERT_OFFSETOF(MsgPopCameraController, m_pCameraController, 0x10);
	BB_ASSERT_OFFSETOF(MsgPopCameraController, m_CameraName, 0x1C);
	BB_ASSERT_OFFSETOF(MsgPopCameraController, m_TransitionTime, 0x20);

	struct MsgPushCameraController : public Hedgehog::Universe::MessageTypeSet
	{
	public:
		HH_FND_MSG_MAKE_TYPE(0x01682150);
		CMotionCameraController* m_pCameraController;
		boost::shared_ptr<CMotionCameraController>* m_spCameraController;
		BB_INSERT_PADDING(0x8);
		Hedgehog::Base::CSharedString* m_Field32;
		BB_INSERT_PADDING(0x4);
		int m_CameraPriority;
		bool m_Field44;
		bool m_Field45;
		float m_Field46;
		int m_Field50;
		MsgPushCameraController(boost::shared_ptr<CMotionCameraController>& in_spCameraController, int in_CameraPriority)
		{
			m_spCameraController = &in_spCameraController;
			m_pCameraController = in_spCameraController.get();
			m_CameraPriority = in_CameraPriority;
			m_Field32 = (Hedgehog::base::CSharedString*)(0x013E0DC0);
		}
	};
	BB_ASSERT_OFFSETOF(MsgPushCameraController, m_pCameraController, 0x10);
	BB_ASSERT_OFFSETOF(MsgPushCameraController, m_spCameraController, 0x14);
	BB_ASSERT_OFFSETOF(MsgPushCameraController, m_Field32, 0x20);
	BB_ASSERT_OFFSETOF(MsgPushCameraController, m_CameraPriority, 0x28);
	BB_ASSERT_OFFSETOF(MsgPushCameraController, m_Field44, 0x2C);

	struct MsgStartEventHUD : public Hedgehog::Universe::MessageTypeSet
	{
		HH_FND_MSG_MAKE_TYPE(0x016804B0);
	};
}