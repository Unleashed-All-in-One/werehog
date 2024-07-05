#pragma once
#include "..\Posture\EvilStateAttackPosture_byList.h"

namespace Evil
{
	class CStateAttackAction_byList : public Sonic::Player::CPlayerSpeedContext::CStateSpeedBase
	{
		int m_LastActionIndex;
		float m_LastFrame;
		float m_LastTriggerIndex;
		Motion m_CurrentMotion;
		//something i like to call a little hack
		CVector ms_InitialVelocity;
		CVector ms_AlteredVelocity;
		std::string m_Posture;
		static constexpr float ms_DecelerationForce = 0.65f;
		static constexpr float ms_AccelerationForce = 1.4285715f; //pulled from unleashed
		std::vector<boost::shared_ptr<CAttackHitbox>> collision;
		SharedPtrTypeless sound;
		const char* prevAnim;
		SharedPtrTypeless genericEffect;
	public:
		static constexpr const char* ms_StateName = "Evil_AttackAction_byList";

		CVector GetForward()
		{
			auto context = GetContext();
			return (context->m_spMatrixNode->m_Transform.m_Rotation * Eigen::Vector3f::UnitZ());
		}
		void GenerateHitbox(CollisionParam param, int index)
		{
			auto newHitbox = boost::make_shared<CAttackHitbox>();
			collision.push_back(newHitbox);
			Sonic::CGameDocument::GetInstance()->AddGameObject(newHitbox);
			switch (param.BoneType)
			{
			case CollisionBoneType::RHand:
			{
				newHitbox->BindToBone("Hand_R");
				break;
			}
			case CollisionBoneType::LHand:
			{
				newHitbox->BindToBone("Hand_L");
				break;
			}
			case CollisionBoneType::RLeg:
			{
				newHitbox->BindToBone("Toe_R");
				break;
			}
			case CollisionBoneType::LLeg:
			{
				newHitbox->BindToBone("Toe_L");
				break;
			}
			case CollisionBoneType::Hips:
			{
				newHitbox->BindToBone("Hips");
				break;
			}
			case CollisionBoneType::Head:
			{
				newHitbox->BindToBone("Head");
				break;
			}
			case CollisionBoneType::MiddleLeg:
			{
				newHitbox->BindToBone("Toe_R");
				break;
			}
			}
			newHitbox->m_TimerMax = (param.EndFrame - param.StartFrame) / 30;
			newHitbox->hitboxName = std::format("{0}{1}", XMLParser::GetBoneNameFromCollisionParam((int)param.BoneType), index + 1);
		}
		void KillHitbox(std::string name)
		{
			for (size_t i = 0; i < collision.size(); i++)
			{
				if (collision[i]->hitboxName == name)
				{
					auto& col = collision[i];
					col->SendMessage(col->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
					collision.erase(collision.begin() + i);
					return;
				}
			}
		}
		void EnterState() override
		{
			auto context = GetContext();
			m_LastFrame = -1;
			m_LastActionIndex = 0;
			m_LastTriggerIndex = 0;
			ms_InitialVelocity = CVector(0, 0, 0);
			ms_AlteredVelocity = ms_InitialVelocity;
			context->m_Velocity = ms_InitialVelocity;
			context->m_pPlayer->m_PostureStateMachine.ChangeState<CStateAttackAction_byList_Posture>();
		}
		bool HasHitboxBeenSpawned(std::string name)
		{
			for (size_t i = 0; i < collision.size(); i++)
			{
				if (collision[i]->hitboxName == name)
					return true;
			}
			return false;
		}
		void Reset()
		{
			for (size_t i = 0; i < collision.size(); i++)
			{
				collision.at(i)->SendMessage(collision.at(i)->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			}
			DebugDrawText::log("RESET", 5);
			collision.clear();
		}
		void UpdateState() override
		{
			m_CurrentMotion = EvilGlobal::GetMotionFromName(EvilGlobal::lastAttackName);
			auto context = GetContext();
			const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
			context->m_pPlayer->SendMessageImm(context->m_pPlayer->m_ActorID, spAnimInfo);
			DebugDrawText::log(std::format("CSTATEATTACKACTION_AnimFrame = {0}", spAnimInfo->m_Frame).c_str(), 0);
			if (spAnimInfo->m_Frame <= m_LastFrame)
			{
				Reset();
			}
			//if it isnt playing the anim for some reason, force it to play now
			if (std::strstr(spAnimInfo->m_Name.c_str(), EvilGlobal::GetStateNameFromTable(EvilGlobal::lastAttackName).c_str()) == nullptr)
				context->ChangeAnimation(EvilGlobal::GetStateNameFromTable(EvilGlobal::lastAttackName).c_str());

			//if the current anim is playing and the frame of it is still the same after 2 frames, it means the animation has ended, change states.
			if (std::strstr(spAnimInfo->m_Name.c_str(), EvilGlobal::GetStateNameFromTable(EvilGlobal::lastAttackName).c_str()) != nullptr && spAnimInfo->m_Frame == m_LastFrame)
			{
				context->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
				context->ChangeState("Stand");
				EvilGlobal::shockwaveGameObject->SendMessage(EvilGlobal::shockwaveGameObject->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
				return;
			}
			if (EvilGlobal::shockwaveGameObject)
			{
				EvilGlobal::shockwaveGameObject->m_spMatrixNodeTransform->m_Transform.m_Position = context->m_spMatrixNode->m_Transform.m_Position + (context->m_spMatrixNode->m_Transform.m_Rotation * CVector(0, 0, 2));
			}

			//this is probably super overkill and could just be done using a for loop with no checks
			for (size_t i = 0; i < m_CurrentMotion.Collision.BoneInfo.size(); i++)
			{
				std::string hitboxName = std::format("{0}{1}", XMLParser::GetBoneNameFromCollisionParam((int)m_CurrentMotion.Collision.BoneInfo[i].BoneType), i + 1);

				if (spAnimInfo->m_Frame >= m_CurrentMotion.Collision.BoneInfo[i].StartFrame && !HasHitboxBeenSpawned(hitboxName))
				{
					GenerateHitbox(m_CurrentMotion.Collision.BoneInfo[i], i);
				}
				if (spAnimInfo->m_Frame >= m_CurrentMotion.Collision.BoneInfo[i].EndFrame && HasHitboxBeenSpawned(hitboxName))
				{
					KillHitbox(hitboxName);
				}

			}
			if (spAnimInfo->m_Frame >= m_CurrentMotion.MotionSpeed_FirstFrame && spAnimInfo->m_Frame < m_CurrentMotion.MotionSpeed_MiddleFrame)
			{
				if (m_CurrentMotion.MotionMoveSpeedRatio != 0 && m_CurrentMotion.MotionMoveSpeedRatio_H[0].FrameValue != 0)
				{
					float velocity = m_CurrentMotion.MotionMoveSpeedRatio / m_CurrentMotion.MotionMoveSpeedRatio_H[0].FrameValue;
					if (abs(velocity) != 0 && !std::isnan(velocity))
					{
						ms_AlteredVelocity = GetForward() * velocity;
						if (m_CurrentMotion.MotionMoveSpeedRatio_H_Y.size() > 0)
							ms_AlteredVelocity.y() = m_CurrentMotion.MotionMoveSpeedRatio_H_Y[0].FrameValue;
						context->m_Velocity = ms_AlteredVelocity;
					}
				}
			}
			else
				context->m_Velocity = ms_InitialVelocity;

			auto triggers = m_CurrentMotion.TriggerInfos;
			auto resources = m_CurrentMotion.ResourceInfos;
			bool skip = false;
			for (size_t i = m_LastTriggerIndex; i < triggers.Resources.size(); i++)
			{
				if (skip)
					break;
				if (spAnimInfo->m_Frame >= triggers.Resources[i].Frame.Start)
				{
					for (size_t x = 0; x < resources.Resources.size(); x++)
					{
						if (resources.Resources[x].ID == triggers.Resources[i].ResourceID)
						{
							m_LastTriggerIndex = i + 1;
							if (resources.Resources[x].Type == ResourceType::CSB)
							{
								if (!resources.Resources[x].Params.Cue.empty())
								{
									Common::PlaySoundStaticCueName(sound, resources.Resources[x].Params.Cue.c_str());
									skip = true;
									break;
								}
							}
							if (resources.Resources[x].Type == ResourceType::Effect)
							{
								//genericEffect
								auto bone = context->m_pPlayer->m_spCharacterModel->GetNode(triggers.Resources[x].NodeName.c_str());
								if (!genericEffect)
									Common::fCGlitterCreate(context->m_pPlayer->m_spContext.get(), genericEffect, &bone, resources.Resources[x].Params.FileName.c_str(), 1);
							}
						}
					}
				}
			}
			if (spAnimInfo->m_Frame >= m_CurrentMotion.MotionSpeed_MiddleFrame)
			{
				context->m_Velocity = ms_AlteredVelocity;
			}
			context->m_Velocity = ms_AlteredVelocity;
			ms_AlteredVelocity = CVector(ms_AlteredVelocity.x() * ms_DecelerationForce, 0, ms_AlteredVelocity.z() * ms_DecelerationForce);
			//if (m_CurrentMotion.MotionMoveSpeedRatio_H.size() > m_LastActionIndex)
			//{
			//	if ((spAnimInfo->m_Frame == m_CurrentMotion.MotionMoveSpeedRatio_H.at(m_LastActionIndex).FrameStart	)
			//		/*|| m_CurrentMotion.MotionMoveSpeedRatioFrameStart.at(m_LastActionIndex) == -1*/)
			//	{
			//		DebugDrawText::log(std::format("CSTATEATTACKACTION_MotionMoveSpeedRatioFrameStart = {0} ind:{1}", spAnimInfo->m_Frame, m_LastActionIndex).c_str(), 5);
			//		ms_AlteredVelocity = ms_InitialVelocity + (GetForward() * m_CurrentMotion.MotionMoveSpeedRatio_H.at(m_LastActionIndex).FrameValue);
			//		ms_AlteredVelocity = CVector(ms_AlteredVelocity.x(), ms_AlteredVelocity.y() + m_CurrentMotion.MotionMoveSpeedRatio_H.at(m_LastActionIndex).FrameValue_Y, ms_AlteredVelocity.z());
			//		m_LastActionIndex++;
			//	}
			//}

			m_LastFrame = spAnimInfo->m_Frame;

		}
	};
}