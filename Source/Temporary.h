#pragma once

static const int* pColID_BasicTerrain = reinterpret_cast<int*>(0x01E0AFAC);
static const int* pColID_Unknown1 = reinterpret_cast<int*>(0x01E0AF90);
static const int* pColID_Common = reinterpret_cast<int*>(0x01E0AF30);
static const int* pColID_PlayerEvent = reinterpret_cast<int*>(0x01E0AFD8);
static const int* pColID_Unknown2 = reinterpret_cast<int*>(0x01E0AFF4);
static const int* pColID_Unknown3 = reinterpret_cast<int*>(0x01E0AFF0);
static const int* pColID_Unknown4 = reinterpret_cast<int*>(0x1E0AFBC);
static const int* pColID_Unknown5 = reinterpret_cast<int*>(0x1E0BE30);
static const int* pColID_Unknown6 = reinterpret_cast<int*>(0x01E0BE28);
static const int* pColID_Unknown7 = reinterpret_cast<int*>(0x01E0BE18);
static const int* pColID_Unknown8 = reinterpret_cast<int*>(0x01E0BE34);
static const int* pColID_ObjectPhysics1 = reinterpret_cast<int*>(0x1E5E79C);
static const int* pColID_ObjectPhysics2 = reinterpret_cast<int*>(0x01E5E79C);
static const int* pColID_ObjectPhysics3 = reinterpret_cast<int*>(0x1E5E7B4);
static const int* pColID_ObjectPhysics4 = reinterpret_cast<int*>(0x1E5E804);
static const int* pColID_ObjectPhysics5 = reinterpret_cast<int*>(0x1E5E754);
static const int* pColID_ObjectPhysics6 = reinterpret_cast<int*>(0x01E61D24);
// TODO: Move to somewhere else, these are StringMap/StringEnum's
struct MsgChangePlayer : public hh::fnd::MessageTypeSet
{
public:
    HH_FND_MSG_MAKE_TYPE(0x0167F870);

    size_t m_PlayerType;

    MsgChangePlayer(size_t playerType) : m_PlayerType(playerType) {}
};
struct MsgResetCamera : public hh::fnd::MessageTypeSet
{
public:
    HH_FND_MSG_MAKE_TYPE(0x016821B0);

    MsgResetCamera() {}
};
//struct MsgChangeHudMode : public hh::fnd::MessageTypeSet
//{
//public:
//	HH_FND_MSG_MAKE_TYPE(0x016803AC);
//
//	size_t m_PlayerType;
//
//	MsgChangeHudMode(size_t playerType) : m_PlayerType(playerType) {}
//};
namespace Sonic::Player
{

    class IPosture : public Hedgehog::Universe::CStateMachineBase::CStateBase
    {
    public:
        Hedgehog::Base::CSharedString m_ClassName;

        IPosture(const bb_null_ctor&) : CStateBase(bb_null_ctor{}) {}
        IPosture(const Hedgehog::Base::CSharedString& name) : m_ClassName(name) {}
        //IPosture() : IPosture(bb_null_ctor{}) {}
    };

    class CPlayerSpeedPostureCommon : public IPosture
    {
    public:
        float m_Unk1 = 0.0f;
        float m_Unk2 = 0.0f;
        float m_Unk3 = 0.0f;
        Hedgehog::Math::CVector m_Vector = Hedgehog::Math::CVector(0, 0, 0);

        CPlayerSpeedPostureCommon(const bb_null_ctor&) : IPosture(bb_null_ctor{}) {}
        //CPlayerSpeedPostureCommon() : CPlayerSpeedPostureCommon(bb_null_ctor{})

        CPlayerSpeedPostureCommon() : IPosture("CPlayerSpeedPostureCommon")
        {
            *(int*)this = 0x016D3B6C;
        }

        CPlayerSpeedPostureCommon(const Hedgehog::Base::CSharedString& name) : IPosture(name)
        {
            *(int*)this = 0x016D3B6C;
        }
    };

    class CPlayerSpeedPosture3DCommon : public CPlayerSpeedPostureCommon
    {
    public:
        CPlayerSpeedPosture3DCommon(const bb_null_ctor&) : CPlayerSpeedPostureCommon(bb_null_ctor{}) {}

        CPlayerSpeedPosture3DCommon() : CPlayerSpeedPostureCommon("CPlayerSpeedPosture3DCommon")
        {
            *(int*)this = 0x016D392C;
        }

        CPlayerSpeedPosture3DCommon(const Hedgehog::Base::CSharedString& name) : CPlayerSpeedPostureCommon(name)
        {
            *(int*)this = 0x016D392C;
        }

        void UpdateState() override
        {
            BB_FUNCTION_PTR(void, __thiscall, MovementRoutine, 0x00E37FD0, void* This);
            MovementRoutine(this);
        }

    };
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
        void EnterState() override
        {
            FUNCTION_PTR(char, __thiscall, DEF010, 0XDEF010, Sonic::Player::CSonicStateStartCrouching * This);
            DEF010(this);

        }
        void UpdateState() override
        {
            printf("running");
            DebugDrawText::log("TEST = {0}", 5);
        }
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

