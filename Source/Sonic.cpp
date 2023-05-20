
using namespace hh::math;
using namespace Sonic;

static const int* pColID_BasicTerrain = reinterpret_cast<int*>(0x01E0AFAC);
static const int* pColID_Common = reinterpret_cast<int*>(0x01E0AF30);
static const int* pColID_PlayerEvent = reinterpret_cast<int*>(0x01E0AFD8);
static const int* pColID_Mystery = reinterpret_cast<int*>(0x01E0AF90);
static const int* pColID_Mystery2 = reinterpret_cast<int*>(0x1E0AFBC);
namespace Sonic
{
	class CRigidBody
	{
	public:
		virtual ~CRigidBody() = default;

		Havok::RigidBody* m_pHkpRigidBody;


		virtual void* GetCollider(void* a1) { return a1; }
		virtual void* fn02(void* a1) { return a1; }
		virtual void ApplyPropertyID(int id, uint64_t* weird) {}
		virtual void fn04(int a, int b) {}
		virtual void fn05(int a, int b) {}
		virtual void GetPropertyValue(uint64_t* weird, int hash) {}
		virtual bool VerifyPropertyID(int hash) { return false; }

		void ApplyPropertyID(int id, int value)
		{
			struct
			{
				int value;
				int buffer;
			} data;
			data.value = value;
			ApplyPropertyID(id, (uint64_t*)&data);
		}

		void AddBoolProperty(int hashedName, int id)
		{
			if (!VerifyPropertyID(hashedName))
				return;

			uint64_t data = 0;
			GetPropertyValue(&data, hashedName);

			data = data != 0;
			ApplyPropertyID(id, &data);
		}

		bool GetBoolProperty(int id)
		{
			if (!VerifyPropertyID(id))
				return false;

			uint64_t data = 0;
			GetPropertyValue(&data, id);

			return data != 0;
		}

		Havok::Shape* GetShape() const
		{
			return m_pHkpRigidBody->m_Collideable.m_Shape;
		}
	};
}

//"Index1_R" :
//"Middle1_R"
//"Pinky1_R" :
//	Ring1_R" : "
//	"Thumb1_R" :
SharedPtrTypeless sound, soundUnleash, soundUnleashStart;
SharedPtrTypeless indexParticle_L, indexParticle_R;
SharedPtrTypeless middleParticle_L, middleParticle_R;
SharedPtrTypeless pinkyParticle_L, pinkyParticle_R;
SharedPtrTypeless ringParticle_L, ringParticle_R;
SharedPtrTypeless thumbParticle_L, thumbParticle_R;
SharedPtrTypeless shield;
SharedPtrTypeless berserk[5];
std::vector<Sonic::EKeyState> currentButtonChain;
bool canJump;
int jumpcount;
int comboAttackIndex;
bool isUsingShield;
float timerCombo;
float timerAttack;
float timerComboMax = 0.75f;
float timerAttackMax = 0.55f;
int comboProgress = 0;
float lifeWerehog = 5.0f;
bool unleashMode;
bool playingAttack;
bool isGrounded;
std::string lastAttackName;
Sonic::EKeyState lastTap;

boost::shared_ptr<Sonic::CGameObject3D> collision1;
class CBasicSphere : public Sonic::CGameObject3D
{
public:
	boost::shared_ptr<hh::mr::CSingleElement> m_spRenderable;
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
	boost::shared_ptr<CRigidBody> m_spRigidBody;

	virtual void AddCallback(const hh::base::THolder<Sonic::CWorld>& worldHolder,
		Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<hh::db::CDatabase>& spDatabase) override
	{
		Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
		pGameDocument->AddUpdateUnit("0", this); // Worth looking into which updateuints do what, by the way.

		hh::mr::CMirageDatabaseWrapper wrapper(spDatabase.get());
		// This is a debug asset that has a broken material, so it will be pure red--but that's ok, cuz we can see it.
		const char* assetName = "BasicSphere";
		boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);

		m_spRenderable = boost::make_shared<hh::mr::CSingleElement>(spModelData);
		// Safeguard from crashing (last I checked this works, could crash if it doesn't exist anyway.)
		if (!spModelData)
			return;
		auto node = Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_spCharacterModel->GetNode("Hand_R");

