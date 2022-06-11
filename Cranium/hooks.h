#pragma once
#include "pch.h"
#include "structs.h"
#include "enums.h"
#include "util.h"
#include "detours.h"
#include "ue4.h"
#include "veh.h"

static void* CurlEasyAddress;
static void* CurlSetAddress;
//UE4
static void* RequestExitWithStatusAddressUE4;
static void* RequestExitAddressUE4;
static void* UnsafeEnvironmentPopupAddressUE4;
//UE5
static void* UnsafeEnvironmentPopupAddressUE5;
static void* RequestExitWithStatusAddressUE5;

namespace Hooks
{
	inline bool curlhook()
	{
		//curl hooking, tested on 3.50+
		CurlEasyAddress = Util::FindPatternAV(Patterns::Curl::CurlEasySetOpt.first, Patterns::Curl::CurlEasySetOpt.second);
		if (!CurlEasyAddress)
		{
			CurlEasyAddress = Util::FindPatternAV(Patterns::Curl::CurlEasySetOpt.first, Patterns::Curl::CurlEasySetOpt.second);
			printf("\n[CARBON] GAME VERSION NOT SUPPORTED.... FALLING BACK TO SAFE STATE!\n");
			printf("\n[CARBON] GAME VERSION NOT SUPPORTED.... FALLING BACK TO SAFE STATE!\n");
			printf("\n[CARBON] GAME VERSION NOT SUPPORTED.... FALLING BACK TO SAFE STATE!\n\n");
			VALIDATE_ADDRESS(CurlEasyAddress, "Curl Pattern Failed.");
		}

		CurlSetAddress = Util::FindPatternAV(Patterns::Curl::CurlSetOpt.first, Patterns::Curl::CurlSetOpt.second);
		VALIDATE_ADDRESS(CurlSetAddress, "Curl Set Pattern Failed.");

		CurlEasySetOpt = decltype(CurlEasySetOpt)(CurlEasyAddress);
		CurlSetOpt = decltype(CurlSetOpt)(CurlSetAddress);

		if (Util::FindPatternAV(Patterns::Misc::PushWidget.first, Patterns::Misc::PushWidget.second))
		{
			printf("\n\n[CARBON] Game version is above season 16!\n\n");
			DetoursEasy(CurlEasySetOpt, CurlEasySetOptDetour);
		}
		else {
			printf("\n\n[CARBON] Game version is below season 17!\n\n");
			VEH::EnableHook(CurlEasySetOpt, CurlEasySetOptDetour);
		}
		return true;
	}

	inline bool exithook()
	{
		UnsafeEnvironmentPopupAddressUE4 = Util::FindPatternAV(Patterns::Exit::UnsafeEnvironmentPopupUE4.first, Patterns::Exit::UnsafeEnvironmentPopupUE4.second);
		if (!UnsafeEnvironmentPopupAddressUE4)//We need the if statements to prevent the searching for the UE5 patterns, this causes it to lockup if we let it do that
		{
			UnsafeEnvironmentPopupAddressUE5 = Util::FindPatternAV(Patterns::Exit::UnsafeEnvironmentPopupUE5.first, Patterns::Exit::UnsafeEnvironmentPopupUE5.second);
		}
		RequestExitWithStatusAddressUE4 = Util::FindPatternAV(Patterns::Exit::RequestExitWithStatusUE4.first, Patterns::Exit::RequestExitWithStatusUE4.second);
		if (!RequestExitWithStatusAddressUE4)
		{
			RequestExitWithStatusAddressUE5 = Util::FindPatternAV(Patterns::Exit::RequestExitWithStatusUE5.first, Patterns::Exit::RequestExitWithStatusUE5.second);
		}
		if (gVersion > 15.90f)
		{
			DetoursEasy(UnsafeEnvironmentPopupAddressUE4, UnsafeEnvironmentPopupHook);
			DetoursEasy(UnsafeEnvironmentPopupAddressUE5, UnsafeEnvironmentPopupHook);

			DetoursEasy(RequestExitWithStatusAddressUE4, RequestExitWithStatusHook);
			DetoursEasy(RequestExitWithStatusAddressUE5, RequestExitWithStatusHook);
		}
		return true;
	}

