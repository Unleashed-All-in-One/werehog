#pragma once

using namespace hh::math;
class CAttackHitbox : public Sonic::CGameObject3D
{
public:
	std::string hitboxName;
	boost::shared_ptr<hh::mr::CSingleElement> m_spRenderable;
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
	boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;
	float m_Timer;
	float m_TimerMax = 20;
	Havok::SphereShape* shapeEventTrigger = new Havok::SphereShape(1);

	void BindToBone(const char* boneName)
	{
		hitboxName = boneName;
		if (m_spMatrixNodeTransform)
		{
			m_spMatrixNodeTransform->NotifyChanged();
			auto node = Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_spCharacterModel->GetNode(boneName);
			m_spMatrixNodeTransform->SetParent(node.get());
			m_spMatrixNodeTransform->m_Transform.SetPosition(CVector(0, 0, 0));
		}
		else
		{
			printf("\nTried to bind to bone, but the matrixnode doesn't exist!");
		}
	}
	void Kill()
	{

	}
	virtual void AddCallback(const hh::base::THolder<Sonic::CWorld>& worldHolder,
		Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<hh::db::CDatabase>& spDatabase) override
	{
		Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
		pGameDocument->AddUpdateUnit("0", this);
		//Uncomment for a debug sphere
		//hh::mr::CMirageDatabaseWrapper wrapper(spDatabase.get());
		//// This is a debug asset that has a broken material, so it will be pure red--but that's ok, cuz we can see it.
		//const char* assetName = "BasicSphere";
		//boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);

		//m_spRenderable = boost::make_shared<hh::mr::CSingleElement>(spModelData);
		/*if (!spModelData)
			return;*/

			//m_spRenderable->BindMatrixNode(m_spMatrixNodeTransform);
			//AddRenderable("Object", m_spRenderable, true);

		m_spMatrixNodeTransform->NotifyChanged();
		m_spMatrixNodeTransform->m_Transform.SetPosition(CVector(0, 0, 0));

		m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
		m_spNodeEventCollision->m_Transform.SetPosition(CVector(0, 0, 0));
		m_spNodeEventCollision->NotifyChanged();
		m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());

		AddEventCollision("ItemBoost", shapeEventTrigger, 27, false, m_spNodeEventCollision);
	}
	void UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo) override
	{
		m_Timer += updateInfo.DeltaTime;
		if (m_Timer > m_TimerMax)
		{
			SendMessage(this->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		}
	}
	bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
	{
		if (in_Flag)
		{
			if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
			{
				auto vector1 = CVector(100, 100, 100);
				auto out_msgDamage = boost::make_shared<Sonic::Message::MsgDamage>();
				out_msgDamage->collisionMask = (int*)0x01E0BE34;
				out_msgDamage->m_HitPosition1 = &vector1;
				out_msgDamage->m_HitPosition2 = &vector1;
				out_msgDamage->m_LaunchVelocity = Sonic::Player::CPlayerSpeedContext::GetInstance()->m_Velocity;
				out_msgDamage->dword50 = true;
				SendMessage(in_rMsg.m_SenderActorID, out_msgDamage);
				return true;
			}
		}
		return Sonic::CGameObject::ProcessMessage(in_rMsg, in_Flag);
	};
	
};