		m_spRenderable->BindMatrixNode(m_spMatrixNodeTransform);
		m_spMatrixNodeTransform->NotifyChanged();
		m_spMatrixNodeTransform->SetParent(node.get());
		m_spMatrixNodeTransform->m_Transform.SetPosition(CVector(0, 0, 0));
		AddRenderable("Object", m_spRenderable, true);

		// Now we set up our havok shape.
		const CVector railHalfExtents = CVector(1.0f, 1.0f, 1.0f);

		Havok::BoxShape* shapeRail = new Havok::BoxShape(railHalfExtents * 0.5f);
		AddRigidBody(m_spRigidBody, shapeRail, *pColID_Mystery, m_spMatrixNodeTransform);
		shapeRail->removeReference();




		// Event collision example
		// -----------------------

		m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
		m_spNodeEventCollision->m_Transform.SetPosition(CVector(0, 2, 0));
		m_spNodeEventCollision->NotifyChanged();
		m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());

		Havok::BoxShape* shapeEventTrigger = new Havok::BoxShape(2, 2, 2);
		AddEventCollision("Damage", shapeEventTrigger, *pColID_Mystery2, true, m_spNodeEventCollision);
		shapeEventTrigger->removeReference();

	}
};
enum WerehogState
{
	Normal,
	Dash,
	Guard
};
WerehogState currentState;

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
//replace with something that makes more sense
double calculateDistance(const std::vector<EKeyState>& array1, const std::vector<EKeyState>& array2) {
	if (array1.size() != array2.size()) {
		// Arrays must have the same size for a valid comparison
		return -1.0;
	}

	double sum = 0.0;
	for (size_t i = 0; i < array1.size(); ++i) {
		double diff = array1[i] - array2[i];
		sum += diff * diff;
	}

	return std::sqrt(sum);
}
std::string GetStateNameFromTable(std::string in)
{
	for (size_t i = 0; i < XMLParser::animationTable.size(); i++)
	{
		if (XMLParser::animationTable[i].MotionName == in)
			return XMLParser::animationTable[i].FileName;
	}
}
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
	if (inputPtr->IsDown(Sonic::eKeyState_RightTrigger))
		return 25;
	else
		return 17;
}
void RegisterInputs()
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (timerCombo < timerComboMax)
	{
		auto state = inputPtr->TappedState;
		lastTap = inputPtr->DownState;
		if (state == eKeyState_A || state == eKeyState_X || state == eKeyState_Y)
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
}
void SpawnParticleOnHand(const char* glitterName, bool right)
{
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	if (right)
	{
		auto index = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Index1_L");
		auto middle = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Middle1_L");
		auto pinky = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Pinky1_L");
		auto ring = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Ring1_L");
		auto thumb = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Thumb1_L");
		if (!indexParticle_L)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), indexParticle_L, &index, glitterName, 1);
		if (!middleParticle_L)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), middleParticle_L, &middle, glitterName, 1);
		if (!pinkyParticle_L)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), pinkyParticle_L, &pinky, glitterName, 1);
		if (!ringParticle_L)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), ringParticle_L, &ring, glitterName, 1);
		if (!thumbParticle_L)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), thumbParticle_L, &thumb, glitterName, 1);
	}
	else
	{
		auto index = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Index1_R");
		auto middle = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Middle1_R");
		auto pinky = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Pinky1_R");
		auto ring = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Ring1_R");
		auto thumb = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Thumb1_R");
		if (!indexParticle_R)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), indexParticle_R, &index, glitterName, 1);
		if (!middleParticle_R)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), middleParticle_R, &middle, glitterName, 1);
		if (!pinkyParticle_R)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), pinkyParticle_R, &pinky, glitterName, 1);
		if (!ringParticle_R)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), ringParticle_R, &ring, glitterName, 1);
		if (!thumbParticle_R)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), thumbParticle_R, &thumb, glitterName, 1);
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
void ExecuteAttackCommand(std::string attack, int attackIndex, bool starter = false)
{
	//comboAttackIndex = attackIndex;
	auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, true);
	Common::CreatePlayerSupportShockWave(playerContext->m_spMatrixNode->m_Transform.m_Position, 0.15f, 5, 0.1f);
	PlayAnim(GetStateNameFromTable(attack));
	/*Common::PlaySoundStatic(sound, attacks.at(attackIndex).cueIDs[comboIndex]);*/
	lastAttackName = attack;
	SpawnParticleOnHand("slash", true);
	SpawnParticleOnHand("slash", false);
	timerAttack = 0;
}
void SetPlayerVelocity()
{
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	switch (currentState)
	{
	case WerehogState::Dash:
	{
		playerContext->m_MaxVelocity = 20;
		break;
	}
	case WerehogState::Normal:
	{
		playerContext->m_MaxVelocity = 10;
		break;
	}
	case WerehogState::Guard:
	{
		playerContext->m_MaxVelocity = 2;
		break;
	}
	}
}
//find a better way please
bool init = false;
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
HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	originalCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

	// Force disable extended boost.
	*(uint32_t*)((uint32_t)*CONTEXT->ms_pInstance + 0x680) = 1;
	CONTEXT->m_ChaosEnergy = min(CONTEXT->m_ChaosEnergy, 100);
	Hedgehog::Base::CSharedString stateCheck = playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
	std::string stateCheckS(stateCheck.c_str());

	if (!init)
	{
		//playerContext->m_JumpThrust = CVector(playerContext->m_JumpThrust.x(), 1, playerContext->m_JumpThrust.z());
		/*playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_BoostEnableChaosEnergy] = 1000.0f;
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_AirBoostEnableChaosEnergy] = 1000.0f;
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_EnableHurdleJumpMinVelocity] = 1000.0f;
		playerContext->m_pSkills = 0;*/


	}
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
	DebugDrawText::log((std::string("AttackAnim") + std::string(lastAttackName)).c_str(), 0);

	DebugDrawText::log(stateCheckS.c_str(), 0);
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (inputPtr->IsDown(eKeyState_X) || inputPtr->IsDown(eKeyState_Y))
	{
		timerCombo = 0;
	}
	RegisterInputs();
	Particle_Checker();
	SetPlayerVelocity();
	if (isGrounded && jumpcount > 0)
		jumpcount = 0;
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

		collision1 = boost::make_shared<CBasicSphere>();
		Sonic::CGameDocument::GetInstance()->AddGameObject(collision1);
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
					for (size_t i = 0; i < XMLParser::starterAttacks.size(); i++)
					{
						if (XMLParser::starterAttacks[i].ActionName == attackName)
						{
							if (XMLParser::starterAttacks[i].MotionName.empty())
								return;
							comboAttackIndex = i;
							timerCombo = 0;
							playingAttack = true;
							ExecuteAttackCommand(XMLParser::starterAttacks[i].MotionName, comboAttackIndex, true);
							break;
						}
					}
				}

				if (comboProgress >= 1)
				{
					switch (currentButtonChain[comboProgress])
					{
					case eKeyState_X:
					{
						for (size_t i = 0; i < XMLParser::attacks.size(); i++)
						{
							if (lastAttackName == XMLParser::attacks[i].ActionName)
							{
								if (XMLParser::attacks[i].KEY__XDown.empty())
									continue;
								comboAttackIndex = i;
								timerCombo = 0;
								playingAttack = true;
								ExecuteAttackCommand(XMLParser::attacks[i].KEY__XDown, comboAttackIndex, false);
								break;
							}
						}

						break;
					}
					case eKeyState_Y:
					{
						for (size_t i = 0; i < XMLParser::attacks.size(); i++)
						{
							if (lastAttackName == XMLParser::attacks[i].ActionName)
							{
								if (XMLParser::attacks[i].KEY__YDown.empty())
									continue;
								comboAttackIndex = i;
								timerCombo = 0;
								playingAttack = true;
								ExecuteAttackCommand(XMLParser::attacks[i].KEY__YDown, comboAttackIndex, false);
								break;
							}
						}
						break;
					}
					case eKeyState_A:
					{
						for (size_t i = 0; i < XMLParser::attacks.size(); i++)
						{
							if (lastAttackName == XMLParser::attacks[i].ActionName)
							{
								if (XMLParser::attacks[i].KEY__ADown.empty())
									continue;
								comboAttackIndex = i;
								timerCombo = 0;
								playingAttack = true;
								ExecuteAttackCommand(XMLParser::attacks[i].KEY__ADown, comboAttackIndex, false);
								break;
							}
						}
						break;
					}
					}
				}

			}
		}
	}
	DebugDrawText::log((std::string("Jump count: ") + std::to_string(jumpcount)).c_str(), 0);
	if (inputPtr->IsTapped(Sonic::eKeyState_A) && !playingAttack)
	{
		isGrounded = false;
		
	}

	timerCombo += in_rUpdateInfo.DeltaTime;
	timerAttack += in_rUpdateInfo.DeltaTime;

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

	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	playerContext->m_ChaosEnergy = 0;
	lifeWerehog = 5.0f;

}
int timerHoming;
HOOK(int, __fastcall, HomingBegin, 0x01232040, CQuaternion* This)
{
	if (CONTEXT->m_HomingAttackTargetActorID)
	{
		return originalHomingBegin(This);
	}
	else

		return 0;
}
HOOK(void, __fastcall, CSonicProcMsgDamage, 0xE27890, Sonic::Player::CSonic* This, void* _, hh::fnd::Message& in_rMsg)
{
	lifeWerehog -= 1;
	if (lifeWerehog > 0)
	{
		PlayAnim("Evilsonic_damageMB");
	}
	else
	{
		CONTEXT->m_RingCount = 0;
		originalCSonicProcMsgDamage(This, _, in_rMsg);

	}
}
HOOK(char, __fastcall, XButtonInput, 0x00DFDF20, CSonicContext* This)
{

	if (playingAttack)
	{
		const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		playerContext->m_Velocity = CVector(0, 0, 0);
	}
	return 0;
}
HOOK(char, __fastcall, XButtonHoming_ChangeToHomingAttack, 0x00DFFE30, CSonicContext* This, void* Edx, int a2)
{
	return 0;
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
	if (GetServiceGameplay(Sonic::CApplicationDocument::GetInstance())[1] == 1)
	{
		if (jumpcount == 0)
		{
			PlayAnim("JumpEvil1");
		}
		else
		{
			if (canJump)
			{
				canJump = false;
				AddJumpThrust(Sonic::Player::CPlayerSpeedContext::GetInstance(), true);
				PlayAnim("JumpEvil2");
			}
		}
		jumpcount++;
	}
}
HOOK(char, __fastcall, JumpStart, 0x01114CB0, int* This)
{
	if (playingAttack == true)
		return 0;
	auto retu = originalJumpStart(This);
	return  retu;

}
HOOK(char, __fastcall, HomingStart, 0x00DC50D0, CSonicContext* This, void* Edx, int a2)
{
	return 0;
}

