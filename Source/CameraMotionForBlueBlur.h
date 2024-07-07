#pragma once

//https://github.com/Ahremic/Sonic-Adventure-Gameplay/blob/ada4d9f658b418d248488175c9a2b4290ab96dbe/SAGameplay/Extra/BlueBlurExtensions.h
namespace Sonic
{
	static constexpr uint32_t pCombineCollisionCategory = 0x010DFD40;
	static void __declspec (naked) fCombineCollisionCategory()
	{
		__asm
		{
			mov eax, [esp]
			add esp, 4
			jmp[pCombineCollisionCategory]
		}
	}
	static inline BB_FUNCTION_PTR(int, __cdecl, fpCombineCollisionCategory, fCombineCollisionCategory, uint32_t in_Arg, ...);

	class CollisionID
	{
		// Its helpful to have these public in case I'm fucking something up.
	//public:
		static __int64 MakeBitField64(const int* array, int size)
		{
			BB_FUNCTION_PTR(__int64*, __cdecl, fpMakeBitfield64, 0x407DC0, __int64* in_pInt64, const int* in_pArray, int in_Size);

			__int64 value = 0;
			fpMakeBitfield64(&value, array, size);
			return value;
		}
		static int MakeID(__int64 in_BitField64)
		{
			BB_FUNCTION_PTR(int, __cdecl, fpMakeCollisionID, 0x010DFB30, unsigned __int16 zero, int Nullptr, unsigned __int8 one, __int64 bitFieldA, __int64 bitFieldB);
			return fpMakeCollisionID(0, 0, 1, MakeBitField64(nullptr, 0), in_BitField64);
		}
		static int MakeID(__int64 in_Self, __int64 in_Interactable)
		{
			BB_FUNCTION_PTR(int, __cdecl, fpMakeCollisionID, 0x010DFB30, unsigned __int16 zero, int Nullptr, unsigned __int8 one, __int64 bitFieldA, __int64 bitFieldB);
			return fpMakeCollisionID(0, 0, 1, in_Self, in_Interactable);
		}
		static int MakeID2(__int64 in_BitField64)
		{
			BB_FUNCTION_PTR(int, __cdecl, fpMakeCollisionID, 0x010DFB30, unsigned __int16 zero, int Nullptr, unsigned __int8 one, __int64 bitFieldA, __int64 bitFieldB);
			return fpMakeCollisionID(0, 0, 1, in_BitField64, MakeBitField64(nullptr, 0));
		}

	public:
		int m_Data;

		CollisionID(std::initializer_list<const int> InitList, bool reverseOrder = false)
		{
			if (reverseOrder)
				m_Data = MakeID(MakeBitField64(InitList.begin(), InitList.size()));
			else
				m_Data = MakeID2(MakeBitField64(InitList.begin(), InitList.size()));
		}

		CollisionID(std::initializer_list<const int> Self, std::initializer_list<const int> Interactable)
		{
			m_Data = MakeID(MakeBitField64(Self.begin(), Self.size()), MakeBitField64(Interactable.begin(), Interactable.size()));
		}

		CollisionID(int a, int b, int c)
		{
			int Ints[3] = { a, b, c };
			m_Data = MakeID(MakeBitField64(Ints, 3));
		}

		static CollisionID Combine(CollisionID in_ColID, int in_TypeID)
		{
			int combineCat = fpCombineCollisionCategory(in_ColID.m_Data, MakeBitField64(&in_TypeID, 1), MakeBitField64(nullptr, 0));
			return *reinterpret_cast<CollisionID*>(&combineCat);
		}

		template <typename... Args>
		CollisionID(Args... args)
		{
			const int* start = std::get<0>(std::make_tuple(&args...));
			m_Data = MakeID(MakeBitField64(start, sizeof...(args)));
		}

		operator int() const
		{
			return m_Data;
		}

		void Combine(int in_TypeID)
		{
			m_Data = Combine(*this, in_TypeID);
		}

