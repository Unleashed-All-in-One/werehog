
using namespace hh::math;
using namespace Sonic;

enum WerehogState
{
	Normal,
	Dash,
	Guard
};

Sonic::Player::CPlayerSpeedContext::CStateSpeedBase* this_is_a_bad_hack_please_fix;
SharedPtrTypeless sound, soundUnleash, soundUnleashStart;
SharedPtrTypeless soundRegularJump;
SharedPtrTypeless indexParticle_L, indexParticle_R;
SharedPtrTypeless middleParticle_L, middleParticle_R;
SharedPtrTypeless pinkyParticle_L, pinkyParticle_R;
SharedPtrTypeless ringParticle_L, ringParticle_R;
SharedPtrTypeless thumbParticle_L, thumbParticle_R;
SharedPtrTypeless punch, referenceEffect;
SharedPtrTypeless shield;
SharedPtrTypeless genericEffect;
SharedPtrTypeless berserk[5];
std::vector<Sonic::EKeyState> currentButtonChain;
WerehogAttackNew attackCache;
bool canJump;
int jumpcount;
int comboAttackIndex;
int lastAttackIndex;
bool isUsingShield;

float tempTimerWalk;
float deltaTime;
float timerCombo;
float attackVelocityDivider = 1.5f;
float timerAttack;
float timerComboMax = 0.75f;
float timerDamage = 0.3f;
float timerDamageMax = 0.3f;
float timerAttackMax = 0.35f;
int comboProgress = 0;
float lifeWerehog = 5.0f;
bool unleashMode;
bool playingAttack;
bool isGrounded;
std::string lastAttackName;
Sonic::EKeyState lastTap;
bool cameraAnimTempExecuted = false;

//find a better way please
bool init = false;
boost::shared_ptr<Sonic::CGameObject3D> collision1;
boost::shared_ptr<Sonic::CGameObject3D> shockwaveGameObject;




void sub_E78310(Sonic::Player::CPlayer* player)
{
	uint32_t func = 0x00E78310;
	DWORD* result;
	__asm
	{
		mov     eax, player
		call func
	};
};
WerehogState currentState;

