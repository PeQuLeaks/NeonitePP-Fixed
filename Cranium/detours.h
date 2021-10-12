/**
 * Copyright (c) 2020-2021 Kareem Olim (Kemo)
 * All Rights Reserved. Licensed under the Neo License
 * https://neonite.dev/LICENSE.html
 */

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

inline void* ProcessEventDetour(UObject* pObj, UFunction* pFunc, void* pParams)
{
	auto nObj = pObj->GetName();
	auto nFunc = pFunc->GetName();


	if (wcsstr(nFunc.c_str(), XOR(L"ReadyToStartMatch")) && bIsStarted && !bIsInit)
	{
		printf(XOR("\n[NeoRoyale] Init!\n"));
		Init();
	}

	if (wcsstr(nFunc.c_str(), XOR(L"DynamicHandleLoadingScreenVisibilityChanged")) && wcsstr(nObj.c_str(), XOR(L"AthenaLobby")))
	{

		UFunctions::ConsoleLog(XOR(L"Welcome to Neonite++"));
		if (bIsDebugCamera) bIsDebugCamera = !bIsDebugCamera;
		//UFunctions::RegionCheck();
	}

	if (wcsstr(nFunc.c_str(), XOR(L"ServerLoadingScreenDropped")) && bIsInit && bIsStarted)
	{

		auto buffetPawn = UE4::SpawnActorEasy(UE4::FindObject<UClass*>(XOR(L"Class /Script/BuffetRuntime.BuffetFlyingPawn")));
		//NeoPlayer.EnableGodMode();
		/*
		NeoPlayer.GrantAbility(UE4::FindObject<UObject*>(L"Class /Script/FortniteGame.FortGameplayAbility_Sprint"));
		NeoPlayer.GrantAbility(UE4::FindObject<UObject*>(L"Class /Script/FortniteGame.FortGameplayAbility_Jump"));
		NeoPlayer.GrantAbility(UE4::FindObject<UObject*>(L"BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractUse.GA_DefaultPlayer_InteractUse_C"));
		NeoPlayer.GrantAbility(UE4::FindObject<UObject*>(L"BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractSearch.GA_DefaultPlayer_InteractSearch_C"));
		*/
		UFunctions::SetupCustomInventory();

		UFunctions::PlayCustomPlayPhaseAlert();
		//LoadMoreClasses();
		
	}
	// NOTE: (irma) This is better.
	if (wcsstr(nFunc.c_str(), XOR(L"ServerAttemptAircraftJump")))
	{
		//NeoPlayer.GetLocation();
		//NeoPlayer.StartSkydiving(0);
		//NeoPlayer.StartSkydiving(0);
		//NeoPlayer.StartSkydiving(22222);
		//NeoPlayer.TeleportToSpawn();
		NeoPlayer.Respawn(); 
		NeoPlayer.TeleportTo(NeoPlayer.GetLocation());//NOTE TIMMY: Still isn't working properly, seems like function ServerAttemptAircraftJump isn't getting called properly and this causes a crash when trying to teleport
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
	if (wcsstr(nFunc.c_str(), XOR(L"CheatScript")))
	{
		FString ScriptNameF = static_cast<UCheatManager_CheatScript_Params*>(pParams)->ScriptName;
		if (ScriptNameF.IsValid())
		{
			std::wstring ScriptNameW = ScriptNameF.ToWString();
			if (wcsstr(ScriptNameW.c_str(), XOR(L"help")))
			{
				UFunctions::ConsoleLog(XOR(L"ehh yes there are no commands. just do 'cheatscipt WID_xxxxx' or something"));
			}
			else if (ScriptNameW.starts_with(XOR(L"FortWeapon")) || ScriptNameW.starts_with(XOR(L"AthenaGadget")) || ScriptNameW.starts_with(XOR(L"WID")))
			{
			//Neoroyale::PlayerPawn->EquipWeapon(ScriptNameW.c_str(), 0);
				NeoPlayer.EquipWeapon(ScriptNameW.c_str(), 0);
			}
		}
		UFunctions::ConsoleLog(XOR(L"Carbon moment"));
	}

	if (wcsstr(nFunc.c_str(), XOR(L"BP_OnDeactivated")) && wcsstr(nObj.c_str(), XOR(L"PickerOverlay_EmoteWheel")))
	{
		if (NeoPlayer.Pawn)
		{
			ObjectFinder PlayerControllerFinder = ObjectFinder::EntryPoint(uintptr_t(NeoPlayer.Controller));

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

		HitLocation.Z = HitLocation.Z + 200;

		Bot.Pawn = UE4::SpawnActorEasy(UE4::FindObject<UClass*>(XOR(L"BlueprintGeneratedClass /Game/Athena/AI/Phoebe/BP_PlayerPawn_Athena_Phoebe.BP_PlayerPawn_Athena_Phoebe_C")), HitLocation);

		if (Bot.Pawn)
		{
			Bot.SetSkeletalMesh(XOR(L"SK_M_MALE_Base"));
			Bot.Emote(UE4::FindObject<UObject*>(XOR(L"EID_HightowerSquash.EID_HightowerSquash"), true));

			Bots.push_back(Bot);
		}
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


	if (wcsstr(nFunc.c_str(), XOR(L"ExecuteConsoleCommand")))
	{
		FString ScriptNameF = static_cast<UKismetSystemLibrary_ExecuteConsoleCommand_Params*>(pParams)->Command;

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
				UFunctions::ConsoleLog(ScriptNameW);
				break;
			}

			case DUMP:
			{
				UE4::DumpGObjects();
				break;
			}

			case DUMPBPS:
			{
				UE4::DumpBPs();
				break;
			}
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
				else if (gVersion == 16.00f)
				{
					UFunctions::Play(YOUGURT_EVENT_PLAYER);
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

			case FLY:
			{
				NeoPlayer.Fly(bIsFlying);
				bIsFlying = !bIsFlying;
				break;
			}

			case EQUIP:
			{
				if (!arg.empty())
				{
					NeoPlayer.EquipWeapon(arg.c_str());
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
					auto Playlist = UE4::FindObject<UObject*>(ScriptNameW.c_str());
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
					const auto BPGClass = UE4::FindObject<UClass*>(XOR(L"Class /Script/Engine.BlueprintGeneratedClass"));

					UE4::StaticLoadObjectEasy(BPGClass, arg.c_str());
				}
				else
				{
					UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
				}
				break;
			}

			default: break;
			}
		
	}
#ifdef FUNCLOGGING
	//Logging
	if (!wcsstr(nFunc.c_str(), L"EvaluateGraphExposedInputs") &&
		!wcsstr(nFunc.c_str(), L"Tick") &&
		!wcsstr(nFunc.c_str(), L"OnSubmixEnvelope") &&
		!wcsstr(nFunc.c_str(), L"OnSubmixSpectralAnalysis") &&
		!wcsstr(nFunc.c_str(), L"OnMouse") &&
		!wcsstr(nFunc.c_str(), L"Pulse") &&
		!wcsstr(nFunc.c_str(), L"IsAcceptablePositionForPlacement") &&
		!wcsstr(nFunc.c_str(), L"OnContextualReticleChanged") &&
		!wcsstr(nFunc.c_str(), L"OnUpdateVisuals") &&
		!wcsstr(nFunc.c_str(), L"Light Flash Timeline__Loop__EventFunc") &&
		!wcsstr(nFunc.c_str(), L"Play Ambient Audio") &&
		!wcsstr(nFunc.c_str(), L"OnUpdateScale") &&
		!wcsstr(nFunc.c_str(), L"SetScalarParameterValueOnAllPreviewMIDs") &&
		!wcsstr(nFunc.c_str(), L"BlueprintUpdateAnimation") &&
		!wcsstr(nFunc.c_str(), L"BlueprintPostEvaluateAnimation") &&
		!wcsstr(nFunc.c_str(), L"BlueprintModifyCamera") &&
		!wcsstr(nFunc.c_str(), L"BlueprintModifyPostProcess") &&
		!wcsstr(nFunc.c_str(), L"Loop Animation Curve") &&
		!wcsstr(nFunc.c_str(), L"UpdateTime") &&
		!wcsstr(nFunc.c_str(), L"Timeline_UpdateFunc") &&
		!wcsstr(nFunc.c_str(), L"GetMutatorByClass") &&
		!wcsstr(nFunc.c_str(), L"UpdatePreviousPositionAndVelocity") &&
		!wcsstr(nFunc.c_str(), L"IsCachedIsProjectileWeapon") &&
		!wcsstr(nFunc.c_str(), L"LockOn") &&
		!wcsstr(nFunc.c_str(), L"HandleSimulatingComponentHit") &&
		!wcsstr(nFunc.c_str(), L"RecieveHit") &&
		!wcsstr(nFunc.c_str(), L"Light Flash Timeline__UpdateFunc") &&
		!wcsstr(nFunc.c_str(), L"ExecuteUbergraph_B_Athena_FlopperSpawnWorld_Placement") &&
		!wcsstr(nFunc.c_str(), L"GetAbilityTargetingLevel") &&
		!wcsstr(nFunc.c_str(), L"HandleSimulatingComponentHit") &&
		!wcsstr(nFunc.c_str(), L"BindVolumeEvents") &&
		//!wcsstr(nFunc.c_str(), L"BindVolumeEvents") &&
		!wcsstr(nFunc.c_str(), L"Update") &&
		!wcsstr(nFunc.c_str(), L"UpdateStateEvent") &&
		!wcsstr(nObj.c_str(), L"BGA_HeldObject_Physics_Explosive_PropaneTank_C_2147448779") &&
		!wcsstr(nFunc.c_str(), L"ReadyToEndMatch"))
	{
		printf(XOR("[Object]: %ws [Function]: %ws\n"), nObj.c_str(), nFunc.c_str());
	}
#endif


out: return ProcessEvent(pObj, pFunc, pParams);
}

namespace CameraHook
{
	inline float Speed = 0.1;
	inline float FOV = 52.0;
	inline FVector Camera(52.274170, 125912.695313, 89.249969);
	inline FRotator Rotation = { 0.870931, -88.071960, 0.008899 };
}

