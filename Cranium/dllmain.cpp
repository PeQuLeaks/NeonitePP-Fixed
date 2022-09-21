#include "pch.h"
#include "curlhooks.h"
#include "hooks.h"
#include "gui/gui.h"
#include "kismet.h"

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

void dllMain()
{
	KismetFunctions::CreateConsole();
	Hooks::curlhook();
	Hooks::exithook();

#ifdef HOOKS
	while (true)
	{
		if (isReady)
		{	
			if (MH_Initialize() != MH_OK)
			{
				MessageBoxA(nullptr, XOR("Failed to initialize min-hook, terminating the thread."), XOR("Carbon"), MB_OK);
				FreeLibraryAndExitThread(GetModuleHandle(nullptr), 0);
			}
			if (Hooks::Misc(gVersion) && Console::Unlock())
			{
				ImguiThread();
				break;
			}
		}
		Sleep(1000 / 30); //30 fps  
	}
#endif
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
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