bool IsCurrentAnimationName(std::string in)
{
	auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
	playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
	return std::strstr(spAnimInfo->m_Name.c_str(), in.c_str()) != nullptr;
}
std::string GetStateNameFromTable(std::string in)
{
	for (size_t i = 0; i < XMLParser::animationTable.size(); i++)
	{
		if (XMLParser::animationTable[i].MotionName == in)
			return XMLParser::animationTable[i].FileName;
	}
}
Motion GetMotionFromName(std::string in)
{
	for (size_t i = 0; i < XMLParser::animationTable.size(); i++)
	{
		if (XMLParser::animationTable[i].MotionName == in)
			return XMLParser::animationTable[i];
	}
}
WerehogAttackNew GetAttackFromName(std::string in)
{
	auto motion = GetMotionFromName(lastAttackName);
	for (size_t i = 0; i < XMLParser::attacks.size(); i++)
	{
		if (XMLParser::attacks[i].MotionName == motion.MotionName)
			return XMLParser::attacks[i];
	}
}
void SetsHighSpeedVectorAndAppliesGravity(void* doc)
{
	uint32_t func = 0x00E4F100;
	DWORD* result;
	__asm
	{
		/*mov     eax, doc*/
		push doc
		call func
	};
};

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
		Common::ClampFloat(attackVelocityDivider, 0.001f, 100);
		CVector velocityWithoutY = playerContext->m_Velocity;
		velocityWithoutY.y() = 0;
		playerContext->m_spMatrixNode->m_Transform.SetPosition(playerContext->m_spMatrixNode->m_Transform.m_Position += (velocityWithoutY / attackVelocityDivider));
		//int __stdcall PostMovement(CSonicContext *sonicContext)

		FUNCTION_PTR(int, __stdcall, PostMov, 0x00E63530, void* context);
		//DoGravityThing(playerContext, deltaTime, PI);
		//SetsHighSpeedVectorAndAppliesGravity(playerContext);
	}
	virtual void UnknownStateFunction(void* a1) {}
	BB_VIRTUAL_FUNCTION_PTR(void, TransformPlayer, 0x0E34550, (float, deltaTime), (bool, UpdateYaw))
};
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

	const char* prevAnim;
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
		m_CurrentMotion = GetMotionFromName(lastAttackName);
		auto context = GetContext();
		const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
		context->m_pPlayer->SendMessageImm(context->m_pPlayer->m_ActorID, spAnimInfo);
		DebugDrawText::log(std::format("CSTATEATTACKACTION_AnimFrame = {0}", spAnimInfo->m_Frame).c_str(), 0);
		if (spAnimInfo->m_Frame <= m_LastFrame)
		{
			Reset();
		}
		//if it isnt playing the anim for some reason, force it to play now
		if (std::strstr(spAnimInfo->m_Name.c_str(), GetStateNameFromTable(lastAttackName).c_str()) == nullptr)
			context->ChangeAnimation(GetStateNameFromTable(lastAttackName).c_str());

		//if the current anim is playing and the frame of it is still the same after 2 frames, it means the animation has ended, change states.
		if (std::strstr(spAnimInfo->m_Name.c_str(), GetStateNameFromTable(lastAttackName).c_str()) != nullptr && spAnimInfo->m_Frame == m_LastFrame)
		{
			context->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
			context->ChangeState("Stand");
			shockwaveGameObject->SendMessage(shockwaveGameObject->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			return;
		}
		if (shockwaveGameObject)
		{
			shockwaveGameObject->m_spMatrixNodeTransform->m_Transform.m_Position = context->m_spMatrixNode->m_Transform.m_Position + (context->m_spMatrixNode->m_Transform.m_Rotation * CVector(0, 0, 2));
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
class TestState : public Sonic::Player::CPlayerSpeedContext::CStateSpeedBase
{
	float lastFrame = -1;
public:
	static constexpr const char* ms_StateName = "StartCrouching";

	CVector GetForward()
	{
		auto context = GetContext();
		return (context->m_spMatrixNode->m_Transform.m_Rotation * Eigen::Vector3f::UnitZ());
	}
	void EnterState() override
	{
		const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		//*(int*)this = 0x016D7648;
		const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
		playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
		playerContext->ChangeAnimation("Evilsonic_start");
		/*if(m_CurrentMotion.MotionMoveSpeedRatioFrameStart.at(m_LastActionIndex) == -1)
			ms_AlteredVelocity = GetForward() * m_CurrentMotion.MotionMoveSpeedRatioFrame.at(m_LastActionIndex);*/
	}
	void UpdateState() override
	{
		auto context = GetContext();
		const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
		context->m_pPlayer->SendMessageImm(context->m_pPlayer->m_ActorID, spAnimInfo);
		DebugDrawText::log(std::format("CSTATEATTACKACTION_AnimFrame = {0}", spAnimInfo->m_Frame).c_str(), 0);
		//if it isnt playing the anim for some reason, force it to play now
		if (spAnimInfo->m_Frame == lastFrame)
		{
			context->ChangeState("Stand");
			sub_E78310(context->m_pPlayer);
			cameraAnimTempExecuted = true;
			FUNCTION_PTR(char, __thiscall, DEF0A0, 0xDEF0A0, Sonic::Player::CPlayerSpeedContext::CStateSpeedBase * This);
			//^state end for the original
			DEF0A0(this_is_a_bad_hack_please_fix);
			//^im hooking into this function to get the statebase when it runs originally, since for some reason if i dont stop it from
			//executing, it'll instantly end the state the second its set
		}
		//void __thiscall sub_DEF180(CTempState *a1)
		/*FUNCTION_PTR(void, __thiscall, sub_DEF180, 0xDEF180, Sonic::Player::CPlayerSpeedContext::CStateSpeedBase * a1);
		sub_DEF180(this);*/
		lastFrame = spAnimInfo->m_Frame;
	}
	void LeaveState() override
	{
		////char, __fastcall, DEF0A0, 0xDEF0A0, 
		//FUNCTION_PTR(char, __thiscall, DEF0A0, 0xDEF0A0, Sonic::Player::CPlayerSpeedContext::CStateSpeedBase * This);
		//DEF0A0(this);
	}

};

// Func for adding the test state.
// Fun fact: declaring and assigning a STATIC variable in a func like this will only do this *once.*	
void AddTestState(Sonic::Player::CPlayerSpeedContext* context)
{
	static bool added = false;
	if (added) return;

	if (!added)
	{
		auto state = (Sonic::Player::CPlayerSpeedContext::CStateSpeedBase*)0x016D7648;

		context->m_pPlayer->m_StateMachine.RegisterStateFactory<CStateAttackAction_byList>();
		context->m_pPlayer->m_StateMachine.RegisterStateFactory<TestState>();
		context->m_pPlayer->m_PostureStateMachine.RegisterStateFactory<CStateAttackAction_byList_Posture>();
		added = true;
	}
}

void AddImpulse(CVector impulse, bool relative)
{
	auto context = Sonic::Player::CPlayerSpeedContext::GetInstance();
	if (!relative)
	{
		context->m_Velocity = impulse;
		return;
	}
	else
	{
		context->m_Velocity += impulse;
		return;
	}
}
void AddJumpThrust(CSonicContext* sonicContext, bool Condition)
{
	uint32_t func = 0x00E57C50;
	__asm
	{
		push Condition
		mov edi, sonicContext
		call func
	};
};
void PlayAnim(std::string name)
{
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
	playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
	const char* animName = name.c_str();
	playerContext->ChangeAnimation(animName);
}
float GetVelocity()
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (currentState == WerehogState::Guard)
		return 1;
	if (currentState == WerehogState::Normal && tempTimerWalk < 0.5f)
		return 2;
	if (currentState == WerehogState::Normal)
		return 5.5f;
	if (currentState == WerehogState::Dash)
		return 12;
}
void RegisterInputs()
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	auto state = inputPtr->TappedState;
	DebugDrawText::log(std::to_string(state).c_str(), 0);
	lastTap = inputPtr->DownState;
	DebugDrawText::log(std::to_string(lastTap).c_str(), 0);
	if (timerCombo < timerComboMax)
	{
		if (lastTap == eKeyState_A || state == eKeyState_X || state == eKeyState_Y)
			currentButtonChain.push_back(state);
		if (currentButtonChain.size() > 8)
		{
			currentButtonChain.erase(currentButtonChain.begin());
		}
		DebugDrawText::log("Current chain:", 0);
		for (size_t i = 0; i < currentButtonChain.size(); i++)
		{
			DebugDrawText::log(std::to_string(currentButtonChain.at(i)).c_str(), 0);
		}
	}
	else
		currentButtonChain.clear();
}
void DespawnParticlesHand()
{
	indexParticle_L.reset();
	middleParticle_L.reset();
	pinkyParticle_L.reset();
	ringParticle_L.reset();
	thumbParticle_L.reset();


	indexParticle_R.reset();
	middleParticle_R.reset();
	pinkyParticle_R.reset();
	ringParticle_R.reset();
	thumbParticle_R.reset();
	punch.reset();
}
void SpawnParticleOnHand(std::string glitterNameLeft, std::string glitterNameRight, std::string boneless)
{
	///
	///							O
	///						   /|\
	///						  / | \
	///	  glitterNameRight-> /  |  \ <- glitterNameLeft
	///							|
	///						   / \
	///						  /   \
	///					     /  v  \
	///						 boneless
	///		

	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	DespawnParticlesHand();
	if (!boneless.empty())
	{
		auto ref = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Reference");
		if (!referenceEffect)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), referenceEffect, &ref, boneless.c_str(), 1);
	}
	if (!glitterNameLeft.empty())
	{
		if (XMLParser::CLAWPARTICLE == glitterNameLeft)
		{
			auto index = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Index1_L");
			auto middle = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Middle1_L");
			auto pinky = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Pinky1_L");
			auto ring = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Ring1_L");
			auto thumb = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Thumb1_L");
			if (!indexParticle_L)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), indexParticle_L, &index, glitterNameLeft.c_str(), 1);
			if (!middleParticle_L)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), middleParticle_L, &middle, glitterNameLeft.c_str(), 1);
			if (!pinkyParticle_L)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), pinkyParticle_L, &pinky, glitterNameLeft.c_str(), 1);
			if (!ringParticle_L)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), ringParticle_L, &ring, glitterNameLeft.c_str(), 1);
			if (!thumbParticle_L)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), thumbParticle_L, &thumb, glitterNameLeft.c_str(), 1);
		}
		else
		{
			auto hand = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Arm09Sub_L");
			if (!punch)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), punch, &hand, glitterNameLeft.c_str(), 1);
		}
	}
	if (!glitterNameRight.empty())
	{
		if (XMLParser::CLAWPARTICLE == glitterNameRight)
		{
			auto index = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Index1_R");
			auto middle = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Middle1_R");
			auto pinky = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Pinky1_R");
			auto ring = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Ring1_R");
			auto thumb = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Thumb1_R");
			if (!indexParticle_R)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), indexParticle_R, &index, glitterNameRight.c_str(), 1);
			if (!middleParticle_R)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), middleParticle_R, &middle, glitterNameRight.c_str(), 1);
			if (!pinkyParticle_R)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), pinkyParticle_R, &pinky, glitterNameRight.c_str(), 1);
			if (!ringParticle_R)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), ringParticle_R, &ring, glitterNameRight.c_str(), 1);
			if (!thumbParticle_R)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), thumbParticle_R, &thumb, glitterNameRight.c_str(), 1);
		}
		else
		{
			auto hand = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Arm09Sub_R");
			if (!punch)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), punch, &hand, glitterNameRight.c_str(), 1);
		}
	}
}
void KillBerserkEffect()
{
	berserk[0].reset();
	berserk[1].reset();
	berserk[2].reset();
}
void CreateBerserkEffect()
{
	KillBerserkEffect();
	auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	auto node0 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Needle1_U_C");
	auto node1 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_R");
	auto node2 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_L");
	if (!berserk[0])
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), berserk[0], &node0, "evil_berserk01", 1);
	if (!berserk[1])
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), berserk[1], &node1, "evil_berserk01", 1);
	if (!berserk[2])
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), berserk[2], &node2, "evil_berserk01", 1);
}
void PlaySoundStaticCueName2(SharedPtrTypeless& soundHandle, Hedgehog::base::CSharedString cueID, CVector a4)
{
	uint32_t* syncObject = *(uint32_t**)0x1E79044;
	if (syncObject)
	{
		FUNCTION_PTR(void*, __thiscall, sub_75FA60, 0x75FB00, void* This, SharedPtrTypeless&, const Hedgehog::base::CSharedString & cueId, const CVector & a4);
		sub_75FA60((void*)syncObject[8], soundHandle, cueID, a4);
	}
}
std::string lastMusicCue;

