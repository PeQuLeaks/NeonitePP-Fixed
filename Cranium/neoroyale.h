/**
 * Copyright (c) 2020-2021 Kareem Olim (Kemo)
 * All Rights Reserved. Licensed under the Neo License
 * https://neonite.dev/LICENSE.html
 */

#pragma once

#include "mods.h"
#include "server.h"
#include "enums.h"
#include "console.h"

inline std::vector<std::wstring> gWeapons;
inline std::vector<std::wstring> gBlueprints;
inline std::vector<std::wstring> gMeshes;
inline std::vector<Player> Bots;

namespace NeoRoyale
{
	inline bool bIsInit;
	inline bool bIsStarted;
	inline bool bIsPlayerInit;
	inline bool bisFlying = true;

	inline bool bHasJumped;
	inline bool bHasShowedPickaxe;

	inline bool bWantsToJump;
	inline bool bWantsToSkydive;
	inline bool bWantsToOpenGlider;
	inline bool bWantsToShowPickaxe;

	inline float bPlayerMovementSpeed = 1.1;

	inline Player NeoPlayer;

	inline void Start(const wchar_t* MapToPlayOn)
	{
		UFunctions::Travel(MapToPlayOn);
		bIsStarted = !bIsStarted;
	}

	inline void Stop()
	{
		UFunctions::Travel(FRONTEND);
		bIsStarted = false;
		bIsInit = false;
		NeoPlayer.Controller = nullptr;
		NeoPlayer.Pawn = nullptr;
		NeoPlayer.Mesh = nullptr;
		NeoPlayer.AnimInstance = nullptr;
	}

	inline void LoadMoreClasses()
	{
		auto BPGClass = UE4::FindObject<UClass*>(XOR(L"Class /Script/Engine.BlueprintGeneratedClass"));


		//Husks
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Characters/Enemies/Husk/Blueprints/HuskPawn.HuskPawn_C"));
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Characters/Enemies/DudeBro/Blueprints/DUDEBRO_Pawn.DUDEBRO_Pawn_C"));


		//CameraFilters
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_FilmNoir.PP_FilmNoir_C"));
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Crazy.PP_Crazy_C"));
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Dark.PP_Dark_C"));
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_HappyPlace.PP_HappyPlace_C"));
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Oak.PP_Oak_C"));
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Pixelizer.PP_Pixelizer_C"));
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Red.PP_Red_C"));
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Retro.PP_Retro_C"));
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Sepia.PP_Sepia_C"));
		UE4::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Spooky.PP_Spooky_C"));

	}
	inline void InitCombos()
	{
		for (auto i = 0x0; i < GObjs->NumElements; ++i)
		{
			auto object = GObjs->GetByIndex(i);
			if (object == nullptr)
			{
				continue;
			}

			if (!Util::IsBadReadPtr(object))
			{
				auto objectFullName = object->GetFullName();
				auto objectFirstName = object->GetName();

				if ((objectFullName.starts_with(L"AthenaGadget") || objectFirstName.starts_with(L"WID_")) && !objectFirstName.starts_with(L"Default__"))
				{
					gWeapons.push_back(objectFirstName);
				}
				else if (objectFirstName.ends_with(L"_C") && !objectFirstName.starts_with(L"Default__"))
				{
					gBlueprints.push_back(objectFirstName);
				}
				else if (objectFullName.starts_with(L"SkeletalMesh ") && !objectFirstName.starts_with(L"Default__"))
				{
					gMeshes.push_back(objectFirstName);
				}
			}
		}
	}

