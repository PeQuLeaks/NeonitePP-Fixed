/**
 * Copyright (c) 2020-2021 Kareem Olim (Kemo)
 * All Rights Reserved. Licensed under the Neo License
 * https://neonite.dev/LICENSE.html
 */

#include "pch.h"
#include "curlhooks.h"
#include "hooks.h"
#include "gui/gui.h"

bool WINAPI ImguiThread()
{
	while (true)
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, reinterpret_cast<void**>(&oPresent), hkPresent);
			break;
		}
	};
	return true;
}

void WINAPI dllMain()
{
#ifdef CONSOLE
	FILE* fDummy;
	AllocConsole();
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	printf(XOR("\n\n[=] Cranium made by Kemo, Sizzy, PeQu and Timmy \n\n"));
#endif

#ifdef PLATANIUMV2
	Hooks::Init();
#endif


#ifdef HOOKS
	while (true)
	{
		//if (isReady)
		//{
		//	//Deprecated
		//	if (Hooks::Misc(99.99f)) //&& Console::Unlock()) //&& ForceSettings())
		//	{
		//		ImguiThread();
		//		break;
		//	}
		//}
		 if (isReady || GetAsyncKeyState(VK_F3))
		{
			if (Hooks::Misc(17.30f))
			{
				ImguiThread();
				break;
			}
		}
	}
#endif
}
//Apollo_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C



BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		dllMain();
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		break;
	}
	return TRUE;
}
