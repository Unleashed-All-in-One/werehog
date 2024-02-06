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
    

}

