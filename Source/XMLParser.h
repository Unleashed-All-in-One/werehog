#pragma once
enum ResourceType
{
	CSB,
	Effect
};
struct Param
{
	std::string FileName;
	std::string Cue;
};
struct Trigger_Frame
{
	int Type;
	int Start;
	int End;
};
struct Trigger
{
	int ResourceID;
	Trigger_Frame Frame;
	bool IsFollowNode;
	bool IsInheritPositionOnly;
	std::string NodeName;
	Hedgehog::math::CVector Translation;
	Hedgehog::math::CQuaternion Rotation;
	Hedgehog::math::CVector Scale;
};
struct Resource
{
	int ID;
	ResourceType Type;
	Param Params;
};
struct ResourceInfo
{
	std::vector<Resource> Resources;
};
struct TriggerInfo
{
	std::vector<Trigger> Resources;
};
struct EffectConfig
{
	std::string REffect_Name1;
	std::string LEffect_Name1;
};
struct MoveRatioHelper
{
	float FrameStart;
	float FrameValue;
	float FrameValue_Y;
};
struct Motion {
	std::string MotionName;
	std::string FileName;
	float MotionBlendTimeS;
	float MotionBlendTimeE;
	float MotionBlendTimeEIdle;
	float MotionMoveSpeedRatio;
	float MotionMoveSpeedRatio_Y;
	std::vector< MoveRatioHelper> MotionMoveSpeedRatio_H;
	std::vector< MoveRatioHelper> MotionMoveSpeedRatio_H_Y;
	std::vector<float> MotionMoveSpeedRatioFrameStart;
	std::vector<float> MotionMoveSpeedRatioFrame;
	std::vector<float> MotionMoveSpeedRatioFrameY;
	float MotionFirstSpeed;
	float MiddleSpeed;
	float EndSpeed;
	float MotionSpeed_FirstFrame;
	float MotionSpeed_MiddleFrame;
	int MotionSpeed_EndFrame;
	std::string AttackScope;
	float DontCatchTime;
	bool DontJump;
	int ShakeCameraStartFrame;
	float GroundHeightRepair;
	std::string MoveType;
	std::string ComposedFileName;
	EffectConfig Effect;

	ResourceInfo ResourceInfos;
	TriggerInfo TriggerInfos;
};
struct WerehogAttackNew
{
	std::string ActionName;
	std::string MotionName;
	int ValidLevel_Min;
	int ValidLevel_Max;
	bool ValidCommon;
	bool ValidBerserker;
	std::string KEY__YDown;
	std::string KEY__XDown;
	std::string KEY__ADown;
	std::string KEY__Land;
	std::string KEY__AirCombo;
	std::string KEY__End;
	bool Guard;
	bool Avoid;
	float KEY__StartFrame;
	float KEY__EndFrame;
	float WaitEndMotionEndFrame;
	float WaitEndMotionSpeed;
	float EndMotionSpeed;
	float LandStartFrame;
	float ActionValidHeightMin;
	bool IsGravity;
	Motion Werehog_AssociatedMotion;
	//From the respective attacks tbres files

};
class XMLParser
{

public:
	static void Install(const char* path);
	static std::vector<WerehogAttackNew> attacks;
	static std::vector<WerehogAttackNew> starterAttacks;
	static std::vector<Motion> animationTable;
	static std::string CLAWPARTICLE;
	static Motion GetMotionFromName(std::string in);
	static WerehogAttackNew GetAttackFromName(std::string in);
	static std::string GetStateNameFromTable(std::string in);

};

