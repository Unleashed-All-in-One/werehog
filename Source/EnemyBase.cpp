struct EnemyList
{
	std::unordered_map<Sonic::CGameObject3D*, float> enemyMap;
};
EnemyList enemyList;
HOOK(void, __fastcall, EnemyBaseAdd, 0x0BDD8D0, Sonic::CGameObject3D* This, void* Edx, int a2)
{
	enemyList.enemyMap.insert(std::make_pair(This, 5.0f));
	return originalEnemyBaseAdd(This, Edx, a2);
}
HOOK(void, __fastcall, MotoraDamage, 0x00BC7440, Sonic::CGameObject3D* This, void* Edx, hh::fnd::Message& a2)
{
	for (auto x : enemyList.enemyMap)
	{
		if (x.first == This)
		{
			enemyList.enemyMap[x.first] -= 1;
			Sonic::CGameObject3D* test = (Sonic::CGameObject3D*)(This);
			auto& position = test->m_spMatrixNodeTransform->m_Transform.m_Position;
			const auto camera = Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera();
			hh::math::CVector4 screenPosition = camera->m_MyCamera.m_View * hh::math::CVector4(position.x(), position.y(), position.z(), 1.0f);
			screenPosition = camera->m_MyCamera.m_Projection * screenPosition;
			screenPosition.head<2>() /= screenPosition.w();
			auto e = DebugDrawText::Location();
			e.x = screenPosition.x();
			e.y = screenPosition.y();
			DebugDrawText::draw(std::format("MotoraDamage: {0}/5", x.second).c_str(), e, 1.77777f);
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

void EnemyBase::Install()
{
	enemyList = EnemyList();
	INSTALL_HOOK(EnemyBaseAdd);
	INSTALL_HOOK(MotoraDamage);
	INSTALL_HOOK(EnemyBaseDeath);
}