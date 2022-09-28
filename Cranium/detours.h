#pragma once
#include "ue4.h"
#include "neoroyale.h"
#include "kismet.h"
#include "EventHelper.h"

#ifndef PROD
//#define LOGGING
#endif

using namespace NeoRoyale;

inline bool bIsDebugCamera;
inline bool bIsFlying;
inline bool bHasEventStarted;
inline bool bIsEventPlaylist;

inline void* ProcessEventDetour(UObject* pObj, UObject* pFunc, void* pParams)
{
	std::wstring nObj;
	std::wstring nFunc;
	if (gVersion >= 16.00f)
	{
		nObj = pObj->GetName();
		nFunc = pFunc->GetName();
	}
	else
	{
		nObj = GetObjectFirstName(pObj);
		nFunc = GetObjectFirstName(pFunc);
	}

	if (GetAsyncKeyState(VK_F1) & 0x1) {
		auto MAP = ARTEMIS_TERRAIN;
		if (gVersion < 19.0f) {
			MAP = APOLLO_TERRAIN;
		}
		else if (gVersion < 11.0f) {
			//TODO
		}
		Start(MAP);
		gPlaylist = FindObject<UObject*>(XOR(L"FortPlaylistAthena /Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab"));
		Sleep(1000);
	}

	//If the game requested matchmaking we open the game mode
	if (gUrl.find(XOR("matchmakingservice")) != std::string::npos)
	{
		printf(XOR("\n\n[NeoRoyale] Start!"));
		//TODO: clean this mess;
		std::string url = gUrl;
		gUrl.clear();
		std::string query = url.erase(0, url.find(XOR("%3A")) + 1);
		query.erase(url.find("&"), url.size());
		query.erase(0, url.find(XOR("playlist")));
		std::string PlaylistName = query + "." + query;
		const std::wstring PlaylistNameW(PlaylistName.begin(), PlaylistName.end());

		auto Playlist = FindObject<UObject*>(PlaylistNameW.c_str(), true, true);
		auto Map = ARTEMIS_TERRAIN;

		if (gVersion >= 19.00f) {
			Map = ARTEMIS_TERRAIN;
		}
		else
		{
			Map = APOLLO_TERRAIN;
		}
		if (PlaylistNameW.find(XOR(L"papaya")) != std::string::npos && !gPlaylist)
		{
			Map = APOLLO_PAPAYA;
		}

		if (PlaylistNameW.find(XOR(L"jerky")) != std::string::npos && !gPlaylist)
		{
			bIsEventPlaylist = true;
		}
		if (PlaylistNameW.find(XOR(L"fritter")) != std::string::npos && !gPlaylist)
		{
			bIsEventPlaylist = true;
		}
		if (PlaylistNameW.find(XOR(L"yogurt")) != std::string::npos && !gPlaylist)
		{
			Map = APOLLO_TERRAIN_YOGURT;
			bIsEventPlaylist = true;
		}
		if (PlaylistNameW.find(XOR(L"buffet")) != std::string::npos && !gPlaylist)
		{
			//Map = RIFT_TOUR_EVENT_MAP;
			bIsEventPlaylist = true;
		}
		if (PlaylistNameW.find(XOR(L"kiwi")) != std::string::npos && !gPlaylist)
		{
			Map = KIWI_PERSISTENT;
			bIsEventPlaylist = true;
		}
		if (PlaylistNameW.find(XOR(L"guava")) != std::string::npos && !gPlaylist)
		{
			Map = GUAVA_PERSISTENT;
			bIsEventPlaylist = true;
		}


		if (Playlist && !gPlaylist)
		{
			gPlaylist = Playlist;
		}
		else if (!Playlist && !gPlaylist)
		{
			MessageBoxW(nullptr, L"Cannot find playlist.\n Using battlelab instead", L"Error", MB_OK);
			printf("[CRANIUM]Playlist not found! Using battlelab instead. \n");
			gPlaylist = FindObject<UObject*>(XOR(L"FortPlaylistAthena /Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab"));
		}
		Start(Map);
	}

	if (wcsstr(nFunc.c_str(), XOR(L"ReadyToStartMatch")) && bIsStarted && !bIsInit)
	{
		printf(XOR("\n[NeoRoyale] Init!\n"));
		Init();
	}

	if (wcsstr(nFunc.c_str(), XOR(L"ServerLoadingScreenDropped")) && bIsInit && bIsStarted)
	{
		NeoPlayer.SetupAbilities();
		printf("[CARBON] Granted pre defined abilities.\n");
		//NeoPlayer.SetupInventory();
		printf("[CARBON] Set ownership.\n");
		NeoPlayer.ExecuteConsoleCommand(XOR(L"destroyall lodactor"));
		printf("[CARBON] Destroyed all lodactors.\n"); //<--- for Events above The Device (except Zero Crisis, Chapter 2 Finale) do not use LODActor and FortHLODSMActor. Those Events don't take place on the BR Map or the Player is very far away. This only cause mor lag - notkrae
		NeoPlayer.ExecuteConsoleCommand(XOR(L"destroyall volume"));
		printf("[CARBON] Destroyed all volumes.\n");
		NeoPlayer.ExecuteConsoleCommand(XOR(L"destroyall fortlodactor"));
		printf("[CARBON] Destroyed all fort lodactors.\n");
		NeoPlayer.ExecuteConsoleCommand(XOR(L"destroyall fortlodsmactor"));
		printf("[CARBON] Destroyed all fortlodsmactors.\n");
		NeoPlayer.ServerSetClientHasFinishedLoading();
		printf("[CARBON] Set client has finished loading.\n");
		if (gVersion > 17.90f)
			NeoPlayer.StartSkydiving(500.0f);
		
		if (gVersion > 16.50f) // Enables the Next-Gen Graphics that were added early on PS5 / Xbox Series X (later introduced in v17.00) - notkrae
		{
			NeoPlayer.ExecuteConsoleCommand(L"Athena.PostProcessLevel 3");
			NeoPlayer.ExecuteConsoleCommand(L"Athena.UseVolumetricClouds true");
			NeoPlayer.ExecuteConsoleCommand(L"Athena.UseJanusFX true");
			NeoPlayer.ExecuteConsoleCommand(L"Athena.UsePhysicalRimlight true");
			NeoPlayer.ExecuteConsoleCommand(L"Athena.UseFluidFX true");
		}

		if(bIsEventPlaylist)
			Events::LoadPreEvent();
	}
	
	if (gVersion == 14.60f) { // Junior aka Galactus Event - notkrae

		if (wcsstr(nFunc.c_str(), XOR(L"ReceiveBeginPlay")) && wcsstr(nObj.c_str(), XOR(L"BP_Junior_ZP")) && bHasEventStarted)
		{
			NeoPlayer.SetPawnGravityScale(100);
		}
		if (wcsstr(nFunc.c_str(), XOR(L"OnAnimationFinished")) && wcsstr(nObj.c_str(), XOR(L"GamePhaseAlert")) && bHasEventStarted) // Removes Junor Timer - notkrae
		{
			Events::JuniorCountdownManager(false);
		}
		if (wcsstr(nFunc.c_str(), XOR(L"Timeline_0__FinishedFunc")) && bHasEventStarted) //Reactivates Next-Gen Graphics which get unloaded for some reason after the Event starts. - notkrae
		{
			Events::JuniorCountdownManager(true); // Removes Event Timer if true. - notkrae
			NeoPlayer.ExecuteConsoleCommand(L"Athena.PostProcessLevel 3");
			NeoPlayer.ExecuteConsoleCommand(L"Athena.UseVolumetricClouds true");
			NeoPlayer.ExecuteConsoleCommand(L"Athena.UseJanusFX true");
			NeoPlayer.ExecuteConsoleCommand(L"Athena.UsePhysicalRimlight true");
			NeoPlayer.ExecuteConsoleCommand(L"Athena.UseFluidFX true");
		}
		//[CARBON] Current Coords: -2261.2 -241532 833498
		if (wcsstr(nFunc.c_str(), XOR(L"ReceiveEndPlay")) && wcsstr(nObj.c_str(), XOR(L"ZeroPointCore")) && bHasEventStarted)
		{
			NeoPlayer.Respawn();
			NeoPlayer.SetPawnGravityScale(1);
			NeoPlayer.ExecuteConsoleCommand(XOR(L"camera firstperson"));
			NeoPlayer.ExecuteConsoleCommand(L"bugitgo 0 1046930 539600 0 270");
			NeoPlayer.SetCameraMode(L"FirstPerson");
			//NeoPlayer.ExecuteConsoleCommand(XOR(L"camera freecam"));
			//NeoPlayer.Fly(true);
			NeoPlayer.HideHead(true);
		}

		if (wcsstr(nFunc.c_str(), XOR(L"Construct")) && wcsstr(nObj.c_str(), XOR(L"JuniorScoreCard_C")) && bHasEventStarted)
		{
			//NeoPlayer.ExecuteConsoleCommand(XOR(L"camera firstperson"));
			NeoPlayer.SetPawnGravityScale(0);
			NeoPlayer.ExecuteConsoleCommand(L"bugitgo -5400 -349600 855200 0 270");
			NeoPlayer.SetCameraMode(L"FirstPerson");
			//NeoPlayer.Fly(true);
			//NeoPlayer.ExecuteConsoleCommand(XOR(L"camera freecam"));
		}

		if (wcsstr(nFunc.c_str(), XOR(L"ReceiveIsFinished")) && wcsstr(nObj.c_str(), XOR(L"CameraShake_Junior_HeliCarrierHit_C")) && bHasEventStarted) // Unloads Helicarrier and Volumes (notkrae)
		{
			Events::JuniorCarrierManager(true);
			Events::JuniorCarrierManager(true);
			NeoPlayer.ExecuteConsoleCommand(L"destroyall Volume");
			NeoPlayer.ExecuteConsoleCommand(L"destroyall Levelbounds");
			NeoPlayer.SetPawnGravityScale(-0.000001); // This should be better than Fly. - notkrae
			//NeoPlayer.Fly(true);
		}

		if (wcsstr(nFunc.c_str(), XOR(L"StreamedVideoOnUrlFailure")) && wcsstr(nObj.c_str(), XOR(L"ActivatableMovieWidget_Monolithic_Native_C")) && bHasEventStarted)
		{
			//[CARBON] Current Coords : -16880.3 - 264071 790292
			NeoPlayer.ExecuteConsoleCommand(L"bugitgo 657.819 0 855200 0 270");
			//NeoPlayer.Fly(false);
		}
	}
	
	if (gVersion == 17.30f)
	{
		if (wcsstr(nFunc.c_str(), XOR(L"CameraFade")) && wcsstr(nObj.c_str(), XOR(L"GA_Buffet_Door_Pull_Real_C")) && bHasEventStarted)
		{
			//MessageBoxA(nullptr, XOR("Teleporting to Buffet_Part_3"), XOR("Carbon"), MB_OK);
			NeoPlayer.ExecuteConsoleCommand(XOR(L"bugitgo -5000 0 360000"));
		}

		if (wcsstr(nFunc.c_str(), XOR(L"OnCreated")) && wcsstr(nObj.c_str(), XOR(L"SequenceDirector_C")))
		{
			//MessageBoxA(nullptr, XOR("Event Start Detected - Setting demospeed to 1"), XOR("Carbon"), MB_OK);
			NeoPlayer.ExecuteConsoleCommand(XOR(L"demospeed 1"));
		}

		if (wcsstr(nFunc.c_str(), XOR(L"OnQuantizationEvent")) && wcsstr(nObj.c_str(), XOR(L"BP_BeatSync_Brain_2")) && bHasEventStarted)
		{
			//MessageBoxA(nullptr, XOR("Teleporting to Slide, fixing camera"), XOR("Carbon"), MB_OK);
			NeoPlayer.ExecuteConsoleCommand(XOR(L"camera freecam"));
			NeoPlayer.Possess();
			NeoPlayer.ExecuteConsoleCommand(XOR(L"bugitgo -15000 -200000 85000"));

		}
	}
	
	if (gVersion == 17.30f)
	{
		if (wcsstr(nFunc.c_str(), XOR(L"CameraFade")) && wcsstr(nObj.c_str(), XOR(L"GA_Buffet_Door_Pull_Real_C")) && bHasEventStarted)
		{
			//MessageBoxA(nullptr, XOR("Teleporting to Buffet_Part_3"), XOR("Carbon"), MB_OK);
			NeoPlayer.ExecuteConsoleCommand(XOR(L"bugitgo -5000 0 360000"));
		}

		if (wcsstr(nFunc.c_str(), XOR(L"OnCreated")) && wcsstr(nObj.c_str(), XOR(L"SequenceDirector_C")))
		{
			//MessageBoxA(nullptr, XOR("Event Start Detected - Setting demospeed to 1"), XOR("Carbon"), MB_OK);
			NeoPlayer.ExecuteConsoleCommand(XOR(L"demospeed 1"));
		}

		if (wcsstr(nFunc.c_str(), XOR(L"OnQuantizationEvent")) && wcsstr(nObj.c_str(), XOR(L"BP_BeatSync_Brain_2")) && bHasEventStarted)
		{
			//MessageBoxA(nullptr, XOR("Teleporting to Slide, fixing camera"), XOR("Carbon"), MB_OK);
			NeoPlayer.ExecuteConsoleCommand(XOR(L"camera freecam"));
			NeoPlayer.Possess();
			NeoPlayer.ExecuteConsoleCommand(XOR(L"bugitgo -15000 -200000 85000"));

		}
	}
	if (wcsstr(nFunc.c_str(), XOR(L"ServerExecuteInventoryItem")))
	{
		FGuid* guid = reinterpret_cast<FGuid*>(pParams);

		auto entries = reinterpret_cast<TArray<FFortItemEntry>*>(__int64(NeoPlayer.FortInventory) + __int64(ObjectFinder::FindOffset(L"FortInventory", L"Inventory")) + __int64(ObjectFinder::FindOffset(L"FortItemList", L"ReplicatedEntries")));

		for (int i = 0; i < entries->Num(); i++)
		{
			auto entry = entries->operator[](i);
			auto entryGuid = reinterpret_cast<FGuid*>((uintptr_t)&entry + __int64(ObjectFinder::FindOffset(L"FortItemEntry", L"ItemGuid")));

			if (NeoPlayer.IsMatchingGuid(*entryGuid, *guid))
			{
				struct
				{
					UObject* WeaponData;
					FGuid ItemEntryGuid;
					UObject* ReturnValue;
				} EquipWeaponDefinitionParams;

				EquipWeaponDefinitionParams.WeaponData = *reinterpret_cast<UObject**>((uintptr_t)&entry + __int64(ObjectFinder::FindOffset(L"FortItemEntry", L"ItemDefinition")));
				EquipWeaponDefinitionParams.ItemEntryGuid = *guid;

				UFunction* fn;
				if (gVersion > 16.00f)
					fn = FindObject<UFunction*>(L"Function /Script/FortniteGame.FortPawn.EquipWeaponDefinition");
				else
					fn = FindObject<UFunction*>(L"Function /Script/FortniteGame.FortPawn:EquipWeaponDefinition");

				ProcessEvent((UObject*)NeoPlayer.Pawn, fn, &EquipWeaponDefinitionParams);
				NeoPlayer.StopMontageIfEmote();
			}
		}
	}

	//Toggle our fly function on "fly" command.
	if (wcsstr(nFunc.c_str(), XOR(L"Fly")) && nObj.starts_with(XOR(L"CheatManager_")))
	{
		NeoPlayer.Fly(bIsFlying);
		bIsFlying = !bIsFlying;
	}


	// NOTE: (irma) This is better.
	if (wcsstr(nFunc.c_str(), XOR(L"ServerAttemptAircraftJump")))
	{
		NeoPlayer.ExecuteConsoleCommand(XOR(L"PAUSESAFEZONE"));
		NeoPlayer.Respawn();
		auto currentLocation = NeoPlayer.GetLocation();
		UFunctions::TeleportToCoords(currentLocation.X, currentLocation.Y, currentLocation.Z);
	}

	if (wcsstr(nFunc.c_str(), XOR(L"EnableCheats")))
	{
		Console::CheatManager();
	}

	if (wcsstr(nFunc.c_str(), XOR(L"OnWeaponEquipped")))
	{
		auto params = static_cast<AFortPawn_OnWeaponEquipped_Params*>(pParams);

		auto OldWeapon = params->PrevWeapon;

		if (OldWeapon && !Util::IsBadReadPtr(OldWeapon))
		{
			UFunctions::DestroyActor(OldWeapon);
			OldWeapon = nullptr;
		}
	}

	if (wcsstr(nFunc.c_str(), XOR(L"ServerReturnToMainMenu")))
	{
		auto CheatManager = reinterpret_cast<UObject**>((uintptr_t)NeoPlayer.Controller + __int64(ObjectFinder::FindOffset(L"PlayerController", L"CheatManager")));
		*CheatManager = nullptr;
		Sleep(500);
		UFunctions::Travel(L"Frontend?Game=/Script/FortniteGame.FortGameModeFrontEnd");
	}

	if (wcsstr(nFunc.c_str(), XOR(L"BP_OnDeactivated")) && wcsstr(nObj.c_str(), XOR(L"PickerOverlay_EmoteWheel")))
	{
		if (NeoPlayer.Pawn)
		{
			ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
			ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));

			ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

			ObjectFinder LastEmotePlayedFinder = PlayerControllerFinder.Find(XOR(L"LastEmotePlayed"));

			auto LastEmotePlayed = LastEmotePlayedFinder.GetObj();

			if (LastEmotePlayed && !Util::IsBadReadPtr(LastEmotePlayed))
			{
				NeoPlayer.Emote(LastEmotePlayed);
				for (auto i = 0; i < Bots.size(); i++)
				{
					auto Bot = Bots[i];
					if (Bot.Pawn)
					{
						Bot.Emote(LastEmotePlayed);
					}
				}
			}
		}
	}

	if (wcsstr(nFunc.c_str(), XOR(L"CheatScript")))
	{
		FString ScriptNameF = static_cast<UCheatManager_CheatScript_Params*>(pParams)->ScriptName;

		if (ScriptNameF.IsValid())
		{
			std::wstring ScriptNameW = ScriptNameF.ToWString();

			std::wstring arg;

			if (ScriptNameW.find(L" ") != std::wstring::npos)
			{
				arg = ScriptNameW.substr(ScriptNameW.find(L" ") + 1);
			}

			auto CMD = str2enum(ScriptNameW.c_str());

			switch (CMD)
			{
			case HELP:
			{
				UFunctions::ConsoleLog(CheatScriptHelp);
				break;
			}

			case TEST:
			{
				NeoPlayer.ExecuteConsoleCommand(XOR(L"streammap Buffet_Bubbles"));
				break;
			}

			case DUMP:
			{
				DumpGObjects();
				break;
			}

			case DUMPBPS:
			{
				DumpBPs();
				break;
			}
			case START_GAME:
			{
				auto Map = ARTEMIS_TERRAIN;
				if (gVersion > 19.00f) {
					Map = ARTEMIS_TERRAIN;
				}
				else
				{
					Map = APOLLO_TERRAIN;
				}
				if (!gPlaylist)
				{
					gPlaylist = FindObject<UObject*>(XOR(L"FortPlaylistAthena /Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab"));
				}
				Start(Map);
				break;
			}
			case EVENT:
			{
				if (Events::FindEventAndPlay())
					bHasEventStarted = true;
				else
					UFunctions::ConsoleLog(XOR(L"Sorry the version you are using doesn't have any event we support."));
				break;
			}

			case DEBUG_CAMERA:
			{
				bIsDebugCamera = !bIsDebugCamera;
				break;
			}

			case CURRENT_LOCATION:
			{
				auto currentLocation = NeoPlayer.GetLocation();
				//UFunctions::TeleportToCoords(currentLocation.X, currentLocation.Y, currentLocation.Z);
				std::cout << "[CARBON] Current Coords: " << currentLocation.X << " " << currentLocation.Y << " " << currentLocation.Z << std::endl;
				break;
			}

			case EQUIP:
			{
				if (gVersion < 18.00f)
				{
					if (!arg.empty())
					{
						if (arg.starts_with(XOR(L"WID_")) || arg.starts_with(XOR(L"AGID_")))
						{
							NeoPlayer.EquipWeapon(arg.c_str());
						}
						else
						{
							UFunctions::ConsoleLog(XOR(L"This command only works with WIDs and AGIDs."));
						}
					}
					else
					{
						UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
					}
				}
				else {
					if (!arg.empty())
					{
						if (arg.starts_with(XOR(L"WID_")) || arg.starts_with(XOR(L"AGID_")))
						{
							std::wstring name = arg + L"." + arg;
							auto WeaponData = FindObject<UObject*>(name.c_str(), true);

							NeoPlayer.AddItemToInventory(WeaponData, 1);
						}
						else
						{
							UFunctions::ConsoleLog(XOR(L"This command only works with WIDs and AGIDs."));
						}
					}
					else
					{
						UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
					}
				}
				if (!arg.empty())
				{
					std::wstring name = arg + L"." + arg;
					std::wcout << name;

					auto WeaponData = FindObject<UObject*>(name.c_str(), true);
					NeoPlayer.AddItemToInventory(WeaponData, 1);
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
				}
				break;
			}

			case SET_MAX_HEALTH:
			{
				if (!arg.empty())
				{
					auto n = std::stof(arg);
					NeoPlayer.SetMaxHealth(n);
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"This command requires an argument, e.g: (cheatscript setmaxhealth 1000)"));
				}
				break;
			}

			case SET_MAX_SHIELD:
			{
				if (!arg.empty())
				{
					auto n = std::stof(arg);
					NeoPlayer.SetMaxShield(n);
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"This command requires an argument e.g: (cheatscript setmaxshield 1000)"));
				}
				break;
			}

			case SET_HEALTH:
			{
				if (!arg.empty())
				{
					auto n = std::stof(arg);
					NeoPlayer.SetHealth(n);
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"This command requires an argument e.g: (cheatscript sethealth 1000)"));
				}
				break;
			}

			case SET_SHIELD:
			{
				if (!arg.empty())
				{
					auto n = std::stof(arg);
					NeoPlayer.SetShield(n);
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"This command requires an argument e.g: (cheatscript setshiled 1000)"));
				}
				break;
			}

			case SET_SPEED:
			{
				if (!arg.empty())
				{
					auto n = std::stof(arg);
					NeoPlayer.SetMovementSpeed(n);
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"This command requires an argument e.g: (cheatscript setspeed 1000)"));
				}
				break;
			}

			case SET_GRAVITY:
			{
				if (!arg.empty())
				{
					auto n = std::stof(arg);
					NeoPlayer.SetPawnGravityScale(n);
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
				}
				break;
			}

			case SET_PLAYLIST:
			{
				if (!arg.empty())
				{
					auto Playlist = FindObject<UObject*>(arg.c_str());
					if (Playlist)
					{
						gPlaylist = Playlist;
					}
					else
					{
						UFunctions::ConsoleLog(XOR(L"Couldn't find the requested playlist!."));
					}
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
				}
				break;
			}

			case SKYDIVE:
			{
				NeoPlayer.StartSkydiving(0);
				NeoPlayer.StartSkydiving(0);
				NeoPlayer.StartSkydiving(0);
				NeoPlayer.StartSkydiving(1500.0f);
				break;
			}

			case RESPAWN:
			{
				NeoPlayer.Respawn();
			}

			case LOADBPC:
			{
				if (!arg.empty())
				{
					const auto BPGClass = FindObject<UClass*>(XOR(L"Class /Script/Engine.BlueprintGeneratedClass"));

					UFunctions::StaticLoadObjectEasy(BPGClass, arg.c_str());
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
				}
			}

			default:
				break;
			}
		}
	}
	/*
	
ct]: Junior_VOFXController_2147457927 [Function]: SetDelayWetLevel
[Object]: Junior_VOFXController_2147457927 [Function]: SetDelayFeedback
[Object]: Junior_VOFXController_2147457927 [Function]: SetDelayTime
[Object]: Junior_VOFXController_2147457927 [Function]: SetReverbWet
[Object]: Junior_VOFXController_2147457927 [Function]: SetReverbDry
[Object]: Junior_VOFXController_2147457927 [Function]: SetReverbDecay
[Object]: Body [Function]: SetCastInsetShadow
[Object]: Face [Function]: SetCastInsetShadow
[Object]: head [Function]: SetCastInsetShadow
[Object]: BP_Junior_AudioScripting_2147457926 [Function]: SetShouldApplyEffects
[Object]: BP_Junior_AudioScripting_2147457926 [Function]: SetShouldDisableCompressor
[Object]: BP_Junior_AudioScripting_2147457926 [Function]: SetShouldApplyGameplayMix
*/
#ifdef LOGGING
	//Logging
	if (!wcsstr(nFunc.c_str(), L"EvaluateGraphExposedInputs") &&
		!wcsstr(nFunc.c_str(), L"Tick") &&
		!wcsstr(nFunc.c_str(), L"OnLanded") &&
		!wcsstr(nFunc.c_str(), L"HandleLanded") &&
		!wcsstr(nFunc.c_str(), L"K2_OnEndAbility") &&
		!wcsstr(nFunc.c_str(), L"SetDelayWetLevel") &&
		!wcsstr(nFunc.c_str(), L"SetDelayTime") &&
		!wcsstr(nFunc.c_str(), L"SetDelayFeedback") &&
		!wcsstr(nFunc.c_str(), L"SetReverbWet") &&
		!wcsstr(nFunc.c_str(), L"SetReverbDry") &&
		!wcsstr(nFunc.c_str(), L"SetReverbDecay") &&
		!wcsstr(nFunc.c_str(), L"SetCastInsetShadow") &&
		!wcsstr(nFunc.c_str(), L"SetShouldApplyEffects") &&
		!wcsstr(nFunc.c_str(), L"SetShouldDisableCompressor") &&
		!wcsstr(nFunc.c_str(), L"SetShouldApplyGameplayMix") &&
		!wcsstr(nFunc.c_str(), L"OnEndJumping") &&
		!wcsstr(nFunc.c_str(), L"OnBeginFalling") &&
		!wcsstr(nFunc.c_str(), L"CanJumpInternal") &&
		!wcsstr(nFunc.c_str(), L"SetSequenceTime") &&
		!wcsstr(nFunc.c_str(), L"SetFieldOfView") &&
		!wcsstr(nFunc.c_str(), L"SetBloomScale") &&
		!wcsstr(nFunc.c_str(), L"SetIntensity") &&
		!wcsstr(nFunc.c_str(), L"CheckShouldDisplayUI") &&
		!wcsstr(nFunc.c_str(), L"SetRuntimeStats") &&
		!wcsstr(nFunc.c_str(), L"SetInitialized") &&
		!wcsstr(nFunc.c_str(), L"SetDisableMasterCompressor") &&
		!wcsstr(nFunc.c_str(), L"BlueprintGetInteractionTime") &&
		!wcsstr(nFunc.c_str(), L"UpdateStateEvent") &&
		!wcsstr(nFunc.c_str(), L"Update") &&
		!wcsstr(nFunc.c_str(), L"BlueprintLinkedAnimationLayersInitialized") &&
		!wcsstr(nFunc.c_str(), L"BlueprintInitializeAnimation") &&
		!wcsstr(nFunc.c_str(), L"SetForcedLodModel") &&
		!wcsstr(nFunc.c_str(), L"SetVolumetricScatteringIntensity") &&
		!wcsstr(nFunc.c_str(), L"SetOuterConeAngle") &&
		!wcsstr(nFunc.c_str(), L"SetLightFalloffExponent") &&
		!wcsstr(nFunc.c_str(), L"SetAffectTranslucentLighting") &&
		!wcsstr(nFunc.c_str(), L"SetCastShadow") &&
		!wcsstr(nFunc.c_str(), L"OnParticleSystemFinished") &&
		!wcsstr(nFunc.c_str(), L"SetAnimationMode") &&
		!wcsstr(nFunc.c_str(), L"IsVisibleToPlayer") &&
		!wcsstr(nFunc.c_str(), L"ServerTouchActiveTime") &&
		!wcsstr(nFunc.c_str(), L"OnSubmixEnvelope") &&
		!wcsstr(nFunc.c_str(), L"OnSubmixSpectralAnalysis") &&
		!wcsstr(nFunc.c_str(), L"OnMouse") &&
		!wcsstr(nFunc.c_str(), L"Pulse") &&
		!wcsstr(nFunc.c_str(), L"BlueprintModifyPostProcess") &&
		!wcsstr(nFunc.c_str(), L"OnNiagaraSystemFinished") &&
		!wcsstr(nFunc.c_str(), L"OnVisibilitySetEvent") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_0") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_8") &&
		!wcsstr(nFunc.c_str(), L"SetFogInscatteringColor") &&
		!wcsstr(nFunc.c_str(), L"OnHUDElementVisibilityChanged") &&
		!wcsstr(nFunc.c_str(), L"SetCurrentFocalLength") &&
		!wcsstr(nFunc.c_str(), L"SetSoftSourceRadius") &&
		!wcsstr(nFunc.c_str(), L"SetAttenuationRadius") &&
		!wcsstr(nFunc.c_str(), L"OnExecute") &&
		!wcsstr(nFunc.c_str(), L"OnBurst") &&
		!wcsstr(nFunc.c_str(), L"HandleOnRequestChangeVis") &&
		!wcsstr(nFunc.c_str(), L"OnAnimationStarted") &&
		!wcsstr(nFunc.c_str(), L"HideLightColumn") &&
		!wcsstr(nFunc.c_str(), L"UserConstructionScript") &&
		!wcsstr(nFunc.c_str(), L"SetInnerConeAngle") &&
		!wcsstr(nFunc.c_str(), L"ClientMessage") &&
		!wcsstr(nFunc.c_str(), L"ClientTeamMessage") &&
		!wcsstr(nFunc.c_str(), L"Play Ambient Audio") &&
		!wcsstr(nFunc.c_str(), L"GameplayCue") &&
		!wcsstr(nFunc.c_str(), L"NetMulticast_InvokeGameplayCueExecuted_FromSpec") &&
		!wcsstr(nFunc.c_str(), L"TIMER VISUALS__UpdateFunc") &&
		!wcsstr(nFunc.c_str(), L"BlueprintUpdateAnimation") &&
		!wcsstr(nFunc.c_str(), L"BlueprintPostEvaluateAnimation") &&
		!wcsstr(nFunc.c_str(), L"BlueprintModifyCamera") &&
		!wcsstr(nFunc.c_str(), L"BlueprintModifyPostProcess") &&
		!wcsstr(nFunc.c_str(), L"Loop Animation Curve") &&
		!wcsstr(nFunc.c_str(), L"UpdateTime") &&
		!wcsstr(nFunc.c_str(), L"GetMutatorByClass") &&
		!wcsstr(nFunc.c_str(), L"UpdatePreviousPositionAndVelocity") &&
		!wcsstr(nFunc.c_str(), L"IsCachedIsProjectileWeapon") &&
		!wcsstr(nFunc.c_str(), L"LockOn") &&
		!wcsstr(nFunc.c_str(), L"GetAbilityTargetingLevel") &&
		!wcsstr(nFunc.c_str(), L"ServerFireAIDirectorEvent") &&
		!wcsstr(nFunc.c_str(), L"BP_UpdateMessaging") &&
		!wcsstr(nFunc.c_str(), L"BindVolumeEvents") &&
		!wcsstr(nFunc.c_str(), L"ReadyToEndMatch"))
	{
		printf(XOR("[Object]: %ws [Function]: %ws\n"), nObj.c_str(), nFunc.c_str());
	}
