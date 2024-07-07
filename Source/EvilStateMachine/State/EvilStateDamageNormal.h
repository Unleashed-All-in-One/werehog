#pragma once
namespace Evil
{
	class CStateDamageNormal : public Sonic::Player::CPlayerSpeedContext::CStateSpeedBase
	{
	public:
		static constexpr const char* ms_StateName = "EvilDamageNormal";
		float lastFrame;
		SharedPtrTypeless soundHandle;
		void EnterState() override
		{
			Common::PlaySoundStaticCueName(soundHandle, "V_WHG_014");
			GetContext()->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
			GetContext()->ChangeAnimation("Evilsonic_damageMB");
			EvilGlobal::canExecuteAttacks = false;
			lastFrame = -1;
			GetContext()->SetStateFlag(Sonic::Player::CPlayerSpeedContext::eStateFlag_IgnorePadInput, true);
		}
		void ForceExit()
		{
			EvilGlobal::canExecuteAttacks = true;
			GetContext()->m_pPlayer->m_StateMachine.ChangeState("Stand");
			GetContext()->SetStateFlag(Sonic::Player::CPlayerSpeedContext::eStateFlag_IgnorePadInput, false);
		}
		void UpdateState() override
		{
			if (!GetContext()->m_Grounded)
			{
				ForceExit();
				return;
			}

			const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
			GetContext()->m_pPlayer->SendMessageImm(GetContext()->m_pPlayer->m_ActorID, spAnimInfo);


			if (spAnimInfo->m_Frame <= lastFrame)
				ForceExit();
			lastFrame = spAnimInfo->m_Frame;
		}
		void LeaveState()
		{
			EvilGlobal::canExecuteAttacks = true;
			GetContext()->SetStateFlag(Sonic::Player::CPlayerSpeedContext::eStateFlag_IgnorePadInput, false);
		}
	};
}