void ExecuteAttackCommand(std::string attack, int attackIndex, bool starter = false)
{
	lastAttackIndex = attackIndex;
	auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, true);

	auto node = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_R");
	DebugDrawText::log(std::format("Matrix 0 {0}, {1}, {2}", node->GetWorldMatrix().data()[0], node->GetWorldMatrix().data()[1], node->GetWorldMatrix().data()[2]).c_str(), 10);
	if(shockwaveGameObject != nullptr)
		shockwaveGameObject->SendMessage(shockwaveGameObject->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
	shockwaveGameObject = Common::CreatePlayerSupportShockWaveReturnGameObject(playerContext->m_spMatrixNode->m_Transform.m_Position + (playerContext->m_spMatrixNode->m_Transform.m_Rotation * CVector(0, 0, 2)), 2, 2, 100);
	PlayAnim(GetStateNameFromTable(attack));
	/*Common::PlaySoundStatic(sound, attacks.at(attackIndex).cueIDs[comboIndex]);*/
	lastAttackName = attack;

	auto resourcelist = XMLParser::animationTable.at(attackIndex).ResourceInfos.Resources;
	int resourceIndex = 0;
	int effectIndex = 0;
	for (size_t i = 0; i < resourcelist.size(); i++)
	{
		if (resourcelist[i].Type == CSB)
		{
			resourceIndex = i;
			break;
		}
	}
	for (size_t i = 0; i < resourcelist.size(); i++)
	{
		if (resourcelist[i].Type == Effect)
		{
			effectIndex = i;
			break;
		}
	}
	playerContext->m_Velocity = CVector(0, 0, 0);
	playerContext->m_pPlayer->m_StateMachine.ChangeState<CStateAttackAction_byList>();
	AddImpulse(((playerContext->m_spMatrixNode->m_Transform.m_Rotation * Eigen::Vector3f::UnitZ()) * ((GetMotionFromName(attack).MotionMoveSpeedRatio) * 10)) * 100, true);
	sound.reset();
	lastMusicCue = resourcelist[resourceIndex].Params.Cue;
	Common::PlaySoundStaticCueName(sound, Hedgehog::base::CSharedString(resourcelist[resourceIndex].Params.Cue.c_str()));

	SpawnParticleOnHand(GetMotionFromName(attack).Effect.LEffect_Name1, GetMotionFromName(attack).Effect.REffect_Name1, resourcelist[effectIndex].Params.Cue);
	timerAttack = 0;
}

