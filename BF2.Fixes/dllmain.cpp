// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Tools.h"
#include <vector>
#include <string>
#include "detours.h"

#pragma comment(lib, "detours.lib")

static char* BF2ServerPassword;

typedef int(__thiscall* __real_NetClient_JoinServer)(DWORD* _this, int a2, int port, int a4, bf2string* password_param, float a6, int a7, int anticheat_param);

__real_NetClient_JoinServer realNetClientJoinServer;

void parseArguments()
{
	LPWSTR params = GetCommandLine();
	std::wstring separators = L" ";
	std::vector<std::wstring> args = splitManyW(params, separators);
	std::vector<std::wstring> arguments = std::vector<std::wstring>();
	for (int i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 9) == L"+password")
		{
			arguments.push_back(args[i]);
			arguments.push_back(args[i + 1]);
		}
	}
	for (int i = 0; i < arguments.size(); i++)
	{
		if (arguments[i] == L"+password")
		{
			const wchar_t* password = (wchar_t*)arguments[i + 1].c_str();
			size_t requiredSize = std::wcstombs(NULL, password, 0) + 1;
			BF2ServerPassword = (char*)malloc(requiredSize);
			std::wcstombs(BF2ServerPassword, password, requiredSize);
		}
	}
}

int __fastcall DetourNetClientJoinServer(DWORD* _this, DWORD EDX, int a2, int port, int a4, bf2string* password_param, float a6, int a7, int anticheat_param)
{
	if (BF2ServerPassword && (!password_param || password_param->size() == 0))
	{
		bf2string password = bf2string(BF2ServerPassword);
		return realNetClientJoinServer(_this, a2, port, a4, &password, a6, a7, anticheat_param);
	}

	return realNetClientJoinServer(_this, a2, port, a4, password_param, a6, a7, anticheat_param);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		parseArguments();
		realNetClientJoinServer = (__real_NetClient_JoinServer)DetourFunction((PBYTE)0x6BEE80, (PBYTE)DetourNetClientJoinServer);
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