	inline void Thread()
	{
		//NOTE (kemo): i know this isn't the best practice but it does the job on another thread so it's not a frezzing call
		while (true) 
		{
			if (NeoPlayer.Pawn && (GetAsyncKeyState(VK_SPACE) & 1))
			{

				if (!NeoPlayer.IsInAircraft())
				{
					// Glide
					if (NeoPlayer.IsSkydiving() && !NeoPlayer.IsParachuteOpen() && !NeoPlayer.IsParachuteForcedOpen())
					{
						//NeoPlayer.SetMovementMode(EMovementMode::MOVE_Custom, 3);
						//NeoPlayer.OnRep_IsParachuteOpen(false);
					}
					// Skydive
					else if (NeoPlayer.IsSkydiving() && NeoPlayer.IsParachuteOpen() && !NeoPlayer.IsParachuteForcedOpen())
					{
						NeoPlayer.SetMovementMode(EMovementMode::MOVE_Custom, 3);
						NeoPlayer.SetMovementMode(EMovementMode::MOVE_Custom, 4);
						//NeoPlayer.OnRep_IsParachuteOpen(true);
					}
					//Jump
					else if (!NeoPlayer.IsJumpProvidingForce())
					{
						NeoPlayer.Jump();
					}
				}
			}
			if (NeoPlayer.Pawn && GetAsyncKeyState(0x31) /* 1 key */)
			{
				bHasShowedPickaxe = !bHasShowedPickaxe;
				NeoPlayer.StopMontageIfEmote();
				bWantsToShowPickaxe = true;
			}
			if (NeoPlayer.Pawn && GetAsyncKeyState(VK_F2))
			{
				NeoPlayer.StartSkydiving(22222);
				Sleep(100);
			}
			if (NeoPlayer.Pawn && GetAsyncKeyState(VK_F12)) //default debugging key, please dont bind this to anything important, ty -Timmy
			{
				NeoPlayer.EnableGod();
				printf("Pressed debug button\n");
			}
			if (NeoPlayer.Pawn && GetAsyncKeyState(VK_F6)) // Cleaned up fly toggling - Max 23/09/2021
			{
				NeoPlayer.debugtest();
				printf("Debug go brrr\n");
			}
			if (NeoPlayer.Pawn && GetAsyncKeyState(VK_OEM_PLUS)) // Speed modifications (Increase) - Max 23/09/2021
			{
				bPlayerMovementSpeed += 0.1;
				NeoPlayer.SetMovementSpeed(bPlayerMovementSpeed);
			}
			else if (NeoPlayer.Pawn && GetAsyncKeyState(VK_OEM_MINUS)) // Speed modifications (Decrease) - Max 23/09/2021
			{
				bPlayerMovementSpeed -= 0.1;
				NeoPlayer.SetMovementSpeed(bPlayerMovementSpeed);
			}
			if (NeoPlayer.Pawn && GetAsyncKeyState(VK_F3))
			{
				Stop();
				break;
			}

			if (bWantsToJump)
			{
				NeoPlayer.Jump();
				bWantsToJump = false;
			}

			else if (bWantsToOpenGlider)
			{
				NeoPlayer.ForceOpenParachute();
				bWantsToOpenGlider = false;
			}

			else if (bWantsToSkydive)
			{
				NeoPlayer.Skydive();
				bWantsToSkydive = false;
			}

			else if (bWantsToShowPickaxe)
			{
				NeoPlayer.ShowPickaxe();
				bWantsToShowPickaxe = false;
			}
			Sleep(1000 / 100);
		}
	}


	inline void Init()
	{
		UFunctions::DestroyAll(UE4::FindObject<UClass*>(XOR(L"Class /Script/FortniteGame.FortHLODSMActor")));
		printf("yee\n");
		//Console::ExecuteConsoleCommand(XOR(L"summon PlayerPawn_Athena_C"));
		//NeoPlayer.DummyPawn = UE4::SpawnActorEasy(UE4::FindObject<UClass*>(XOR(L"BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C")));
		NeoPlayer.Pawn = UE4::SpawnActorEasy(UE4::FindObject<UClass*>(XOR(L"BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C")));
		printf("yee3\n");
		NeoPlayer.Authorize();
		printf("yee4\n");
		if (NeoPlayer.Pawn)
		{
			NeoPlayer.Authorize();

			NeoPlayer.Possess();
			printf("yee5");
			//NeoPlayer.ToggleInfiniteAmmo();

			//NeoPlayer.SkinOverride = L"Test";

			//NeoPlayer.ApplyOverride();

			NeoPlayer.SetMovementSpeed(bPlayerMovementSpeed);

			UFunctions::SetPlaylist();

			NeoPlayer.Respawn();

			NeoPlayer.Respawn();

			UFunctions::SetGamePhase();

			NeoPlayer.ShowSkin();

			NeoPlayer.ShowPickaxe();

			UFunctions::StartMatch();

			UFunctions::ServerReadyToStartMatch();

			//InitCombos();

			//LoadMoreClasses();

			CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&Thread), nullptr, NULL, nullptr);

			NeoPlayer.EnableConsole();

			//NeoPlayer.EnableCheatManager();

			UFunctions::ConsoleLog(XOR(L"\n\nWelcome to Neonite++\nMade with ♥ By Kemo (@xkem0x on twitter)."));

			//NeoPlayer.EquipWeapon(L"WID_SMG_Recoil_Athena_SR");
			//ConnectServer();


			bIsInit = !bIsInit;
		}
	}

}
