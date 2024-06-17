EvilParameters* EvilGlobal::parameters;
std::string EvilGlobal::lastAttackName;
boost::shared_ptr<Sonic::CGameObject3D> EvilGlobal::shockwaveGameObject;
boost::shared_ptr<Sonic::CMotionCameraController> EvilGlobal::m_spMotionCameraController;

Motion EvilGlobal::GetMotionFromName(std::string in_Name)
{
	for (size_t i = 0; i < XMLParser::animationTable.size(); i++)
	{
		if (XMLParser::animationTable[i].MotionName == in_Name)
			return XMLParser::animationTable[i];
	}
}
std::string EvilGlobal::GetStateNameFromTable(std::string in)
{
	for (size_t i = 0; i < XMLParser::animationTable.size(); i++)
	{
		if (XMLParser::animationTable[i].MotionName == in)
			return XMLParser::animationTable[i].FileName;
	}
}
WerehogAttackNew EvilGlobal::GetAttackFromName()
{
	auto motion = GetMotionFromName(EvilGlobal::lastAttackName);
	for (size_t i = 0; i < XMLParser::attacks.size(); i++)
	{
		if (XMLParser::attacks[i].MotionName == motion.MotionName)
			return XMLParser::attacks[i];
	}
}
void EvilGlobal::initializeValues()
{
	EvilParameters* params = new EvilParameters();
	params->attackVelocityDivider = 1.5f;
	params->lifeMaxAmount = 15;
	parameters = params;
}