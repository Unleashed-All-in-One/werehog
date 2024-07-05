#pragma once
using namespace hh::math;
namespace Evil
{
	class CStateAttackAction_byList_Posture : public Sonic::Player::CPlayerSpeedPosture3DCommon
	{
	public:
		static constexpr const char* ms_StateName = "Evil_AttackAction_byListP";

		__declspec(noinline) void DoGravityThing(Sonic::Player::CPlayerSpeedContext* context, float deltaTime, float multiplier)
		{
			uint32_t func = 0x00E59C30;
			__asm
			{
				mov eax, context
				push multiplier
				push deltaTime
				call func
			}
		}

		Sonic::Player::CPlayerSpeedContext* GetContext() const
		{
			return static_cast<Sonic::Player::CPlayerSpeedContext*>(m_pContext);
		}
		void EnterState() override
		{
			GetContext()->m_Velocity = CVector(0, 0, 0);
		}
		void UpdateState() override
		{
			void* vtable = *(void**)this;
			BB_FUNCTION_PTR(void, __thiscall, MovementRoutine, 0x00E37FD0, void* This);
			//if(isGrounded)
			MovementRoutine(this);
			auto context = GetContext();
			const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			Common::ClampFloat(EvilGlobal::parameters->attackVelocityDivider, 0.001f, 100);
			CVector velocityWithoutY = playerContext->m_Velocity;
			velocityWithoutY.y() = 0;
			playerContext->m_spMatrixNode->m_Transform.SetPosition(playerContext->m_spMatrixNode->m_Transform.m_Position += (velocityWithoutY / EvilGlobal::parameters->attackVelocityDivider));
			//int __stdcall PostMovement(CSonicContext *sonicContext)

			FUNCTION_PTR(int, __stdcall, PostMov, 0x00E63530, void* context);
			//DoGravityThing(playerContext, deltaTime, PI);
			//SetsHighSpeedVectorAndAppliesGravity(playerContext);
		}
		virtual void UnknownStateFunction(void* a1) {}
		BB_VIRTUAL_FUNCTION_PTR(void, TransformPlayer, 0x0E34550, (float, deltaTime), (bool, UpdateYaw))
	};
}