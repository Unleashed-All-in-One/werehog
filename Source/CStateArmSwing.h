#pragma once
class CStateArmSwing : public Sonic::Player::CPlayerSpeedContext::CStateSpeedBase
{
	float werehogArmHoming_timer;
	bool isDoingHoming;
	SharedPtrTypeless soundArmStretch;
	Hedgehog::math::CVector posStartArm;
public:
	static constexpr const char* ms_StateName = "EvilArmSwing";
	static boost::shared_ptr<Sonic::CMatrixNodeTransform> target;

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
		if (target == nullptr)
		{
			DebugDrawText::log("Null armswing target!!! Please fix!");
			context->ChangeState("Jump");
		}
	}
	void UpdateState() override
	{
		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];

		DebugDrawText::log("CanDo", 0);
		
			if (werehogArmHoming_timer >= 1)
			{
				werehogArmHoming_timer = 0;
			}
			werehogArmHoming_timer += Configuration::getDeltaTime();

			GetContext()->m_spMatrixNode->m_Transform.SetPosition(
				Hedgehog::math::CVector(
					Common::lerpUnclampedf(posStartArm.x(), target->m_Transform.m_Position.x(), easeInOutQuart(werehogArmHoming_timer / 1)),
					Common::lerpUnclampedf(posStartArm.y(), target->m_Transform.m_Position.y(), easeInOutQuart(werehogArmHoming_timer / 1)),
					Common::lerpUnclampedf(posStartArm.z(), target->m_Transform.m_Position.z(), easeInOutQuart(werehogArmHoming_timer / 1))
				)
			);
			GetContext()->m_Velocity = Hedgehog::math::CVector(0, 0, 0);
		
	}
};