void SearchThenExecute(std::string input, bool starter, Sonic::EKeyState state)
{
	if (starter)
	{
		for (size_t i = 0; i < XMLParser::starterAttacks.size(); i++)
		{
			if (XMLParser::starterAttacks[i].ActionName == input)
			{
				if (XMLParser::starterAttacks[i].MotionName.empty())
					return;
				comboAttackIndex = i;
				timerCombo = 0;
				playingAttack = true;
				attackCache = XMLParser::starterAttacks[i];
				ExecuteAttackCommand(XMLParser::starterAttacks[i].MotionName, comboAttackIndex, true);
				break;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < XMLParser::attacks.size(); i++)
		{
			if (input == XMLParser::attacks[i].ActionName)
			{
				std::string container;
				if (state == eKeyState_X)
					container = XMLParser::attacks[i].KEY__XDown;
				if (state == eKeyState_Y)
					container = XMLParser::attacks[i].KEY__YDown;
				if (state == eKeyState_A)
					container = XMLParser::attacks[i].KEY__ADown;

				if (container.empty())
					continue;
				ExecuteAttackCommand(container, comboAttackIndex, false);
				for (size_t i = 0; i < XMLParser::attacks.size(); i++)
				{
					if (lastAttackName == container)
					{
						attackCache = XMLParser::attacks[i];
						break;
					}
				}

				comboAttackIndex = i;
				timerCombo = 0;
				playingAttack = true;
				break;
			}
		}

		if (!playingAttack)
		{
			timerAttack = timerAttackMax;
			timerCombo = timerComboMax;
			currentButtonChain.clear();
		}
	}
}
float SetPlayerVelocity()
{
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	switch (currentState)
	{
	case WerehogState::Dash:
	{
		return 20;
	}
	case WerehogState::Normal:
	{
		return 10;
	}
	case WerehogState::Guard:
	{
		return 2;
	}
	}
}
void Particle_Checker()
{
	if (timerAttack > timerAttackMax)
		DespawnParticlesHand();
}
void CheckForThinPlatform()
{
	Eigen::Vector4f outPos;

	const uint32_t result = *(uint32_t*)((uint32_t) * (void**)((uint32_t)*PLAYER_CONTEXT + 0x110) + 172);
	float* position = (float*)(*(uint32_t*)(result + 16) + 112);
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	Eigen::Vector4f const rayLeftStart(position[0] - 1, position[1], position[2], 1.0f);
	Eigen::Vector4f const rayLeftEnd(position[0] - 1, position[1], position[2] - 5, 1.0f);
	Eigen::Vector4f outNormal;
	if (Common::fRaycast(rayLeftStart, rayLeftStart, outPos, outNormal, *(uint32_t*)0x1E0AFB4))
	{
		DebugDrawText::log("Left Hit!", 0);
	}
}
HOOK(char, __stdcall, SonicStateGrounded, 0xDFF660, int* a1, bool a2)
{
	canJump = true;
	jumpcount = 0;
	isGrounded = true;
	return originalSonicStateGrounded(a1, a2);
}

HOOK(void, __fastcall, CPlayerAddCallback, 0xE799F0, Sonic::Player::CPlayer* This, void* Edx,
	const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder, Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	originalCPlayerAddCallback(This, Edx, worldHolder, pGameDocument, spDatabase);

	if (BlueBlurCommon::IsClassic())
	{
		const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		playerContext->m_ChaosEnergy = 0;
		lifeWerehog = 20.0f;
		canJump = true;
		isGrounded = true;
		playingAttack = false;
		playerContext->ChangeState("StartCrouching");
	}

}
HOOK(int, __fastcall, HomingBegin, 0x01232040, CQuaternion* This)
{
	if (BlueBlurCommon::IsClassic())
	{
		if (CONTEXT->m_HomingAttackTargetActorID)
		{
			return originalHomingBegin(This);
		}
		else

			return 0;
	}
	return originalHomingBegin(This);
}
HOOK(void, __fastcall, CClassicSonicProcMsgDamage, 0xDEA340, Sonic::Player::CSonic* This, void* _, hh::fnd::Message& in_rMsg)
{
	if (timerDamage >= timerDamageMax)
	{
		lifeWerehog -= 1;
		if (lifeWerehog > 0)
		{
			timerDamage = 0;
			PlayAnim("Evilsonic_damageMB");
			const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

		}
		else
		{
			CONTEXT->m_RingCount = 0;
			originalCClassicSonicProcMsgDamage(This, _, in_rMsg);
		}

	}
}

//HOOK(char, __fastcall, XButtonInput, 0x00DFDF20, DWORD* This)
//{
//	if (BlueBlurCommon::IsClassic())
//	{
//		if (playingAttack)
//		{
//			const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
//			if (!attackCache.IsGravity)
//				playerContext->m_Velocity = CVector(0, 0, 0);
//		}
//		return 0;
//
//	}
//		return originalXButtonInput(This);
//}
HOOK(char, __fastcall, XButtonHoming_ChangeToHomingAttack, 0x00DFFE30, CSonicContext* This, void* Edx, int a2)
{
	if (BlueBlurCommon::IsClassic())
		return 0;
	else
		return originalXButtonHoming_ChangeToHomingAttack(This, Edx, a2);
}
DWORD* GetServiceGameplay(Hedgehog::Base::TSynchronizedPtr<Sonic::CApplicationDocument> doc)
{
	uint32_t func = 0x0040EBD0;
	DWORD* result;
	__asm
	{
		mov     edi, doc
		add     edi, 34h
		call func
		mov     result, eax
	};
};


HOOK(void, __fastcall, Jump_PlayAnimation, 0x01235250, int This)
{
	originalJump_PlayAnimation(This);
	if (BlueBlurCommon::IsClassic())
	{
		if (jumpcount == 0)
		{
			if (currentState == WerehogState::Dash)
			{
				PlayAnim("Evilsonic_dash_jumpS");
			}
			else
				PlayAnim("JumpEvil1");

			Common::PlaySoundStatic(sound, 42);
		}
		else
		{
			if (currentState == WerehogState::Dash)
				return;
			if (canJump)
			{
				AddJumpThrust(Sonic::Player::CPlayerSpeedContext::GetInstance(), true);
				PlayAnim("JumpEvil2");
			}
		}
		jumpcount++;
	}
}
HOOK(char, __fastcall, JumpStart, 0x01114CB0, int* This)
{
	if (playingAttack == true && BlueBlurCommon::IsClassic())
		return 0;
	auto retu = originalJumpStart(This);
	return  retu;

}
HOOK(char, __fastcall, HomingStart, 0x00DC50D0, CSonicContext* This, void* Edx, int a2)
{
	return 0;
}

HOOK(void, __fastcall, ProcMsgRestartStage, 0xE76810, Sonic::Player::CPlayer* This, void* Edx, hh::fnd::Message& message)
{
	originalProcMsgRestartStage(This, Edx, message);
	lifeWerehog = 5.0f;
}
//00BDE360
//void __thiscall Sonic::CEnemyBase::AddCallback(Sonic::CEnemyBase *this, int a2)
//char __thiscall sub_BE0790(char *this, , int a3)

HOOK(void, __fastcall, CSonicSetMaxSpeedBasis, 0xDFBCA0, int* This)
{
	originalCSonicSetMaxSpeedBasis(This);

	if (*pClassicSonicContext && GetServiceGameplay(Sonic::CApplicationDocument::GetInstance())[1] == 1)
	{
		float* maxSpeed = Common::GetPlayerMaxSpeed();
		*maxSpeed = max(*maxSpeed, SetPlayerVelocity());
	}
}

HOOK(double, __fastcall, GetClassicMaxVelocity, 0x00DC1F20, CSonicContext* This, void* Edx, int a2)
{
	return CONTEXT->m_MaxVelocity;
}
HOOK(void, __fastcall, SetClassicMaxVelocity, 0x00DC2020, CSonicContext* This)
{
	if (*pClassicSonicContext && GetServiceGameplay(Sonic::CApplicationDocument::GetInstance())[1] == 1)
	{
		float* maxSpeed = Common::GetPlayerMaxSpeed();
		*maxSpeed = max(*maxSpeed, SetPlayerVelocity());
	}
}
//_DWORD *__thiscall Sonic::Player::CSonicStateWalk::Begin(int this)
HOOK(void, __fastcall, StandCalc, 0x00DED4E0, int* This)
{
	if (BlueBlurCommon::IsClassic() && timerAttack < timerAttackMax && timerCombo < timerComboMax)
	{
		auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		if (playingAttack)
			PlayAnim(GetStateNameFromTable(lastAttackName));
	}
	else
		originalStandCalc(This);
}
std::string getEVSId()
{
	uint32_t* appdocMember = (uint32_t*)Sonic::CApplicationDocument::GetInstance()->m_pMember;
	auto gameParameters = *((DWORD*)appdocMember + 109);
	Hedgehog::Base::CSharedString* evsIDLoc = (Hedgehog::Base::CSharedString*)(*((DWORD*)gameParameters + 32) + 44);
	Hedgehog::Base::CSharedString* stageIDLoc = (Hedgehog::Base::CSharedString*)(*((DWORD*)gameParameters + 32) + 48);
 	return evsIDLoc->c_str();

}
//void __thiscall sub_DB9F90(CTempState *this)
HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	originalCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
	deltaTime = in_rUpdateInfo.DeltaTime;
	if (BlueBlurCommon::IsClassic() && getEVSId() == "")
	{
		const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		if (abs(playerContext->m_Velocity.x() + playerContext->m_Velocity.z()) > 0)
			tempTimerWalk += in_rUpdateInfo.DeltaTime;
		else
			tempTimerWalk = 0;
		const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
		playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
		Hedgehog::Base::CSharedString stateCheck = playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
		std::string stateCheckS(stateCheck.c_str());
		//DebugDrawText::log((std::string("Current Player Anim: ") + std::string(spAnimInfo->m_Name.c_str())).c_str(), 0);
		//DebugDrawText::log((std::string("tempTimerWalk: ") + std::to_string(tempTimerWalk)).c_str(), 0);
		//
		//DebugDrawText::log((std::string("Current Player State: ") + stateCheckS).c_str(), 0);
		//DebugDrawText::log((std::string("Current Player Posture: ") + std::string(playerContext->m_pPlayer->m_PostureStateMachine.GetCurrentState()->m_Name.c_str())).c_str(), 0);
		if (!BlueBlurCommon::IsClassic())
		{
			return;
		}

		if (timerDamage <= timerDamageMax)
			timerDamage += in_rUpdateInfo.DeltaTime;
		// Force disable extended boost.
		*(uint32_t*)((uint32_t)*CONTEXT->ms_pInstance + 0x680) = 1;
		CONTEXT->m_ChaosEnergy = min(CONTEXT->m_ChaosEnergy, 100);


		//playerContext->m_JumpThrust = CVector(playerContext->m_JumpThrust.x(), 1, playerContext->m_JumpThrust.z());
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_BoostEnableChaosEnergy] = 1000.0f;
		float rotForce = 70.0f;
		if (currentState == WerehogState::Dash)
			rotForce = 30.0f;
		else if (currentState == WerehogState::Guard)
			rotForce = 0;
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_RotationForce0] = rotForce;
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_RotationForce1] = rotForce;
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_RotationForce2] = rotForce;
		playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::eStateFlag_EnableAirOnceAction] =false;
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_AirBoostEnableChaosEnergy] = 1000.0f;
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_EnableHurdleJumpMinVelocity] = 1000.0f;

		SONIC_CLASSIC_CONTEXT->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_MaxHorzVelocity] = GetVelocity();



		isGrounded = playerContext->m_Grounded;
		/*sonic->m_spParameter->m_scpNode->m_ValueMap.erase(Sonic::Player::ePlayerSpeedParameter_BoostEnableChaosEnergy);
		sonic->m_spParameter->m_scpNode->m_ValueMap.erase(Sonic::Player::ePlayerSpeedParameter_AirBoostEnableChaosEnergy);*/
		DebugDrawText::log((std::string("Timer Combo:") + std::to_string(timerCombo)).c_str(), 0);
		DebugDrawText::log((std::string("Timer Combo Max:") + std::to_string(timerComboMax)).c_str(), 0);
		DebugDrawText::log((std::string("Timer Attack:") + std::to_string(timerAttack)).c_str(), 0);
		DebugDrawText::log((std::string("Timer Attack Max:") + std::to_string(timerAttackMax)).c_str(), 0);
		DebugDrawText::log((std::string("Combo progress:") + std::to_string(comboProgress)).c_str(), 0);
		DebugDrawText::log((std::string("Boost") + std::to_string(CONTEXT->m_ChaosEnergy)).c_str(), 0);
		DebugDrawText::log((std::string("Tap") + std::to_string(lastTap)).c_str(), 0);
		DebugDrawText::log("\n", 0);
		DebugDrawText::log((std::string("Life") + std::to_string(lifeWerehog)).c_str(), 0);
		DebugDrawText::log((std::string("PlayingAttack") + std::to_string(playingAttack)).c_str(), 0);
		DebugDrawText::log((std::string("AttackAnim: ") + std::string(lastAttackName)).c_str(), 0);
		DebugDrawText::log((std::string("Latest Music Cue: ") + std::string(lastMusicCue)).c_str(), 0);
		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
		if (inputPtr->IsDown(eKeyState_X) || inputPtr->IsDown(eKeyState_Y) || inputPtr->IsDown(eKeyState_A))
		{
			timerCombo = 0;
		}
		RegisterInputs();
		Particle_Checker();
		if (isGrounded && jumpcount > 0)
		{
			canJump = true;
			jumpcount = 0;
		}
		if (unleashMode && CONTEXT->m_ChaosEnergy > 0)
		{
			playerContext->m_ChaosEnergy -= in_rUpdateInfo.DeltaTime * 5;
		}
		else
		{
			unleashMode = false;
			KillBerserkEffect();
			soundUnleash.reset();
		}
		if (inputPtr->IsDown(eKeyState_DpadDown))
		{
			auto node = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_R");

		}
		if ((inputPtr->IsDown(eKeyState_RightBumper) && CONTEXT->m_ChaosEnergy == 100.0f) && !unleashMode && isGrounded)
		{
			playerContext->m_Velocity = CVector(0, 0, 0);
			PlayAnim("Evilsonic_BerserkerS");
			CreateBerserkEffect();
			Common::PlaySoundStatic(soundUnleashStart, 126);
			Common::PlaySoundStatic(soundUnleash, 127);
			unleashMode = true;
		}
		if (inputPtr->IsDown(eKeyState_LeftBumper) && currentState != WerehogState::Guard)
		{
			currentState = WerehogState::Guard;
			//ef_ch_sng_yh1_boost2
			auto node0 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hips");
			if (!shield)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), shield, &node0, "evil_guard_sphere01", 1);

			PlayAnim("Evilsonic_guard_idle");
		}
		else
		{
			currentState = WerehogState::Normal;
			shield.reset();
		}
		CheckForThinPlatform();

		if (inputPtr->IsDown(Sonic::eKeyState_RightTrigger))
		{
			currentState = WerehogState::Dash;
		}
		else
		{
			currentState = WerehogState::Normal;
		}
		if (!playingAttack && isGrounded)
		{

			DebugDrawText::log(std::format("SPEED_MAGNITUDE: {0}", abs(playerContext->m_Velocity.norm())).c_str(), 0);
			const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
			playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
			if (currentState == WerehogState::Dash)
			{
				if (abs(playerContext->m_Velocity.norm() > 12))
				{
					if ((!IsCurrentAnimationName("Evilsonic_dash"))
						|| (IsCurrentAnimationName("Evilsonic_dash") && spAnimInfo->m_Frame >= 10))
					{
						PlayAnim("Evilsonic_dash");
					}
				}
			}
			if (currentState == WerehogState::Normal)
			{
				/*	if(std::strstr(spAnimInfo->m_Name.c_str(), "Evilsonic_dash") != nullptr)
						PlayAnim("Evilsonic_dashE");*/
				if (abs(playerContext->m_Velocity.norm() > 5))
				{
					if ((!IsCurrentAnimationName("Evilsonic_run"))
						|| (IsCurrentAnimationName("Evilsonic_run") && spAnimInfo->m_Frame >= 51))
						PlayAnim("Evilsonic_run");
				}

			}
		}

		if (timerCombo > timerComboMax)
		{
			comboProgress = 0;
			Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, false);
		}
		else
		{
			if (timerAttack > timerAttackMax && playingAttack)
			{
				playingAttack = false;
				comboProgress++;
			}

			if ((timerCombo > 0.1f && comboProgress > 0 || comboProgress == 0) && (timerAttack > timerAttackMax) && currentButtonChain.size() > comboProgress)
			{
				if (currentButtonChain[currentButtonChain.size() - 1] == eKeyState_X || currentButtonChain[currentButtonChain.size() - 1] == eKeyState_Y || currentButtonChain[currentButtonChain.size() - 1] == eKeyState_A)
				{

					if (comboProgress == 0)
					{
						std::string attackName = "Start_";
						if (currentState == WerehogState::Dash)
						{
							if (isGrounded)
								attackName += "Dash_Run_";
							else
								attackName += "Dash_";

						}
						if (currentState == WerehogState::Guard)
							attackName += "Guard_";
						if (!isGrounded)
							attackName += "Air_";
						if (!isGrounded || currentState == WerehogState::Dash)
						{
							switch (currentButtonChain[comboProgress])
							{
							case eKeyState_X:
							{
								attackName += "XButtonDown";
								break;
							}
							case eKeyState_Y:
							{
								attackName += "YButtonDown";
								break;
							}
							}
						}
						else
						{
							switch (currentButtonChain[comboProgress])
							{
							case eKeyState_X:
							{
								attackName += "XButtonUP";
								break;
							}
							case eKeyState_Y:
							{
								attackName += "YButtonUP";
								break;
							}
							}
						}
						SearchThenExecute(attackName, true, eKeyState_None);
					}

					if (comboProgress >= 1)
					{
						SearchThenExecute(lastAttackName, false, currentButtonChain[comboProgress]);
					}

				}
			}
		}
		DebugDrawText::log((std::string("Jump count: ") + std::to_string(jumpcount)).c_str(), 0);
		if (inputPtr->IsTapped(Sonic::eKeyState_A) && !playingAttack)
		{
			isGrounded = false;
			if (jumpcount == 1 && canJump)
			{
				canJump = false;
				Sonic::Player::CPlayerSpeedContext::GetInstance()->m_Velocity.y() = 0;
				AddJumpThrust(Sonic::Player::CPlayerSpeedContext::GetInstance(), true);
				Common::PlaySoundStatic(sound, 42);

				PlayAnim("JumpEvil2");
				jumpcount++;
			}

		}

		timerCombo += in_rUpdateInfo.DeltaTime;
		timerAttack += in_rUpdateInfo.DeltaTime;
	}
}

