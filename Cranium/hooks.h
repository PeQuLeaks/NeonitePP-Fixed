#pragma once
#include "pch.h"
#include "structs.h"
#include "enums.h"
#include "util.h"
#include "detours.h"
#include "ue4.h"
#include "veh.h"
#include "SSL.h"
#include "defs.h"

static void* CurlEasyAddress;
static void* CurlSetAddress;
static void* PushWidgetAddress;
//UE4
static void* RequestExitWithStatusAddressUE4;
static void* RequestExitAddressUE4;
static void* UnsafeEnvironmentPopupAddressUE4;
//UE5
static void* UnsafeEnvironmentPopupAddressUE5;
static void* RequestExitWithStatusAddressUE5;

static __int64 (*PushWidget)(__int64 WidgetInstance, const TCHAR* Body, const TCHAR* Widget, const TCHAR* WidgetType);


namespace Hooks
{
#ifdef SSL_BYPASS

	void RequestExitWithStatusHook(bool Force, uint8_t ReturnCode)
	{
		printf("[VEH] RequestExitWithStatusHook Call Forced: %i ReturnCode: %u\n", Force, ReturnCode);
	}

	__int64 PushWidgetHook(__int64 WidgetInstance, const TCHAR* Body, const TCHAR* Widget, const TCHAR* WidgetType)
	{
		const std::wstring bodyW(Body);
		if (bodyW == L"Logging In...")
		{
			return PushWidget(WidgetInstance,
				XOR(L"Ordering McDonalds for PeQu..."),
				Widget,
				WidgetType);
		}
		else if (bodyW == L"FILL")
		{
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourDetach(reinterpret_cast<void**>(&PushWidget), PushWidgetHook);
			DetourTransactionCommit();
		}
		return PushWidget(WidgetInstance, Body, Widget, WidgetType);
	}


	void UnsafeEnvironmentPopupHook(wchar_t** unknown1,
		unsigned __int8 _case,
		__int64 unknown2,
		char unknown3)
	{
		//printfc(FOREGROUND_BLUE, "[VEH] <REDACTED> Call with Case: %i\n", _case);
	}

