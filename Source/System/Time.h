#pragma once
class Time
{
public:
	static bool freeze;
	static void registerPatches();
	static void SetTimescale(float timeScale);
};