// Call the function when we initialize everything.
HOOK(void*, __fastcall, _InitializePlayer, 0x00D96110, void* This)
{
	void* result = original_InitializePlayer(This);
	auto context = Sonic::Player::CPlayerSpeedContext::GetInstance();    // Hack: there's a better way to do this but whatever. This writes to the singleton anyway.
	if (BlueBlurCommon::IsClassic())
	{
		AddTestState(context);
	}
	return result;
}
extern "C" __declspec(dllexport) float API_GetLife()
{
	return lifeWerehog;
}
extern "C" __declspec(dllexport) bool API_IsWerehogActive()
{
	return BlueBlurCommon::IsClassic();
}
HOOK(void, __cdecl, InitializeApplicationVFXParams, 0x00D65180, Sonic::CParameterFile* This)
{
	boost::shared_ptr<Sonic::CParameterGroup> parameterGroup;
	This->CreateParameterGroup(parameterGroup, "EvilSonic", "Parameters for the Werehog Mod from NextinHKRY");
	Sonic::CEditParam* cat_Physics = parameterGroup->CreateParameterCategory("EvilSonic", "Werehog's Parameters");

	cat_Physics->CreateParamFloat(&timerComboMax, "Timer Combo Maximum");
	cat_Physics->CreateParamFloat(&timerAttackMax, "Timer Attack Maximum");
	cat_Physics->CreateParamFloat(&timerDamageMax, "Timer Damage Maximum");
	cat_Physics->CreateParamFloat(&lifeWerehog, "Life Amount");
	cat_Physics->CreateParamFloat(&attackVelocityDivider, "Atttack Velocity Divider");

	parameterGroup->Flush();

	originalInitializeApplicationVFXParams(This);
}
//00DED4E0
//Hedgehog::Base::CSharedString* __thiscall Sonic::Player::CSonicStateWalk::Update(int* this)
HOOK(Hedgehog::Base::CSharedString*, __fastcall, WalkUpdate, 0x00DED4E0, int* This)
{

	if (BlueBlurCommon::IsClassic())
		return 0;
	else
		return originalWalkUpdate(This);
}
void __declspec(naked) TestJump()
{
	static uint32_t RedRingCollectedCheckReturnAddress = 0x00DC6719;
	static uint32_t sub_E71A50 = 0xE71A50;
	static uint32_t sub_DFCE30 = 0xDFCE30;
	static uint32_t loc_E01F37 = 0xE01F37;
	static uint32_t loc_E01F68 = 0xE01F68;
	__asm
	{
		mov     al, [edi + 440h]
		mov     esi, edi
		call[sub_E71A50]
		push    edi
		call[sub_DFCE30]
		jmp[RedRingCollectedCheckReturnAddress]
	}
}

