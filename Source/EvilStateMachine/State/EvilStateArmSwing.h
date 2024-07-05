#pragma once
namespace Evil
{
	class CStateArmSwing : public Sonic::Player::CPlayerSpeedContext::CStateSpeedBase
	{
		float werehogArmHoming_timer;
		bool isDoingHoming;
		SharedPtrTypeless soundArmStretch;
		Hedgehog::math::CVector posStartArm;
		float maximumTime = 0.7f;
	public:
		static constexpr const char* ms_StateName = "EvilArmSwing";
		static Hedgehog::math::CVector target;

		static float easeInOutQuart(float x)
		{
			return x < 0.5
				? (1 - sqrt(1 - pow(2 * x, 2))) / 2
				: (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
		}
		void EnterState() override
		{
			auto context = GetContext();
			context->m_Velocity = Hedgehog::math::CVector(0, 0, 0);
			Common::PlaySoundStaticCueName(soundArmStretch, "es_armstretch");
			posStartArm = context->m_spMatrixNode->m_Transform.m_Position;
			werehogArmHoming_timer = 0;
			target = SUConversionAPI::GetClosestSetObjectForArmswing();

		}
		void UpdateState() override
		{
			auto context = GetContext();
			auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
			DebugDrawText::log("CanDo", 0);
			if (werehogArmHoming_timer >= maximumTime)
			{
				werehogArmHoming_timer = 0;
				context->ChangeState(StateAction::Stand);
				return;
			}
			werehogArmHoming_timer += Configuration::getDeltaTime();

			context->m_spMatrixNode->m_Transform.SetPosition(
				Hedgehog::math::CVector(
					Common::lerpUnclampedf(posStartArm.x(), target.x(), easeInOutQuart(werehogArmHoming_timer / maximumTime)),
					Common::lerpUnclampedf(posStartArm.y(), target.y(), easeInOutQuart(werehogArmHoming_timer / maximumTime)),
					Common::lerpUnclampedf(posStartArm.z(), target.z(), easeInOutQuart(werehogArmHoming_timer / maximumTime))
				)
			);
			context->m_Velocity = Hedgehog::math::CVector(0, 0, 0);
		}
	};
}