	inline bool curl()
	{
		//cURL Hooking.
		//Works on all versions

	UnsafeEnvironmentPopupAddressUE4 = Util::FindPatternAV(Patterns::Curl::UnsafeEnvironmentPopup.first, Patterns::Curl::UnsafeEnvironmentPopup.second);
	if (!UnsafeEnvironmentPopupAddressUE4)
	{
		UnsafeEnvironmentPopupAddressUE5 = Util::FindPatternAV(Patterns::Curl::UnsafeEnvironmentPopup.first, Patterns::Curl::UnsafeEnvironmentPopup.second);
		//VALIDATE_ADDRESS(UnsafeEnvironmentPopupAddressUE5, "UnsafeEnvironmentPopup pattern is outdated.")
	}


	RequestExitWithStatusAddressUE4 = Util::FindPatternAV(Patterns::Curl::RequestExitWithStatus.first, Patterns::Curl::RequestExitWithStatus.second);
	if (!RequestExitWithStatusAddressUE4)
	{
		UnsafeEnvironmentPopupAddressUE5 = Util::FindPatternAV(Patterns::Curl::UnsafeEnvironmentPopup.first, Patterns::Curl::UnsafeEnvironmentPopup.second);
		//VALIDATE_ADDRESS(RequestExitWithStatusAddressUE5, "UnsafeEnvironmentPopup pattern is outdated.")
	}


		CurlEasyAddress = Util::FindPatternAV(Patterns::Curl::CurlEasySetOpt.first, Patterns::Curl::CurlEasySetOpt.second);
		VALIDATE_ADDRESS(CurlEasyAddress, "curl_easy_setopt pattern is outdated.")

		CurlSetAddress = Util::FindPatternAV(Patterns::Curl::CurlSetOpt.first, Patterns::Curl::CurlSetOpt.second);
		VALIDATE_ADDRESS(CurlSetAddress, "curl_setopt pattern is outdated.")

		PushWidgetAddress = Util::FindPatternAV(Patterns::Curl::PushWidget.first, Patterns::Curl::PushWidget.second);
		VALIDATE_ADDRESS(PushWidgetAddress, "Third pattern is outdated.")


		CurlEasySetOpt = decltype(CurlEasySetOpt)(CurlEasyAddress);

		CurlSetOpt = decltype(CurlSetOpt)(CurlSetAddress);

		PushWidget = decltype(PushWidget)(PushWidgetAddress);


		DetoursEasy(CurlEasySetOpt, CurlEasySetOptDetour)
		DetoursEasy(PushWidget, PushWidgetHook)
		DetoursEasy(RequestExitWithStatusAddressUE5, RequestExitWithStatusHook);
		DetoursEasy(UnsafeEnvironmentPopupAddressUE4, UnsafeEnvironmentPopupHook);
		DetoursEasy(RequestExitWithStatusAddressUE4, RequestExitWithStatusHook);
		

		return true;
	}

#endif
	inline bool Misc(float version)
	{

		if (MH_Initialize() != MH_OK)
		{
			MessageBoxA(nullptr, XOR("Failed to initialize min-hook, terminating the thread."), XOR("Cranium"), MB_OK);
			FreeLibraryAndExitThread(GetModuleHandle(nullptr), 0);
		}

		//GObject Array
		auto GObjectsAdd = Util::FindPattern(Patterns::bGlobal::GObjects, Masks::bGlobal::GObjects);
		VALIDATE_ADDRESS(GObjectsAdd, XOR("Failed to find GObjects Address."));

		GObjs = decltype(GObjs)(RELATIVE_ADDRESS(GObjectsAdd, 7));

		auto FNameToStringAdd = UE4::FindByString(FNAMETOSTRING_STRINGREF, { CALL }, true, 1);
		if (!FNameToStringAdd)
		{
			MessageBoxW(nullptr, L"Cannot find FNameToString.", L"Error", MB_OK);
			return false;
		}

		FNameToString = decltype(FNameToString)(FNameToStringAdd);
		/*const auto offset = *reinterpret_cast<int32_t*>(FNameToStringAdd + 1);
		FNameToStringAdd = FNameToStringAdd + 5 + offset;*/

		FNameToString = decltype(FNameToString)(FNameToStringAdd);

		//A work around instead of using a pattern.
		GEngine = UE4::FindObject<UEngine*>(XOR(L"FortEngine /Engine/Transient.FortEngine_"));


		uintptr_t ProcessEventAdd;

		if (gVersion > 19.00f) {

		}
		else if (gVersion < 19.00f) {
			if (version >= 16.00f)
			{
				const auto vtable = *reinterpret_cast<void***>(GEngine);
				ProcessEventAdd = (uintptr_t)vtable[0x44];

			}
			else
			{
				//ProcessEventAdd = Util::FindPattern(Patterns::bGlobal::ProcessEvent, Masks::bGlobal::ProcessEvent);
				//VALIDATE_ADDRESS(ProcessEventAdd, XOR("Failed to find ProcessEvent Address."));
			}
		}


		ProcessEvent = decltype(ProcessEvent)(ProcessEventAdd);
		MH_CreateHook(reinterpret_cast<void*>(ProcessEventAdd), ProcessEventDetour, reinterpret_cast<void**>(&ProcessEvent));
		MH_EnableHook(reinterpret_cast<void*>(ProcessEventAdd));


		//Used to construct objects, mostly used for console stuff.
		//Tested from 12.41 to latest
		auto SCOIAdd = Util::FindPattern(Patterns::bGlobal::SCOI, Masks::bGlobal::SCOI);
		VALIDATE_ADDRESS(SCOIAdd, XOR("Failed to find SCOI Address. Check Number 1"));

		StaticConstructObject = decltype(StaticConstructObject)(SCOIAdd);


		//Used to spawn actors
		auto SpawnActorAdd = Util::FindPattern(Patterns::bGlobal::SpawnActorInternal, Masks::bGlobal::SpawnActorInternal);
		VALIDATE_ADDRESS(SpawnActorAdd, XOR("Failed to find SpawnActor Address."));

		SpawnActor = decltype(SpawnActor)(SpawnActorAdd);

		return true;
	}
}
