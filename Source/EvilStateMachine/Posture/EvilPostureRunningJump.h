#pragma once
namespace Evil
{
	class CRunningJumpPosture : public Sonic::Player::CPlayerSpeedPosture3DCommon
	{
	private:
		float m_PreviousFrame;
		SharedPtrTypeless m_SoundHandle;
		bool m_IsLeavingPosture;
		boost::shared_ptr<Hedgehog::Motion::CCameraMotionData> motion;
		bool m_InitializedPosture;
	public:
		static constexpr const char* ms_StateName = "EvilRunningJump";


		Sonic::Player::CPlayerSpeedContext* GetContext() const
		{
			return static_cast<Sonic::Player::CPlayerSpeedContext*>(m_pContext);
		}
		void EnterState() override
		{
			GetContext()->ChangeAnimation("Evilsonic_dash_jumpS");
			GetContext()->AddVelocity(GetContext()->m_Velocity / 20);
		}
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
		void UpdateState() override
		{
			if(GetContext()->m_Velocity.y() <= 1)
			{
				GetContext()->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
				return;
			}
			GetContext()->m_spMatrixNode->m_Transform.SetPosition(GetContext()->m_spMatrixNode->m_Transform.m_Position +GetContext()->m_Velocity);
			TransformPlayer(Configuration::getDeltaTime(), true);
			DebugDrawText::log(std::format("WorldInput ({0}, {1}, {2})", GetContext()->m_WorldInput.x(), GetContext()->m_WorldInput.y(), GetContext()->m_WorldInput.z()).c_str(), 0);
			GetContext()->HandleVelocityChanged();
			GetContext()->HandleHorizontalOrVerticalVelocityChanged();
			DoGravityThing(GetContext(), Configuration::getDeltaTime(), 0);
		}
		//GetContext()->m_ppl
		virtual void UnknownStateFunction(void* a1) {}
		BB_VIRTUAL_FUNCTION_PTR(void, TransformPlayer, 0x0E34550, (float, deltaTime), (bool, UpdateYaw))
	};
}