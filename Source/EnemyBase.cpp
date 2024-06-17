struct EnemyList
{
	std::unordered_map<Sonic::CGameObject3D*, float> enemyMap;
};
EnemyList enemyList;
SharedPtrTypeless musicBattle;
std::string previousBGM;
int attackChain = 0;
float timerAttackChain = 0;
float timerAttackChainMax = 3;
HOOK(void, __fastcall, EnemyBaseAdd, 0x0BDD8D0, Sonic::CGameObject3D* This, void* Edx, int a2)
{
	enemyList.enemyMap.insert(std::make_pair(This, 5.0f));
	return originalEnemyBaseAdd(This, Edx, a2);
}
bool playingBGM;
HOOK(void, __fastcall, MotoraDamage, 0x00BC7440, Sonic::CGameObject3D* This, void* Edx, hh::fnd::Message& a2)
{
	attackChain++;
	timerAttackChain = 0;
	if (!playingBGM)
	{
		previousBGM = Sonic::CGameDocument::GetInstance()->m_pMember->m_AudioData[0].m_SongName.c_str();
		Common::StopBGM(Sonic::CGameDocument::GetInstance()->m_pMember->m_AudioData[0].m_SongName.c_str(), 0.1f);
		Common::PlayBGM("evil_battle1", 0);
		playingBGM = true;
	}
	for (auto x : enemyList.enemyMap)
	{
		if (x.first == This)
		{
			
			enemyList.enemyMap[x.first] -= 1;
			Sonic::CGameObject3D* test = (Sonic::CGameObject3D*)(This);
			auto& position = test->m_spMatrixNodeTransform->m_Transform.m_Position;
			Eigen::Vector4f positionFixed = Eigen::Vector4f(position.x(), position.y() + 2, position.z(), 1);
			Eigen::Vector4f screenPosition;
			Common::fGetScreenPosition(positionFixed, screenPosition);
			DebugDrawText::draw(std::format("MotoraDamage: {0}/5", x.second).c_str(), DebugDrawText::Location(screenPosition.x(), screenPosition.y()), 1.77777f);
			if (x.second <= 0)
				originalMotoraDamage(This, Edx, a2);
		}
	}
}
HOOK(int*, __fastcall, EnemyBaseDeath, 0x00BDEBE0, Sonic::CGameObject3D* This, void* Edx, int a2)
{
	for (auto x : enemyList.enemyMap)
	{
		if (x.first == This)
		{
			enemyList.enemyMap.erase(This);
			break;
		}
	}
	return originalEnemyBaseDeath(This, Edx, a2);
}
void Debug_EnemyHealth()
{
	for (auto x : enemyList.enemyMap)
	{
		Sonic::CGameObject3D* test = (Sonic::CGameObject3D*)(x.first);
		auto& position = test->m_spMatrixNodeTransform->m_Transform.m_Position;
		const auto camera = Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera();
		hh::math::CVector4 screenPosition = camera->m_MyCamera.m_View * hh::math::CVector4(position.x(), position.y(), position.z(), 1.0f);
		screenPosition = camera->m_MyCamera.m_Projection * screenPosition;
		screenPosition.head<2>() /= screenPosition.w();
		DebugDrawText::draw(std::format("MotoraDamage: {0}/5", x.second).c_str(), DebugDrawText::Location(screenPosition.x(), screenPosition.y()));
	}
}
extern "C" __declspec(dllexport) EnemyList API_GetEnemyList()
{
	return enemyList;
}
extern "C" __declspec(dllexport) int API_GetHitCount()
{
	return attackChain;
}
extern "C" __declspec(dllexport) int API_IsAttacking()
{
	return timerAttackChain <= timerAttackChainMax;
}
HOOK(void, __fastcall, EnemyBaseUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	originalEnemyBaseUpdateParallel(This, Edx, in_rUpdateInfo);
	if (timerAttackChain < timerAttackChainMax)
		timerAttackChain += in_rUpdateInfo.DeltaTime;
	else
	{
		attackChain = 0;
	}
	if (enemyList.enemyMap.size() == 0)
	{
		if (playingBGM)
		{
			Common::StopBGM("evil_battle1", 6);
			Common::PlayBGM(previousBGM.c_str(), 6);
			playingBGM = false;
		}
	}

	DebugDrawText::log((std::format("Timer Attack Chain: ") + std::to_string(timerAttackChain)).c_str(), 0);

}
void EnemyBase::registerPatches()
{
	enemyList = EnemyList();
	INSTALL_HOOK(EnemyBaseUpdateParallel);
	INSTALL_HOOK(EnemyBaseAdd);
	INSTALL_HOOK(MotoraDamage);
	INSTALL_HOOK(EnemyBaseDeath);
}