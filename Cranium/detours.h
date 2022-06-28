#pragma once
#include "ue4.h"
#include "neoroyale.h"
#include "kismet.h"

#ifndef PROD
//#define LOGGING
#endif

using namespace NeoRoyale;

inline bool bIsDebugCamera;
inline bool bIsFlying;
inline bool bSpecialEvent1 = false; //make it so that you can't use the special event 1 more than once per match


inline void* ProcessEventDetour(UObject* pObj, UObject* pFunc, void* pParams)
{
	std::wstring nObj;
	std::wstring nFunc;
	if (gVersion > 19.30f)
	{
		nObj = pObj->GetName();;
		nFunc = pFunc->GetFullName();
	}
	else if (gVersion > 16.00f)
	{
		nObj = pObj->GetName();;
		nFunc = pFunc->GetName();
	}
	else
	{
		nObj = GetObjectFirstName(pObj);
		nFunc = GetObjectFirstName(pFunc);
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

		if (gVersion > 19.00f) {
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

		if (PlaylistNameW.find(XOR(L"yogurt")) != std::string::npos && !gPlaylist)
		{
			Map = APOLLO_TERRAIN_YOGURT;
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

	if (wcsstr(nFunc.c_str(), XOR(L"DynamicHandleLoadingScreenVisibilityChanged")) && wcsstr(nObj.c_str(), XOR(L"AthenaLobby")))
	{
		if (bIsDebugCamera) bIsDebugCamera = !bIsDebugCamera;
	}

	if (wcsstr(nFunc.c_str(), XOR(L"RecieveBeginPlay")) && wcsstr(nObj.c_str(), XOR(L"BP_Buffet_Memories_BigPlayer_C")) && !bSpecialEvent1)
	{
		MessageBoxA(nullptr, XOR("Called a special function!"), XOR("Carbon"), MB_OK);
	}

	if (wcsstr(nFunc.c_str(), XOR(L"ServerLoadingScreenDropped")) && bIsInit && bIsStarted)
	{
		NeoPlayer.SetupAbilities();
		NeoPlayer.SetupInventory();
		NeoPlayer.ExecuteConsoleCommand(XOR(L"destroyall lodactor"));
		NeoPlayer.ExecuteConsoleCommand(XOR(L"destroyall volume"));
		NeoPlayer.ExecuteConsoleCommand(XOR(L"destroyall fortlodactor"));
		NeoPlayer.ExecuteConsoleCommand(XOR(L"destroyall fortlodsmactor"));
		NeoPlayer.ServerSetClientHasFinishedLoading();
		if(gVersion > 17.90f)
			NeoPlayer.StartSkydiving(500.0f);
	}


	if (wcsstr(nFunc.c_str(), XOR(L"ExecuteUbergraph_Buffet_Part_3")))
	{
		MessageBoxA(nullptr, XOR("Teleporting player to Buffet_Part_3!"), XOR("Carbon"), MB_OK);
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
			UFunctions::DestoryActor(OldWeapon);
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
	
	if (wcsstr(nFunc.c_str(), XOR(L"ReceiveHit")) && nObj.starts_with(XOR(L"Prj_Athena_FrenchYedoc_JWFriendly_C")))
	{
		Player Bot;
		const auto Params = static_cast<AActor_ReceiveHit_Params*>(pParams);
		auto HitLocation = Params->HitLocation;

		NeoPlayer.Summon(XOR(L"BP_PlayerPawn_Athena_Phoebe_C"));
		Bot.Pawn = ObjectFinder::FindActor(XOR(L"BP_PlayerPawn_Athena_Phoebe_C"), Bots.size());

		if (Bot.Pawn)
		{
			HitLocation.Z = HitLocation.Z + 200;

			FRotator Rotation;
			Rotation.Yaw = 0;
			Rotation.Roll = 0;
			Rotation.Pitch = 0;

			Bot.TeleportTo(HitLocation, Rotation);

			Bot.SetSkeletalMesh(XOR(L"SK_M_MALE_Base"));
			Bot.Emote(FindObject<UObject*>(XOR(L"EID_HightowerSquash.EID_HightowerSquash"), true));

			Bots.push_back(Bot);
		}
	}

	if (wcsstr(nFunc.c_str(), XOR(L"BlueprintOnInteract")) && nObj.starts_with(XOR(L"BGA_FireExtinguisher_Pickup_C_")))
	{
		NeoPlayer.EquipWeapon(XOR(L"WID_FireExtinguisher_Spray"));
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
#ifndef PROD
			case ACTIVATE:
			{
				if (!arg.empty())
				{
					UFunctions::ConsoleLog(XOR(L"Couldn't process your activation code!."));

				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"Please input your activation key!."));
				}
				break;
			}
#endif
			case EVENT:
			{
				if (gVersion == 14.60f)
				{
					UFunctions::Play(GALACTUS_EVENT_PLAYER);
				}
				else if (gVersion == 12.41f)
				{
					UFunctions::Play(JERKY_EVENT_PLAYER);
				}
				else if (gVersion == 12.61f)
				{
					UFunctions::Play(DEVICE_EVENT_PLAYER);
				}
				else if (gVersion == 17.30f)
				{
					UFunctions::Play(RIFT_TOUR_EVENT_PLAYER);
				}
				else if (gVersion == 17.50f)
				{
					UFunctions::Play(KIWI_EVENT_PLAYER);
				}
				else if (gVersion == 18.40f)
				{
					UFunctions::Play(GUAVA_EVENT_PLAYER);
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"Sorry the version you are using doesn't have any event we support."));
				}
				break;
			}

			case DEBUG_CAMERA:
			{
				bIsDebugCamera = !bIsDebugCamera;
				break;
			}

			/*
				if (!arg.empty())
	{
		NeoPlayer.EquipWeapon(arg.c_str());
	}
	else
	{
		UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
	}
	break;*/
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
[Object]: Athena_PlayerController_C_2147472497 [Function]: ClientMessage
[Object]: Athena_PlayerController_C_2147472497 [Function]: ClientTeamMessage
[Object]: PlayerPawn_Athena_C_2147471858 [Function]: ClientCheatFly
[Object]: Fortnite_M_Avg_Player_AnimBlueprint_C_2147471850 [Function]: AnimNotify_CreateToolFlyModeStart_Entered
[Object]: M_MED_RustyRaider_01_Body_Skeleton_AnimBP_C_2147470044 [Function]: OnEndJumping
[Object]: M_MED_Cupid_Dark_Backpack_AnimBlueprint_C_2147470032 [Function]: OnEndJumping
[Object]: SequenceDirector_C_2147450605 [Function]: SequenceEvent__ENTRYPOINTSequenceDirector_14
[Object]: Athena_PlayerController_C_2147472497 [Function]: ClientPlayForceFeedback_Internal
[Object]: Fortnite_M_Avg_Player_AnimBlueprint_C_2147471850 [Function]: AnimNotify_CreateToolFlyModeLoop_Entered
[Object]: SequenceDirector_C_2147450605 [Function]: SequenceEvent__ENTRYPOINTSequenceDirector_15
[Object]: Athena_PlayerController_C_2147472497 [Function]: ClientPlayForceFeedback_Internal
[Object]: SequenceDirector_C_2147450605 [Function]: SequenceEvent__ENTRYPOINTSequenceDirector_16
[Object]: Athena_PlayerController_C_2147472497 [Function]: ClientPlayForceFeedback_Internal
	*/
#ifndef LOGGING
	//Logging
	if (!wcsstr(nFunc.c_str(), L"EvaluateGraphExposedInputs") &&
		!wcsstr(nFunc.c_str(), L"Tick") &&
		!wcsstr(nFunc.c_str(), L"SetCurrentFocalLength") &&
		!wcsstr(nFunc.c_str(), L"ClientMessage") &&
		!wcsstr(nFunc.c_str(), L"ClientPlayForceFeedback_Internal") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_1") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_2") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_3") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_4") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_5") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_6") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_7") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_8") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_9") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_10") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_11") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_12") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_13") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_14") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_15") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_16") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_17") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_18") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_19") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_20") &&
		!wcsstr(nFunc.c_str(), L"SequenceEvent__ENTRYPOINTSequenceDirector_21") &&
		!wcsstr(nFunc.c_str(), L"Received_Notify") &&
		!wcsstr(nFunc.c_str(), L"OnLanded") &&
		!wcsstr(nFunc.c_str(), L"HandleLanded") &&
		!wcsstr(nFunc.c_str(), L"K2_OnEndAbility") &&
		!wcsstr(nFunc.c_str(), L"OnEndJumping") &&
		!wcsstr(nFunc.c_str(), L"OnBeginFalling") &&
		!wcsstr(nFunc.c_str(), L"CanJumpInternal") &&
		!wcsstr(nFunc.c_str(), L"SetSequenceTime") &&
		!wcsstr(nFunc.c_str(), L"SetFieldOfView") &&
		!wcsstr(nFunc.c_str(), L"SetBloomScale") &&
		!wcsstr(nFunc.c_str(), L"SetIntensity") &&
		!wcsstr(nFunc.c_str(), L"SetInitialized") &&
		!wcsstr(nFunc.c_str(), L"SetDisableMasterCompressor") &&
		!wcsstr(nFunc.c_str(), L"BlueprintGetInteractionTime") &&
		!wcsstr(nFunc.c_str(), L"UpdateStateEvent") &&
		!wcsstr(nFunc.c_str(), L"Update") &&
		!wcsstr(nFunc.c_str(), L"ServerTouchActiveTime") &&
		!wcsstr(nFunc.c_str(), L"OnSubmixEnvelope") &&
		!wcsstr(nFunc.c_str(), L"OnSubmixSpectralAnalysis") &&
		!wcsstr(nFunc.c_str(), L"OnMouse") &&
		!wcsstr(nFunc.c_str(), L"Pulse") &&
		!wcsstr(nFunc.c_str(), L"BlueprintModifyPostProcess") &&
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
	inline FRotator Rotation = {0.870931, -88.071960, 0.008899};
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