	inline bool Misc(float version)
	{

		if (MH_Initialize() != MH_OK)
		{
			MessageBoxA(nullptr, XOR("Failed to initialize min-hook, terminating the thread."), XOR("Carbon"), MB_OK);
			FreeLibraryAndExitThread(GetModuleHandle(nullptr), 0);
		}

		printf("\n\n[CARBON] Started Hooks \n\n");

		//GObject Array
		auto GObjectsAdd = Util::FindPattern(Patterns::bGlobal::GObjects, Masks::bGlobal::GObjects);
		VALIDATE_ADDRESS(GObjectsAdd, XOR("Failed to find GObjects Address.\n"));
		GObjs = decltype(GObjs)(RELATIVE_ADDRESS(GObjectsAdd, 7));
		printf("\n\n[CARBON] Found GObjs! \n\n");

		//FNameToString
		if (gVersion > 16.60f)
		{
			auto FNameToStringAdd = Util::FindByString(FNAMETOSTRING_STRINGREF, { CALL }, true, 1);
			if (!FNameToStringAdd)
			{
				MessageBoxW(nullptr, L"Cannot find FNameToString.", L"Carbon", MB_OK);
				return false;
			}
			FNameToString = decltype(FNameToString)(FNameToStringAdd);
			printf("\n\n[CARBON] Found FNameToString! \n\n");
		}

		//GEngine Hook
		if (gVersion > 16.60f)
		{
			GEngine = FindObject<UEngine*>(XOR(L"FortEngine /Engine/Transient.FortEngine_"));
			VALIDATE_ADDRESS(GEngine, XOR("Failed to find GEngine Address.\n"));
			printf("\n\n[CARBON] Found GEngine! \n\n");
		}
		else if (gVersion > 9.40)
		{
			auto GEngineAdd = Util::FindPattern(Patterns::bGlobal::GEngine, Masks::bGlobal::GEngine);
			VALIDATE_ADDRESS(GEngineAdd, XOR("Failed to find GEngine Address."));
			GEngine = *reinterpret_cast<UEngine**>(GEngineAdd + 7 + *reinterpret_cast<int32_t*>(GEngineAdd + 3));
			printf("\n\n[CARBON] Found GEngine! \n\n");
		}
		else 
		{
			auto GEngineAdd = Util::FindPattern(Patterns::Oldies::bChapone::GEngine, Masks::Oldies::bChapone::GEngine);
			VALIDATE_ADDRESS(GEngineAdd, XOR("Failed to find GEngine Address."));
			GEngine = *reinterpret_cast<UEngine**>(GEngineAdd + 7 + *reinterpret_cast<int32_t*>(GEngineAdd + 3));
			printf("\n\n[CARBON] Found GEngine! \n\n");
		}

		//ProcessEvent Hook
		uintptr_t ProcessEventAdd = 0;
		if (gVersion >= 19.00f)
		{
			const auto vtable = *reinterpret_cast<void***>(GEngine);
			ProcessEventAdd = (uintptr_t)vtable[0x4B];
			printf("\n\n[CARBON] Found ProcessEvent! \n\n");
		}
		else if (gVersion > 16.00f)
		{
			const auto vtable = *reinterpret_cast<void***>(GEngine);
			ProcessEventAdd = (uintptr_t)vtable[0x44];
			printf("\n\n[CARBON] Found ProcessEvent! \n\n");
		}
		else
		{
			ProcessEventAdd = Util::FindPattern(Patterns::bGlobal::ProcessEvent, Masks::bGlobal::ProcessEvent);
			VALIDATE_ADDRESS(ProcessEventAdd, XOR("Failed to find ProcessEvent Address."));
		}

		ProcessEvent = decltype(ProcessEvent)(ProcessEventAdd);
		gProcessEventAdd = ProcessEventAdd;
		
		//Used for Camera Hooking.
		//Tested from 12.41 to 15.40
		if(gVersion < 16.00f)
		{
			auto GetViewPointAdd = Util::FindPattern(Patterns::bGlobal::GetViewPoint, Masks::bGlobal::GetViewPoint);
			VALIDATE_ADDRESS(GetViewPointAdd, XOR("Failed to find GetViewPoint Address."));

			GetViewPoint = decltype(GetViewPoint)(GetViewPointAdd);
		}

		//Used for getting UObjects names.
		//Tested from 12.41 to latest
		if (gVersion >= 16.00f)
		{
			auto GONIAdd = Util::FindPattern(Patterns::New::GONI, Masks::New::GONI);
			VALIDATE_ADDRESS(GONIAdd, XOR("Failed to find GetObjectName Address."));

			int32_t offset = *(int32_t*)(GONIAdd + 1);
			auto fnAddress = GONIAdd + 5 + offset;

			GetObjectNameInternal = decltype(GetObjectNameInternal)(fnAddress);
			printf("\n\n[CARBON] Found GetObjectNameInternal! \n\n");

		}
		else
		{
			auto GONIAdd = Util::FindPattern(Patterns::bGlobal::GONI, Masks::bGlobal::GONI);
			VALIDATE_ADDRESS(GONIAdd, XOR("Failed to find GetObjectName Address."));

			GetObjectNameInternal = decltype(GetObjectNameInternal)(GONIAdd);
			printf("\n\n[CARBON] Found GetObjectNameInternal! \n\n");
		}

		//Used for getting UObjects full names.
		if (gVersion < 16.00f)
		{
			if (gVersion < 14.30f)
			{
				//Tested only on 12.41 and 12.61.
				auto GetObjectFullNameAdd = Util::FindPattern(Patterns::Oldies::bGlobal::GetObjectFullName, Masks::Oldies::bGlobal::GetObjectFullName);
				VALIDATE_ADDRESS(GetObjectFullNameAdd, XOR("Failed to find GetObjectFullName Address."));

				GetObjectFullNameInternal = decltype(GetObjectFullNameInternal)(GetObjectFullNameAdd);
			}
			else
			{
				//14.30^
				auto GetObjectFullNameAdd = Util::FindPattern(Patterns::bGlobal::GetObjectFullName, Masks::bGlobal::GetObjectFullName);
				VALIDATE_ADDRESS(GetObjectFullNameAdd, XOR("Failed to find GetObjectFullName Address."));

				GetObjectFullNameInternal = decltype(GetObjectFullNameInternal)(GetObjectFullNameAdd);
			}
		}
		
		//Used for getting FFields full names, works from 12.41 - 18.40
		auto GetFullNameAdd = Util::FindPattern(Patterns::bGlobal::GetFullName, Masks::bGlobal::GetFullName);
		VALIDATE_ADDRESS(GetFullNameAdd, XOR("Failed to find GetFullName Address."));

		GetFullName = decltype(GetFullName)(GetFullNameAdd);
		printf("\n\n[CARBON] Found GetFullName! \n\n");

		//Used to free the memory for names.
		if (gVersion > 16.30f)
		{
			auto FreeInternalAdd = Util::FindPatternEasy(Patterns::bGlobal::FreeMemory);
			VALIDATE_ADDRESS(FreeInternalAdd, XOR("Failed to find Free Address."));

			FreeInternal = decltype(FreeInternal)(FreeInternalAdd);
			printf("\n\n[CARBON] Found FreeInternal! \n\n");
		}
		else {
			auto FreeInternalAdd = Util::FindPattern(Patterns::bGlobal::FreeInternal, Masks::bGlobal::FreeInternal);
			VALIDATE_ADDRESS(FreeInternalAdd, XOR("Failed to find Free Address."));

			FreeInternal = decltype(FreeInternal)(FreeInternalAdd);
			printf("\n\n[CARBON] Found FreeInternal! \n\n");
		}

		//Used to construct objects, mostly used for console stuff.
		//Tested from 12.41 to latest
		auto SCOIAdd = Util::FindPattern(Patterns::bGlobal::SCOI, Masks::bGlobal::SCOI);
		VALIDATE_ADDRESS(SCOIAdd, XOR("Failed to find SCOI Address."));

		StaticConstructObject = decltype(StaticConstructObject)(SCOIAdd);
		printf("\n\n[CARBON] Found StaticConstructObject! \n\n");

		//Used to load objects.
		//Tested from 12.41 to latest
		auto SLOIAdd = Util::FindPattern(Patterns::bGlobal::SLOI, Masks::bGlobal::SLOI);
		VALIDATE_ADDRESS(SLOIAdd, XOR("Failed to find SLOI Address."));

		StaticLoadObject = decltype(StaticLoadObject)(SLOIAdd);
		printf("\n\n[CARBON] Found StaticLoadObject! \n\n");

		
		//Process Event Hooking.
		MH_CreateHook(reinterpret_cast<void*>(ProcessEventAdd), ProcessEventDetour, reinterpret_cast<void**>(&ProcessEvent));
		MH_EnableHook(reinterpret_cast<void*>(ProcessEventAdd));

		/*
		//GetViewPoint Hooking.
		if(gVersion < 16.00f)
		{
			MH_CreateHook(reinterpret_cast<void*>(GetViewPointAdd), GetViewPointDetour, reinterpret_cast<void**>(&GetViewPoint));
			MH_EnableHook(reinterpret_cast<void*>(GetViewPointAdd));
		}
		*/
		return true;
		
	}
}
