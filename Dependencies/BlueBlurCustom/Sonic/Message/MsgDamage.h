#pragma once

#include <Hedgehog/Universe/Engine/hhMessage.h>

namespace Sonic::Message
{
	struct MsgDamage : public Hedgehog::Universe::MessageTypeSet
	{
	public:
		HH_FND_MSG_MAKE_TYPE(0x01681E80);
		BYTE gap4[12]; //padding
		int* collisionMask; // seems like its not used?
		DWORD m_Unknown1; //???
		BYTE gap18[8];	//padding
		Hedgehog::Math::CVector* m_HitPosition1;
		Hedgehog::Math::CVector* m_HitPosition2; // copy of m_HitPosition1?
		Hedgehog::Math::CVector m_LaunchVelocity; // for objectphysics, determines how far the broken parts go
		DWORD dword50; // unknown, seems to be either set to 0 or 1	
	};
}