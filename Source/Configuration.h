#pragma once

class Configuration
{
private:
	static float m_deltaTime;
	
public:
	/// <summary>
	/// Reads the INI configuration file for the mod.
	/// </summary>
	static void Read();

	static void setDeltaTime(float dt) { m_deltaTime = dt; }
	static float getDeltaTime() { return m_deltaTime; }

};

