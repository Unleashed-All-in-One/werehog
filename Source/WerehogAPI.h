#pragma once

// Standard library
#include <string>
#include <unordered_map>


// Creates a pointer to a function from a loaded library.
#define LIB_FUNCTION(returnType, libraryName, procName, ...) \
    typedef returnType _##procName(__VA_ARGS__); \
    _##procName* procName = (_##procName*)GetProcAddress(GetModuleHandle(TEXT(libraryName)), #procName);

// Creates a void export based on the API's requirements.
#define VOID_EXPORT(libFunctionName, ...) \
	if (GetInstance()->libFunctionName == nullptr) \
		return; \
	GetInstance()->libFunctionName(__VA_ARGS__);

// Creates an integer export based on the API's requirements.
#define INT_EXPORT(libFunctionName, ...) \
	if (GetInstance()->libFunctionName == nullptr) \
		return -1; \
	return GetInstance()->libFunctionName(__VA_ARGS__);

// Creates an integer export based on the API's requirements.
#define FLOAT_EXPORT(libFunctionName, ...) \
	if (GetInstance()->libFunctionName == nullptr) \
		return -1.0f; \
	return GetInstance()->libFunctionName(__VA_ARGS__);

// Creates a Boolean export based on the API's requirements.
#define BOOL_EXPORT(libFunctionName, ...) \
	if (GetInstance()->libFunctionName == nullptr) \
		return false; \
	return GetInstance()->libFunctionName(__VA_ARGS__);

// Creates a generic export based on the API's requirements.
#define GENERIC_EXPORT(returnType, libFunctionName, ...) \
	if (GetInstance()->libFunctionName == nullptr) \
		return returnType(); \
	return GetInstance()->libFunctionName(__VA_ARGS__);


class WerehogAPI
{
private:

	LIB_FUNCTION(float, "Werehog.dll", API_GetLife);

public:
	/// <summary>
	/// Gets the current instance of the API and creates a new one if it's a null pointer.
	/// </summary>
	static WerehogAPI* GetInstance()
	{
		static WerehogAPI* instance;

		return instance != nullptr ? instance : instance = new WerehogAPI();
	}

	static float GetLife()
	{
		FLOAT_EXPORT(API_GetLife);
	}

	/// <summary>
	/// Checks if the Score Generations module is attached to the current process.
	/// </summary>
	static bool IsAttached()
	{
		if (GetModuleHandle(TEXT("Werehog.dll")) == nullptr)
			return false;

		return true;
	}
};