
#include "EvilSonic.h"

#include "EvilStateMachine/State/EvilStateRunningJump.h"
#include "EvilStateMachine/State/EvilStateArmSwing.h"
#include "EvilStateMachine/State/EvilStateAttackAction_byList.h"
#include "EvilStateMachine/State/EvilStateDamageNormal.h"


using namespace hh::math;
using namespace Sonic;

enum WerehogState
{
	Normal,
	Dash,
	Guard
};

struct MsgRestartStage : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01681FA0);
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
float timerAttack;
float timerDamage = 0.3f;
int comboProgress = 0;
bool unleashMode;
bool playingAttack;
bool isGrounded;
Sonic::EKeyState lastTap;
bool cameraAnimTempExecuted = false;

//find a better way please
bool init = false;
boost::shared_ptr<Sonic::CGameObject3D> collision1;
Hedgehog::math::CVector Evil::CStateArmSwing::target;
WerehogState currentState;


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

bool IsCurrentAnimationName(std::string in)
{
	auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
	playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
	return std::strstr(spAnimInfo->m_Name.c_str(), in.c_str()) != nullptr;
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
	if (currentState == WerehogState::Guard)
		return 1;
	if (currentState == WerehogState::Normal && tempTimerWalk < 0.5f)
		return 2;
	if (currentState == WerehogState::Normal)
		return 5.5f;
	if (currentState == WerehogState::Dash)
		return 12;
	return 1;
}
void RegisterInputs()
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	auto state = inputPtr->TappedState;
	DebugDrawText::log(std::to_string(state).c_str(), 0);
	lastTap = inputPtr->DownState;
	DebugDrawText::log(std::to_string(lastTap).c_str(), 0);
	if (timerCombo < EvilGlobal::parameters->timerComboMax)
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
	if(EvilGlobal::shockwaveGameObject != nullptr)
		EvilGlobal::shockwaveGameObject->SendMessage(EvilGlobal::shockwaveGameObject->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
	EvilGlobal::shockwaveGameObject = Common::CreatePlayerSupportShockWaveReturnGameObject(playerContext->m_spMatrixNode->m_Transform.m_Position + (playerContext->m_spMatrixNode->m_Transform.m_Rotation * CVector(0, 0, 2)), 2, 2, 100);
	PlayAnim(EvilGlobal::GetStateNameFromTable(attack));
	/*Common::PlaySoundStatic(sound, attacks.at(attackIndex).cueIDs[comboIndex]);*/
	EvilGlobal::lastAttackName = attack;

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
	playerContext->m_pPlayer->m_StateMachine.ChangeState<Evil::CStateAttackAction_byList>();
	AddImpulse(((playerContext->m_spMatrixNode->m_Transform.m_Rotation * Eigen::Vector3f::UnitZ()) * ((EvilGlobal::GetMotionFromName(attack).MotionMoveSpeedRatio) * 10)) * 100, true);
	sound.reset();
	lastMusicCue = resourcelist[resourceIndex].Params.Cue;
	Common::PlaySoundStaticCueName(sound, Hedgehog::base::CSharedString(resourcelist[resourceIndex].Params.Cue.c_str()));

	SpawnParticleOnHand(EvilGlobal::GetMotionFromName(attack).Effect.LEffect_Name1, EvilGlobal::GetMotionFromName(attack).Effect.REffect_Name1, resourcelist[effectIndex].Params.Cue);
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
				for (size_t x = 0; x < XMLParser::attacks.size(); x++)
				{
					///FIX
					if (EvilGlobal::lastAttackName == container)
					{
						attackCache = XMLParser::attacks[x];
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
			timerAttack = EvilGlobal::parameters->timerAttackMax;
			timerCombo = EvilGlobal::parameters->timerComboMax;
			currentButtonChain.clear();
		}
	}
}
float SetPlayerVelocity()
{
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
	return 10;
}
void Particle_Checker()
{
	if (timerAttack > EvilGlobal::parameters->timerAttackMax)
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

HOOK(void, __fastcall, CPlayerSpeedContext_AddCallback, 0xE799F0, Sonic::Player::CPlayer* This, void* Edx,
	const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder, Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	originalCPlayerSpeedContext_AddCallback(This, Edx, worldHolder, pGameDocument, spDatabase);

	if (BlueBlurCommon::IsClassic())
	{
		const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		playerContext->m_ChaosEnergy = 0;
		canJump = true;
		isGrounded = true;
		playingAttack = false;
		playerContext->ChangeState("StartCrouching");
	}

}
HOOK(int, __fastcall, CSonicStateHomingAttack_Begin, 0x01232040, CQuaternion* This)
{
	if (BlueBlurCommon::IsClassic())
	{
		/*if (CONTEXT->m_HomingAttackTargetActorID)
		{
			return originalCSonicStateHomingAttack_Begin(This);
		}
		else*/
			return 0;
	}
	return originalCSonicStateHomingAttack_Begin(This);
}

HOOK(void, __fastcall, CSonicClassicMsgDamageProcessor, 0xDEA340, Sonic::Player::CSonic* This, void* _, Sonic::Message::MsgDamage& in_rMsg)
{
	if (timerDamage >= EvilGlobal::parameters->timerDamageMax)
	{
		EvilGlobal::parameters->lifeCurrentAmount -= 1;
		if (EvilGlobal::parameters->lifeCurrentAmount > 0)
		{
			timerDamage = 0;
			if(in_rMsg.m_DamageType == 1)
			{
				Sonic::Player::CPlayerSpeedContext::GetInstance()->ChangeState<Evil::CStateDamageNormal>();
			}
			else
			{
				//Sonic::Player::CPlayerSpeedContext::GetInstance()->ChangeState<Evil::CStateDamageFly>();
			}
		}
		else
		{
			//Sonic::Player::CPlayerSpeedContext::GetInstance()->ChangeState<Evil::CStateDamageDead>();
			CONTEXT->m_RingCount = 0;
			originalCSonicClassicMsgDamageProcessor(This, _, in_rMsg);
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
HOOK(char, __fastcall, CPlayerSpeedContext_ChangeToHomingAttackX, 0x00DFFE30, CSonicContext* This, void* Edx, int a2)
{
	if (BlueBlurCommon::IsClassic())
		return 0;
	else
		return originalCPlayerSpeedContext_ChangeToHomingAttackX(This, Edx, a2);
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


HOOK(void, __fastcall, CPlayerSpeedContext_CStateJumpBall_Start, 0x01235250, int This)
{
	originalCPlayerSpeedContext_CStateJumpBall_Start(This);
	if (BlueBlurCommon::IsClassic())
	{
		if (jumpcount == 0)
		{
			if (currentState == WerehogState::Dash)
			{
				PlayAnim("Evilsonic_dash_jumpS");
				Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_StateMachine.ChangeState<Evil::CStateRunningJump>();
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
HOOK(char, __fastcall, CSonicClassicStateJumpShort_StartJump, 0x01114CB0, int* This)
{
	if (playingAttack == true && BlueBlurCommon::IsClassic())
		return 0;
	auto retu = originalCSonicClassicStateJumpShort_StartJump(This);
	return  retu;

}
HOOK(char, __fastcall, HomingStart, 0x00DC50D0, CSonicContext* This, void* Edx, int a2)
{
	return 0;
}

HOOK(void, __fastcall, MsgRestartStageProcessor, 0xE76810, Sonic::Player::CPlayer* This, void* Edx, hh::fnd::Message& message)
{
	originalMsgRestartStageProcessor(This, Edx, message);
	EvilGlobal::parameters->lifeCurrentAmount = EvilGlobal::parameters->lifeMaxAmount;
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

HOOK(double, __fastcall, CSonicClassic_GetMaximumVelocity, 0x00DC1F20, CSonicContext* This, void* Edx, int a2)
{
	return CONTEXT->m_MaxVelocity;
}
HOOK(void, __fastcall, CSonicClassic_SetMaximumVelocity, 0x00DC2020, CSonicContext* This)
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
	if (BlueBlurCommon::IsClassic() && timerAttack < EvilGlobal::parameters->timerAttackMax && timerCombo < EvilGlobal::parameters->timerComboMax)
	{
		if (playingAttack)
			PlayAnim(EvilGlobal::GetStateNameFromTable(EvilGlobal::lastAttackName));
	}
	else
		originalStandCalc(This);
}
std::string getEVSId()
{
	uint32_t* appdocMember = (uint32_t*)Sonic::CApplicationDocument::GetInstance()->m_pMember;
	auto gameParameters = *((DWORD*)appdocMember + 109);
	Hedgehog::Base::CSharedString* evsIDLoc = (Hedgehog::Base::CSharedString*)(*((DWORD*)gameParameters + 32) + 44);
 	return evsIDLoc->c_str();

}
//void __thiscall sub_DB9F90(CTempState *this)
HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0xDDABA0, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
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

		if (timerDamage <= EvilGlobal::parameters->timerDamageMax)
			timerDamage += in_rUpdateInfo.DeltaTime;
		// Force disable extended boost.
		*(uint32_t*)((uint32_t)*CONTEXT->ms_pInstance + 0x680) = 1;
		CONTEXT->m_ChaosEnergy = min(CONTEXT->m_ChaosEnergy, 100);

		//Set rotational parameters every frame since they sometimes reset, maybe find a better way to do this
		//playerContext->m_JumpThrust = CVector(playerContext->m_JumpThrust.x(), 1, playerContext->m_JumpThrust.z());
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_BoostEnableChaosEnergy] = 1000.0f;
		float rotForce = 70.0f;
		if (currentState == WerehogState::Dash)
		{
			rotForce = 30.0f;
			playerContext->m_WorldInput /= 10;
			
		}
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

		//hideous
		DebugDrawText::log("### WEREHOG INFO ### ", 0, 0, DebugDrawText::Color(1, 0, 1, 1));
		DebugDrawText::log(std::format("Timer Combo: {0}", timerCombo).c_str(), 0,1);
		DebugDrawText::log(std::format("Timer Combo Max: {0}", EvilGlobal::parameters->timerComboMax).c_str(), 0,2);
		DebugDrawText::log(std::format("Timer Attack: {0}", timerAttack).c_str(), 0,3);
		DebugDrawText::log(std::format("Timer Attack Max: {0}", EvilGlobal::parameters->timerAttackMax).c_str(), 0,4);
		DebugDrawText::log(std::format("Combo progress: {0}", comboProgress).c_str(), 0,5);
		DebugDrawText::log(std::format("Boost amount: {0}", CONTEXT->m_ChaosEnergy).c_str(), 0,6);
		DebugDrawText::log(std::format("HP: {0}", EvilGlobal::parameters->lifeCurrentAmount).c_str(), 0,7);
		DebugDrawText::log(std::format("Attack Anim: {0}", EvilGlobal::lastAttackName).c_str(), 0,8);
		DebugDrawText::log(std::format("Timer Combo: {0}", timerCombo).c_str(), 0,9);
		DebugDrawText::log(std::format("Posture: {0}", CONTEXT->m_pPlayer->m_PostureStateMachine.GetCurrentState()->m_Name.c_str()).c_str(), 0,10);
		DebugDrawText::log("-----------------------------------------------", 0, 11, DebugDrawText::Color(1, 0, 1, 1));
		

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

		if (timerCombo > EvilGlobal::parameters->timerComboMax)
		{
			comboProgress = 0;
			Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, false);
		}
		else
		{
			//Attack state prepare logic
			if(EvilGlobal::canExecuteAttacks)
			{
				if (timerAttack > EvilGlobal::parameters->timerAttackMax && playingAttack)
				{
					playingAttack = false;
					comboProgress++;
				}

				if ((timerCombo > 0.1f && comboProgress > 0 || comboProgress == 0) && (timerAttack > EvilGlobal::parameters->timerAttackMax) && currentButtonChain.size() > comboProgress)
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
							SearchThenExecute(EvilGlobal::lastAttackName, false, currentButtonChain[comboProgress]);
						}

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


extern "C" __declspec(dllexport) float API_GetLife()
{
	return EvilGlobal::parameters->lifeCurrentAmount;
}
extern "C" __declspec(dllexport) bool API_IsWerehogActive()
{
	return BlueBlurCommon::IsClassic();
}
HOOK(void, __cdecl, Evil_InitializeParametersForEditor, 0x00D65180, Sonic::CParameterFile* This)
{
	boost::shared_ptr<Sonic::CParameterGroup> parameterGroup;
	This->CreateParameterGroup(parameterGroup, "EvilSonic", "Parameters for the Werehog Mod from NextinHKRY");
	Sonic::CEditParam* cat_Physics = parameterGroup->CreateParameterCategory("EvilSonic", "Werehog's Parameters");

	cat_Physics->CreateParamFloat(&EvilGlobal::parameters->timerComboMax, "Timer Combo Maximum");
	cat_Physics->CreateParamFloat(&EvilGlobal::parameters->timerAttackMax, "Timer Attack Maximum");
	cat_Physics->CreateParamFloat(&EvilGlobal::parameters->timerDamageMax, "Timer Damage Maximum");
	cat_Physics->CreateParamFloat(&EvilGlobal::parameters->lifeCurrentAmount, "Life Amount");
	cat_Physics->CreateParamFloat(&EvilGlobal::parameters->lifeMaxAmount, "Life Max Amount");
	cat_Physics->CreateParamFloat(&EvilGlobal::parameters->attackVelocityDivider, "Atttack Velocity Divider");

	parameterGroup->Flush();

	originalEvil_InitializeParametersForEditor(This);
}
//00DED4E0
//Hedgehog::Base::CSharedString* __thiscall Sonic::Player::CSonicStateWalk::Update(int* this)
HOOK(Hedgehog::Base::CSharedString*, __fastcall, CSonicStateWalk_Update, 0x00DED4E0, int* This)
{

	if (BlueBlurCommon::IsClassic())
		return 0;
	else
		return originalCSonicStateWalk_Update(This);
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
HOOK(void, __fastcall, CSonicClassicStateNormalDamageDead_Begin, 0x01252170, int* a1, void* Edx)
{

	Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_ActorID, boost::make_shared<Sonic::Message::MsgDead>(true));
	Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_ActorID, boost::make_shared<MsgResetCamera>());
}
HOOK(void, __fastcall, NormalDamageDeadAfter, 0x12520D0, int* a1, void* Edx)
{
}
//void __thiscall sub_11146C0(void *this)
HOOK(void, __fastcall, CSonicClassicStateMoveStop_Update, 0x11146c0, void* This, void* Edx)
{
	Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_StateMachine.ChangeState("Walk");
}
HOOK(void, __fastcall, _CAnimationPose_SampleAnimation, 0x6CC950, Ceramic::Animation::CAnimationPose* This)
{
	original_CAnimationPose_SampleAnimation(This);

	ProceduralData* procData = ProceduralData::Get(This);
	if (procData->UpdateProcedural == nullptr || procData->m_pObject == nullptr)
		return;

	procData->UpdateProcedural(This, procData->m_pObject);
}
void ProceduralData::SetUpdateFunction(void* functionPointer)
{
	UpdateProcedural = (FPtrUpdateProcedural)functionPointer;
}
ProceduralData* ProceduralData::Get(Ceramic::Animation::CAnimationPose* pose)
{
	return &reinterpret_cast<CAnimationPose_Alternate*>(pose)->m_pMap->procData;
}
int boneIndexTest = 50;
void HeadTurnClassic(Ceramic::Animation::CAnimationPose* pose, Sonic::Player::CPlayerSpeed* player)
{
	using namespace hh::math;

	auto context = player->GetContext();

	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	
	enum class BoneIndex
	{
		Shoulder_L = 60,
		Hand_L = 72,
		Shoulder_R = 102,
	};

	//////////////
	// Code start
	//////////////
	//-----------------------------------------

	// We don't want to do anything if we're not in the right state.
	const auto state = player->m_StateMachine.GetCurrentState().get();
	if (!state)
		return;
	if(EvilGlobal::allowFreemoveArmLeft)
	{
		Ceramic::Animation::hkQsTransform* tBone = pose->m_pAnimData->m_TransformArray.GetIndex(BoneIndex::Shoulder_L);
		tBone->m_Position = EvilGlobal::freemovePositionLeft;
	}
	if (EvilGlobal::allowFreemoveArmRight)
	{
		Ceramic::Animation::hkQsTransform* tBone = pose->m_pAnimData->m_TransformArray.GetIndex(BoneIndex::Shoulder_R);
		tBone->m_Position = EvilGlobal::freemovePositionRight;
	}
	//DebugDrawText::log(std::format("BONE INDEX: {0}", boneIndexTest).c_str(), 0, 100);
	//
	////// RArm
	//Ceramic::Animation::hkQsTransform* tRArm = pose->m_pAnimData->m_TransformArray.GetIndex(boneIndexTest);
	//tRArm->m_Position = CVector(10,10,10);
	//if (inputPtr->IsTapped(Sonic::eKeyState_DpadRight))
	//{
	//	boneIndexTest++;
	//}
}
void CAnimationPoseInit_AddCallbackC(app::Player::CPlayerSpeed* player)
{
	auto test = (Ceramic::Animation::CAnimationPose*)player->m_spAnimationPose.get();
	auto procData = &reinterpret_cast<CAnimationPose_Alternate*>(test)->m_pMap->procData;
	procData->m_pObject = player;
	procData->SetUpdateFunction(HeadTurnClassic);
}
HOOK(void, __fastcall, _SonicInitializeRenderables_Classic, 0x00DE93A0, app::Player::CPlayerSpeed* This, void*, void* a2, int a3)
{
	original_SonicInitializeRenderables_Classic(This, nullptr, a2, a3);
	CAnimationPoseInit_AddCallbackC(This);
}
void registerAnimations()
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
	CustomAnimationManager::RegisterAnimation("Evilsonic_pillar_up", "evilsonic_pillar_up", 1, true);
	CustomAnimationManager::RegisterAnimation("Evilsonic_pillar_upH", "evilsonic_pillar_upH", 1, false);
	CustomAnimationManager::RegisterAnimation("Evilsonic_gate", "evilsonic_gate", 1, false);
	CustomAnimationManager::RegisterAnimation("Evilsonic_pillar_turnL", "evilsonic_pillar_turnL");
	CustomAnimationManager::RegisterAnimation("Evilsonic_pillar_turnR", "evilsonic_pillar_turnR");
	CustomAnimationManager::RegisterAnimation("Evilsonic_pillar_fall", "evilsonic_pillar_fall", 1, true);
	CustomAnimationManager::RegisterAnimation("Evilsonic_run", "evilsonic_run", 3);
	CustomAnimationManager::RegisterAnimation("Evilsonic_runE", "evilsonic_runE");
	CustomAnimationManager::RegisterAnimation("Evilsonic_dash_jumpS", "evilsonic_dash_jumpS");
}
//void __thiscall Sonic::Player::SetBoneMatricies(CPlayer *this, float deltaTime)
HOOK(void, __fastcall, ClassicSonicAnimationUpdate, 0x00E79DA0, void* This, void* Edx, float deltaTime)
{
	if (!EvilGlobal::disableAnimations)
		originalClassicSonicAnimationUpdate(This, Edx, deltaTime);
}
void CAnimationPoseInit_Ctor(Ceramic::Animation::CAnimationPose* pose)
{
	ProceduralData* procData = ProceduralData::Get(pose);
	procData->m_pObject = nullptr;
	procData->UpdateProcedural = nullptr;

}
HOOK(void*, __fastcall, _ConstructCAnimationPose_Common, 0x006CB140, Ceramic::Animation::CAnimationPose* This, void*, void* a2, void* a3)
{
	void* result = original_ConstructCAnimationPose_Common(This, nullptr, a2, a3);
	CAnimationPoseInit_Ctor(This);
	return result;
}
HOOK(void*, __fastcall, _ConstructCAnimationPose_Msn, 0x006CA910, Ceramic::Animation::CAnimationPose* This, void*, void* a2, void* a3, void* a4)
{
	void* result = original_ConstructCAnimationPose_Msn(This, nullptr, a2, a3, a4);
	CAnimationPoseInit_Ctor(This);
	return result;
}
void EvilSonic::registerPatches()
{
	EvilGlobal::canExecuteAttacks = true;
	registerAnimations();
	//CustomAnimationManager::RegisterAnimation("StartEventDash", "evilsonic_runE");
	//WRITE_JUMP(0x00DC6713, TestJump);
	//WRITE_JUMP(0x00DEF41F, TestJump1);
	//WRITE_MEMORY(0xD00E6F, uint8_t, 0xEB);
	//INSTALL_HOOK(XButtonInput);
	//INSTALL_HOOK(CSonicSetMaxSpeedBasis);
	//INSTALL_HOOK(sub_BE0790);/
	//INSTALL_HOOK(NormalDamageDeadAfter);
	//INSTALL_HOOK(NormalDamageDeadUpdate);
	//INSTALL_HOOK(CSonicStateWalk_Update);

	WRITE_MEMORY(0x006CB29D, uint8_t, sizeof(CAnimationPose_Alternate::Map));
	INSTALL_HOOK(_ConstructCAnimationPose_Common);
	INSTALL_HOOK(_ConstructCAnimationPose_Msn);
	INSTALL_HOOK(_SonicInitializeRenderables_Classic);
	INSTALL_HOOK(_CAnimationPose_SampleAnimation);
	//INSTALL_HOOK(ClassicSonicAnimationUpdate);
	INSTALL_HOOK(Evil_InitializeParametersForEditor);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	INSTALL_HOOK(CPlayerSpeedContext_AddCallback);

	/// ------------------------------
	///	Classic patches
	/// ------------------------------
	
	INSTALL_HOOK(CSonicClassic_SetMaximumVelocity);
	INSTALL_HOOK(CSonicClassic_GetMaximumVelocity);
	INSTALL_HOOK(CSonicClassicMsgDamageProcessor);
	INSTALL_HOOK(MsgRestartStageProcessor);	

	/// ------------------------------
	///	Original States patches
	/// ------------------------------

	INSTALL_HOOK(StandCalc);
	INSTALL_HOOK(CSonicStateHomingAttack_Begin);
	INSTALL_HOOK(CSonicClassicStateMoveStop_Update);
	INSTALL_HOOK(CSonicClassicStateJumpShort_StartJump);
	INSTALL_HOOK(CSonicClassicStateNormalDamageDead_Begin);
	INSTALL_HOOK(CPlayerSpeedContext_CStateJumpBall_Start);
	INSTALL_HOOK(CPlayerSpeedContext_ChangeToHomingAttackX);

	//Jump over check for start anims
	WRITE_JUMP(0X00D900C7, 0X00D9019A);

	//Force Jump stuff
	WRITE_JUMP(0x01114A14, 0x01114A39);
	WRITE_JUMP(0x01114D95, 0x01114DDA);

	//Disable Spindash
	WRITE_JUMP(0x00DC28D9, 0x00DC2946);

	//Unmap stomp/spin for classic
	WRITE_JUMP(0X00DC5F7E, 0X00DC6054);
	WRITE_JUMP(0X012523C0, 0x012530E0);

	//Disable MoveStop
	//WRITE_JUMP(0x0111C020, 0x0111C066);

	//Force the jump sound to be silent - disable if we redo classic
	//WRITE_MEMORY(0x00E57E4E, uint32_t, -1);

}