		static inline const int& TypeStaticTerrain = *reinterpret_cast<int*>(0x01E5E750);
		static inline const int& TypeTerrain = *reinterpret_cast<int*>(0x01E5E754);
		static inline const int& TypePlayerTerrain = *reinterpret_cast<int*>(0x01E5E758);
		static inline const int& TypeCameraTerrain = *reinterpret_cast<int*>(0x01E5E75C);
		static inline const int& TypeIKTerrain = *reinterpret_cast<int*>(0x01E5E760);
		static inline const int& TypeIKTerrainStairsMove = *reinterpret_cast<int*>(0x01E5E764);
		static inline const int& TypeIKTerrainStairs = *reinterpret_cast<int*>(0x01E5E768);
		static inline const int& TypeNavigationTerrain = *reinterpret_cast<int*>(0x01E5E76C);
		static inline const int& TypeRagdollTerrain = *reinterpret_cast<int*>(0x01E5E770);
		static inline const int& TypeAttackTerrain = *reinterpret_cast<int*>(0x01E5E774);
		static inline const int& TypeBrokenPiece = *reinterpret_cast<int*>(0x01E5E778);
		static inline const int& TypeBreakable = *reinterpret_cast<int*>(0x01E5E77C);
		static inline const int& TypeInsulate = *reinterpret_cast<int*>(0x01E5E780);
		static inline const int& TypeSearchInsulate = *reinterpret_cast<int*>(0x01E5E784);
		static inline const int& TypePlayer = *reinterpret_cast<int*>(0x01E5E788);
		static inline const int& TypePlayerItem = *reinterpret_cast<int*>(0x01E5E78C);
		static inline const int& TypePlayerResident = *reinterpret_cast<int*>(0x01E5E790);
		static inline const int& TypePlayerAttack = *reinterpret_cast<int*>(0x01E5E794);
		static inline const int& TypePlayerPath = *reinterpret_cast<int*>(0x01E5E798);
		static inline const int& TypeEvent = *reinterpret_cast<int*>(0x01E5E79C);
		static inline const int& TypeEventItem = *reinterpret_cast<int*>(0x01E5E7A0);
		static inline const int& TypePath = *reinterpret_cast<int*>(0x01E5E7A4);
		static inline const int& Type2DPath = *reinterpret_cast<int*>(0x01E5E7A8);
		static inline const int& TypeForwardPath = *reinterpret_cast<int*>(0x01E5E7AC);
		static inline const int& TypeGrindPath = *reinterpret_cast<int*>(0x01E5E7B0);
		static inline const int& TypeGrindPathForSideJump = *reinterpret_cast<int*>(0x01E5E7B4);
		static inline const int& TypeDashPath = *reinterpret_cast<int*>(0x01E5E7B8);
		static inline const int& TypeFloatingPath = *reinterpret_cast<int*>(0x01E5E7BC);
		static inline const int& TypeAntiRollBackPath = *reinterpret_cast<int*>(0x01E5E7C0);
		static inline const int& TypeCamera = *reinterpret_cast<int*>(0x01E5E7C4);
		static inline const int& TypeWater = *reinterpret_cast<int*>(0x01E5E7C8);
		static inline const int& TypeFloat = *reinterpret_cast<int*>(0x01E5E7CC);
		static inline const int& TypeNaviMesh = *reinterpret_cast<int*>(0x01E5E7D0);
		static inline const int& TypeNPC = *reinterpret_cast<int*>(0x01E5E7D4);
		static inline const int& TypeNPCDamage = *reinterpret_cast<int*>(0x01E5E7D8);
		static inline const int& TypeRagDoll = *reinterpret_cast<int*>(0x01E5E7DC);
		static inline const int& TypeEventTrigger = *reinterpret_cast<int*>(0x01E5E7E0);
		static inline const int& TypeMoveObject = *reinterpret_cast<int*>(0x01E5E7E4);
		static inline const int& TypeEnemy = *reinterpret_cast<int*>(0x01E5E7E8);
		static inline const int& TypeEnemyShotInsulate = *reinterpret_cast<int*>(0x01E5E7EC);
		static inline const int& TypeEnemyNoDamage = *reinterpret_cast<int*>(0x01E5E7F0);
		static inline const int& TypeLockon = *reinterpret_cast<int*>(0x01E5E7F4);
		static inline const int& TypeLightSpeedDash = *reinterpret_cast<int*>(0x01E5E7F8);
		static inline const int& TypeItemPull = *reinterpret_cast<int*>(0x01E5E7FC);
		static inline const int& TypeReactRing = *reinterpret_cast<int*>(0x01E5E800);
		static inline const int& TypeKeepOffEnemy = *reinterpret_cast<int*>(0x01E5E804);
		static inline const int& TypeSwitchPushable = *reinterpret_cast<int*>(0x01E5E808);
		static inline const int& TypeWrongWay = *reinterpret_cast<int*>(0x01E5E80C);
		static inline const int& TypeSpikeContact = *reinterpret_cast<int*>(0x01E5E810);
		static inline const int& TypeSpikeIgnore = *reinterpret_cast<int*>(0x01E5E814);
		static inline const int& TypeABSRay = *reinterpret_cast<int*>(0x01E5E818);
		static inline const int& TypeSlidingColli = *reinterpret_cast<int*>(0x01E5E81C); // sic

	};
}

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