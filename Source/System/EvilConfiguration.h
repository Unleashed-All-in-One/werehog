#pragma once
struct EvilParameters
{
	float attackVelocityDivider;
	float lifeMaxAmount;
	float lifeCurrentAmount;
	float movementSpeed;
	float turnRate;
	float timerComboMax = 0.75f;
	float timerDamageMax = 0.3f;
	float timerAttackMax = 0.35f;
};
class EvilGlobal
{
public:
	static std::string lastAttackName;
	static boost::shared_ptr<Sonic::CGameObject3D> shockwaveGameObject;
	static EvilParameters* parameters;

	static Motion GetMotionFromName(std::string in_Name);
	static WerehogAttackNew GetAttackFromName();
	static std::string GetStateNameFromTable(std::string in);
	static void initializeValues();
};