#endif

out:
	return ProcessEvent(pObj, pFunc, pParams);
}

namespace CameraHook
{
	inline float Speed = 0.1;
	inline float FOV = 52.0;
	inline FVector Camera(52.274170, 125912.695313, 89.249969);
	inline FRotator Rotation = { 0.870931, -88.071960, 0.008899 };
}

inline int GetViewPointDetour(void* pPlayer, FMinimalViewInfo* pViewInfo, BYTE stereoPass)
{
	const auto CurrentViewPoint = GetViewPoint(pPlayer, pViewInfo, stereoPass);

	if (bIsDebugCamera)
	{
		//fov increase and decrease
		if (GetAsyncKeyState(VK_OEM_PLUS) == 0) CameraHook::FOV += CameraHook::Speed;

		if (GetAsyncKeyState(VK_OEM_MINUS) == 0) CameraHook::FOV -= CameraHook::Speed;

		//froward and backward left and right
		if (GetAsyncKeyState(0x57) == 0) CameraHook::Camera.Y += CameraHook::Speed;

		if (GetAsyncKeyState(0x53) == 0) CameraHook::Camera.Y -= CameraHook::Speed;

		if (GetAsyncKeyState(0x41) == 0) CameraHook::Camera.X += CameraHook::Speed;

		if (GetAsyncKeyState(0x44) == 0) CameraHook::Camera.X -= CameraHook::Speed;

		//up and down
		if (GetAsyncKeyState(0x45) == 0) CameraHook::Camera.Z += CameraHook::Speed;

		if (GetAsyncKeyState(0x51) == 0) CameraHook::Camera.Z -= CameraHook::Speed;


		//looking around
		if (GetAsyncKeyState(VK_UP) == 0) CameraHook::Rotation.Pitch -= CameraHook::Speed;

		if (GetAsyncKeyState(VK_DOWN) == 0) CameraHook::Rotation.Pitch += CameraHook::Speed;

		if (GetAsyncKeyState(VK_LEFT) == 0) CameraHook::Rotation.Yaw += CameraHook::Speed;

		if (GetAsyncKeyState(VK_RIGHT) == 0) CameraHook::Rotation.Yaw -= CameraHook::Speed;

		//assign our hooked variables
		pViewInfo->Location.X = CameraHook::Camera.X;
		pViewInfo->Location.Y = CameraHook::Camera.Y;
		pViewInfo->Location.Z = CameraHook::Camera.Z;

		pViewInfo->Rotation.Pitch = CameraHook::Rotation.Pitch;
		pViewInfo->Rotation.Yaw = CameraHook::Rotation.Yaw;
		pViewInfo->Rotation.Roll = CameraHook::Rotation.Roll;

		pViewInfo->FOV = CameraHook::FOV;
	}

	return CurrentViewPoint;
}