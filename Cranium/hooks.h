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
static void* RequestExitWithStatusAddress;
static void* RequestExitAddress;
static void* UnsafeEnvironmentPopupAddress;
namespace Hooks
{
#ifdef PLATANIUMV2
	void RequestExitWithStatusHook(bool unknown, bool force)
	{
		//printfc(FOREGROUND_BLUE, "[VEH] <REDACTED> Call IsForced: %i\n", force);
	}

	void RequestExitHook(bool force)
	{
		printf("[VEH] RequestExit Call IsForced: %i\n");
	}

	void UnsafeEnvironmentPopupHook(wchar_t** unknown1,
		unsigned __int8 _case,
		__int64 unknown2,
		char unknown3)
	{
		//printfc(FOREGROUND_BLUE, "[VEH] <REDACTED> Call with Case: %i\n", _case);
	}

	/*bool HotfixIniFileHook(void* HotfixManager, const FString& FileName, const FString& IniData)
	{
		HotfixManagerInstance = HotfixManager;
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(reinterpret_cast<void**>(&HotfixIniFile), HotfixIniFileHook);
		DetourTransactionCommit();
		return HotfixIniFile(HotfixManager, FileName, IniData);
	}*/

	void VerifyPeerPatch()
	{
		auto* const VerifyPeerAdd = Util::FindPattern("\x41\x39\x28\x0F\x95\xC0\x88\x83\x50\x04\x00\x00",
			"xxxxxxxxxx??");
		auto* const bytes = (uint8_t*)VerifyPeerAdd;
		bytes[4] = 0x94; //SETE
		printf("[DLL] VerifyPeer was changed!.\n");
	}
	inline CURLcode CurlEasySetOptHook(struct Curl_easy* data, CURLoption tag, ...)
	{
		va_list arg;
		va_start(arg, tag);

		CURLcode result = {};

		if (!data)
			return CURLE_BAD_FUNCTION_ARGUMENT;


		if (tag == CURLOPT_SSL_VERIFYPEER)
		{
			result = CurlSetOpt_(data, tag, 0);
		}

		if (tag == CURLOPT_PROXY)
		{
			result = CurlSetOpt_(data, tag, "");
		}

		else if (tag == CURLOPT_URL)
		{
			std::string url = va_arg(arg, char*);

			Uri uri = Uri::Parse(url);

			if (uri.Host.ends_with("ol.epicgames.com")
				|| uri.Host.ends_with(".akamaized.net")
				|| uri.Host.ends_with("on.epicgames.com"))
			{
				//printf("LogURL: %s\n", url.c_str());
				if (strcmp(URL_HOST, XOR("localhost")) == 0)
				{
					url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
				}
			}


			result = CurlSetOpt_(data, tag, url.c_str());
		}

		else
		{
			result = CurlSetOpt(data, tag, arg);
		}

		va_end(arg);
		return result;
	}
	inline bool Init()
	{
		//SSL BYPASS GO BRRRRRR -Timmy
		UnsafeEnvironmentPopupAddress = Util::FindPattern(PlatPatterns::UnsafeEnvironmentPopup.first,
			PlatPatterns::UnsafeEnvironmentPopup.second);
		VALIDATE_ADDRESS(UnsafeEnvironmentPopupAddress, "First pattern is outdated.")

			RequestExitWithStatusAddress = Util::FindPattern(PlatPatterns::RequestExitWithStatus.first,
				PlatPatterns::RequestExitWithStatus.second);
		VALIDATE_ADDRESS(RequestExitWithStatusAddress, "Second pattern is outdated.")

			RequestExitAddress = Util::FindPattern(
				"\x40\x53\x48\x83\xEC\x30\x41\xB9\x00\x00\x00\x00\x0F\xB6\xD9\x44\x38\x0D\x00\x00\x00\x00\x72\x20\x48\x8D\x05\x00\x00\x00\x00\x89\x5C\x24\x28\x4C\x8D\x05\x00\x00\x00\x00\x48\x89\x44\x24\x00\x33\xD2\x33\xC9\xE8\x00\x00\x00\x00",
				"xxxxxxxx????xxxxxx????xxxxx????xxxxxxx????xxxx?xxxxx????");
		VALIDATE_ADDRESS(RequestExitAddress, "SecondV2 pattern is outdated.")


			CurlEasyAddress = Util::FindPattern(PlatPatterns::CurlEasySetOpt.first, PlatPatterns::CurlEasySetOpt.second);
		VALIDATE_ADDRESS(CurlEasyAddress, "Curl easy pattern is outdated.")

			CurlSetAddress = Util::FindPattern(PlatPatterns::CurlSetOpt.first, PlatPatterns::CurlSetOpt.second);
		VALIDATE_ADDRESS(CurlSetAddress, "Curl set pattern is outdated.")

		CurlEasySetOpt = decltype(CurlEasySetOpt)(CurlEasyAddress);

		CurlSetOpt = decltype(CurlSetOpt)(CurlSetAddress);

		DetoursEasy(UnsafeEnvironmentPopupAddress, UnsafeEnvironmentPopupHook);
		DetoursEasy(RequestExitWithStatusAddress, RequestExitWithStatusHook);
		DetoursEasy(RequestExitAddress, RequestExitHook);

		DetoursEasy(CurlEasySetOpt, CurlEasySetOptHook);

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

		auto FNameToStringAdd = Util::FindPattern(Patterns::New::FNameToString, Masks::New::FNameToString);
		VALIDATE_ADDRESS(FNameToStringAdd, XOR("Failed to find FNameToString Address."));

		/*const auto offset = *reinterpret_cast<int32_t*>(FNameToStringAdd + 1);
		FNameToStringAdd = FNameToStringAdd + 5 + offset;*/

		FNameToString = decltype(FNameToString)(FNameToStringAdd);

		//A work around instead of using a pattern.
		GEngine = UE4::FindObject<UEngine*>(XOR(L"FortEngine /Engine/Transient.FortEngine_"));


		uintptr_t ProcessEventAdd;
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



		auto Map = GUAVA_P;
#ifdef RiftTourGUI
		//gPlaylist = UE4::FindObject<UObject*>(XOR(L"FortPlaylistAthena /BuffetPlaylist/Playlist/Playlist_Buffet.Playlist_Buffet")); //Buffet Event
		//gPlaylist = UE4::FindObject<UObject*>(XOR(L"FortPlaylistAthena /GuavaPlaylist/Playlist/Playlist_Guava.Playlist_Guava")); //Guava Event
		//gPlaylist = UE4::FindObject<UObject*>(XOR(L"FortPlaylistAthena /KiwiPlaylist/Playlists/Playlist_Kiwi.Playlist_Kiwi")); // Kiwi Event
		gPlaylist = UE4::FindObject<UObject*>(XOR(L"FortPlaylistAthena /GuavaPlaylist/Playlist/Playlist_Guava.Playlist_Guava")); //Battlelab
#else
	gPlaylist = UE4::FindObject<UObject*>(XOR(L"FortPlaylistAthena /Game/Athena/Playlists/Mash/Playlist_Mash_Squads_Legacy.Playlist_Mash_Squads_Legacy")); //Battlelab
#endif
		//gPlaylist = UE4::FindObject<UObject*>(XOR(L"FortPlaylistAthena /Game/Athena/Playlists/ItemTest/Playlist_ItemTest.Playlist_ItemTest"));
		//gPlaylist = UE4::FindObject<UObject*>(XOR(L"FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo")); //Solos
		Start(Map);

		return true;
	}
}