HOOK(void, __fastcall, MotoraDamage, 0x00BC7440, int This, void* Edx, Hedgehog::Universe::MessageTypeSet* a2)
{
}
HOOK(void, __fastcall, ProcMsgRestartStage, 0xE76810, Sonic::Player::CPlayer* This, void* Edx, hh::fnd::Message& message)
{
	originalProcMsgRestartStage(This, Edx, message);
	lifeWerehog = 5.0f;
}
extern "C" __declspec(dllexport) float API_GetLife()
{
	return lifeWerehog;
}
void evSonic::Install()
{
	/*CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCA", "evilsonic_attackNCA");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCB", "evilsonic_attackNCB");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCC", "evilsonic_attackNCC");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCD", "evilsonic_attackNCD");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCE", "evilsonic_attackNCE");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNSA", "evilsonic_attackNSA");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNSB", "evilsonic_attackNSB");*/
	CustomAnimationManager::RegisterAnimation("Evilsonic_damageMB", "evilsonic_damageMB");
	CustomAnimationManager::RegisterAnimation("Evilsonic_guard_idle", "evilsonic_guard_idle");
	CustomAnimationManager::RegisterAnimation("Evilsonic_BerserkerS", "evilsonic_BerserkerS");
	CustomAnimationManager::RegisterAnimation("JumpEvil1", "evilsonic_jumpVS");
	CustomAnimationManager::RegisterAnimation("JumpEvil2", "evilsonic_jumpVS2");


	/*const char* comboName;
	const char* stateName;
	Sonic::EKeyState combo[5];
	int cueIDs[5];
	const char* animations[5];*/
	/*attacks.push_back(WerehogAttack({ "Wild Whirl", {eKeyState_X,eKeyState_X,eKeyState_X,eKeyState_X,eKeyState_X }, {134,134,135,136,136 }, {"Evilsonic_attackNCA","Evilsonic_attackNCB","Evilsonic_attackNCC","Evilsonic_attackNCD","Evilsonic_attackNCE"}, {0.15F,0.15F,0.15F,0.55F,0.85f}, 1 }));
	attacks.push_back(WerehogAttack({ "Wild Whirl2", {eKeyState_Y,eKeyState_Y }, {134,134,135,136,136 }, {"Evilsonic_attackNSA","Evilsonic_attackNSB"}, {0.15F,0.15F},1 }));*/
	WRITE_MEMORY(0x01271FD1, char*, "ef_null"); // Disable original game's jumpball creation
	WRITE_MEMORY(0x015E9078, char*, "ef_null"); // Disable empty boost

	INSTALL_HOOK(XButtonHoming_ChangeToHomingAttack);
	INSTALL_HOOK(XButtonInput);
	INSTALL_HOOK(JumpStart);
	INSTALL_HOOK(Jump_PlayAnimation);
	INSTALL_HOOK(MotoraDamage);
	INSTALL_HOOK(HomingStart);
	INSTALL_HOOK(CPlayerAddCallback);
	INSTALL_HOOK(ProcMsgRestartStage);
	INSTALL_HOOK(CSonicProcMsgDamage);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	INSTALL_HOOK(HomingBegin);
	//WRITE_JUMP(0x01232055, 0x01232073);
	//Unmap stomp
	WRITE_JUMP(0X00DFDD72, 0X00DFDDF4);

	/*WRITE_MEMORY(0x015D71F8, char*, "chr_Sonic_EV");
	WRITE_MEMORY(0x015EF200, char*, "chr_Sonic_EV");
	WRITE_MEMORY(0x015D0CB8, char*, "chr_Sonic_EV");
	WRITE_MEMORY(0x015D0C00, char*, "chr_Sonic_EV");
	WRITE_MEMORY(0x015B1ADC, char*, "chr_Sonic_EV");
	WRITE_MEMORY(0x015B1ACC, char*, "chr_Sonic_EV");*/
	//Force Jump stuff
	WRITE_JUMP(0x01114A14, 0x01114A39);
	WRITE_JUMP(0x01114D95, 0x01114DDA);

	////Unmap boost (use chaos energy as unleashed meter)
	//WRITE_MEMORY(0x0111BEEC, uint32_t, -1);
	//WRITE_NOP(0xDFE1C4, 2);
	//WRITE_NOP(0xDFE1D2, 2);
	//WRITE_MEMORY(0x00DF1D91, uint32_t, -1);

	//Disable Spindash
	WRITE_JUMP(0x00DC28D9, 0x00DC2946);

	//Replace jump sound with the werehog's

	//WRITE_JUMP(0x011BEFF4, 0x011BF036);
	////From Brianuu - Disable squat and sliding
	//WRITE_MEMORY(0xDFF8D5, uint8_t, 0xEB, 0x05);
	//WRITE_MEMORY(0xDFF856, uint8_t, 0xE9, 0x81, 0x00, 0x00, 0x00);

	////From Hyper - unmap drift
	//WRITE_MEMORY(0xDF2DFF, uint32_t, -1);
	//WRITE_MEMORY(0xDFF62B, uint32_t, -1);
	//WRITE_MEMORY(0xDFF816, uint32_t, -1);
	//WRITE_MEMORY(0x1119549, uint32_t, -1);
	//WRITE_MEMORY(0x119910D, uint32_t, -1);
}