void __declspec(naked) TestJump1()
{
	static uint32_t called = 0x00DFC470;
	static uint32_t RedRingCollectedCheckReturnAddress = 0x00DEF428;
	__asm
	{
		mov ebx, Sonic::Player::CSonicClassicContext::GetInstance
		jmp[RedRingCollectedCheckReturnAddress]
	}
}


void sub_E68360(CSonicContext* a1)
{
	uint32_t func = 0xE68360;
	__asm
	{
		mov edi, a1
		push edi
		call    func
	};
};

//void __thiscall Sonic::Player::CSonicClassicStateNormalDamageDead::Calculate(int a1)
HOOK(void, __fastcall, NormalDamageDeadUpdate, 0x012523C0, int* a1, void* Edx)
{

}
struct MsgRestartStage : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01681FA0);
};
HOOK(void, __fastcall, NormalDamageDeadBegin, 0x01252170, int* a1, void* Edx)
{

	Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_ActorID, boost::make_shared<Sonic::Message::MsgDead>(true));
	Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_ActorID, boost::make_shared<MsgResetCamera>());
}
HOOK(void, __fastcall, NormalDamageDeadAfter, 0x12520D0, int* a1, void* Edx)
{
}
//void __thiscall sub_11146C0(void *this)
HOOK(void, __fastcall, ClassicMoveStopUpdate, 0x11146c0, void* This, void* Edx)
{
	Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_StateMachine.ChangeState("Walk");
}
void EvilSonic::Install()
{
	//Register some of the basic non-attack anims
	CustomAnimationManager::RegisterAnimation("Evilsonic_damageMB", "evilsonic_damageMB");
	CustomAnimationManager::RegisterAnimation("Evilsonic_guard_idle", "evilsonic_guard_idle");
	CustomAnimationManager::RegisterAnimation("Evilsonic_BerserkerS", "evilsonic_BerserkerS");
	CustomAnimationManager::RegisterAnimation("JumpEvil1", "evilsonic_jumpVS");
	CustomAnimationManager::RegisterAnimation("JumpEvil2", "evilsonic_jumpVS2");
	CustomAnimationManager::RegisterAnimation("Evilsonic_dashS", "evilsonic_dashS");
	CustomAnimationManager::RegisterAnimation("Evilsonic_dash", "evilsonic_dash");
	CustomAnimationManager::RegisterAnimation("Evilsonic_start", "evilsonic_start");

	CustomAnimationManager::RegisterAnimation("Evilsonic_runS", "evilsonic_runS");
	CustomAnimationManager::RegisterAnimation("Evilsonic_pillar_idle", "evilsonic_pillar_idle");
	CustomAnimationManager::RegisterAnimation("Evilsonic_run", "evilsonic_run", 3);
	CustomAnimationManager::RegisterAnimation("Evilsonic_runE", "evilsonic_runE");
	CustomAnimationManager::RegisterAnimation("Evilsonic_dash_jumpS", "evilsonic_dash_jumpS");
	//CustomAnimationManager::RegisterAnimation("StartEventDash", "evilsonic_runE");


	//WRITE_MEMORY(0xD00E6F, uint8_t, 0xEB);
	//INSTALL_HOOK(XButtonInput);
	//INSTALL_HOOK(CSonicSetMaxSpeedBasis);
	//INSTALL_HOOK(sub_BE0790);

	//runE = walk stop
	//runS = walk start
	//run = walk
	//dashS = run start
	//dash run
	//dashE = run stop

	//INSTALL_HOOK(NormalDamageDeadAfter);
	INSTALL_HOOK(NormalDamageDeadBegin);
	//INSTALL_HOOK(NormalDamageDeadUpdate);
	INSTALL_HOOK(InitializeApplicationVFXParams);
	INSTALL_HOOK(XButtonHoming_ChangeToHomingAttack);
	INSTALL_HOOK(SetClassicMaxVelocity);
	INSTALL_HOOK(GetClassicMaxVelocity);
	INSTALL_HOOK(GetClassicMaxVelocity);
	INSTALL_HOOK(JumpStart);
	INSTALL_HOOK(Jump_PlayAnimation);
	INSTALL_HOOK(CPlayerAddCallback);
	INSTALL_HOOK(ProcMsgRestartStage);
	INSTALL_HOOK(CClassicSonicProcMsgDamage);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	INSTALL_HOOK(ClassicMoveStopUpdate);

	INSTALL_HOOK(WalkUpdate);
	INSTALL_HOOK(HomingBegin);
	INSTALL_HOOK(StandCalc);
	INSTALL_HOOK(_InitializePlayer);
	//WRITE_JUMP(0x00DC6713, TestJump);
	//WRITE_JUMP(0x00DEF41F, TestJump1);
	
	//Jump over check for start anims
	WRITE_JUMP(0X00D900C7, 0X00D9019A);
	//Unmap stomp/spin for classic
	WRITE_JUMP(0X00DC5F7E, 0X00DC6054);
	WRITE_JUMP(0X012523C0, 0x012530E0);

	//Disable MoveStop
	//WRITE_JUMP(0x0111C020, 0x0111C066);

	//Force Jump stuff
	WRITE_JUMP(0x01114A14, 0x01114A39);
	WRITE_JUMP(0x01114D95, 0x01114DDA);

	//Force the jump sound to be silent - disable if we redo classic
	//WRITE_MEMORY(0x00E57E4E, uint32_t, -1);

	//Disable Spindash
	WRITE_JUMP(0x00DC28D9, 0x00DC2946);
}