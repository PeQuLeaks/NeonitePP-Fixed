#pragma once

#include "mods.h"
#include "server.h"

inline std::vector<std::wstring> gWeapons;
inline std::vector<std::wstring> gBlueprints;
inline std::vector<std::wstring> gMeshes;
inline std::vector<Player> Bots;

namespace NeoRoyale
{
	inline bool bIsInit;
	inline bool bIsStarted;
	inline bool bIsPlayerInit;

	inline bool bHasJumped;
	inline bool bHasShowedPickaxe;

	inline bool bWantsToJump;
	inline bool bWantsToSkydive;
	inline bool bWantsToOpenGlider;
	inline bool bWantsToShowPickaxe;

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
		Bots.clear();
		gPlaylist = nullptr;
		gNeoniteLogoTexture = nullptr;
	}

	inline void LoadMoreClasses()
	{
		const auto BPGClass = FindObject<UClass*>(XOR(L"Class /Script/Engine.BlueprintGeneratedClass"));

		//Mech
		UFunctions::StaticLoadObjectEasy(BPGClass,XOR(L"/Game/Athena/DrivableVehicles/Mech/TestMechVehicle.TestMechVehicle_C"));

		//Husks
		UFunctions::StaticLoadObjectEasy(BPGClass,XOR(L"/Game/Characters/Enemies/Husk/Blueprints/HuskPawn.HuskPawn_C"));
		UFunctions::StaticLoadObjectEasy(BPGClass,XOR(L"/Game/Characters/Enemies/DudeBro/Blueprints/DUDEBRO_Pawn.DUDEBRO_Pawn_C"));
		

		//CameraFilters
		UFunctions::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_FilmNoir.PP_FilmNoir_C"));
		UFunctions::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Crazy.PP_Crazy_C"));
		UFunctions::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Dark.PP_Dark_C"));
		UFunctions::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_HappyPlace.PP_HappyPlace_C"));
		UFunctions::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Oak.PP_Oak_C"));
		UFunctions::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Pixelizer.PP_Pixelizer_C"));
		UFunctions::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Red.PP_Red_C"));
		UFunctions::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Retro.PP_Retro_C"));
		UFunctions::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Sepia.PP_Sepia_C"));
		UFunctions::StaticLoadObjectEasy(BPGClass, XOR(L"/Game/Creative/PostProcess/PP_Spooky.PP_Spooky_C"));
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
				auto objectFullName = GetObjectFullName(object);
				auto objectFirstName = GetObjectFirstName(object);

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
			/*
			if (NeoPlayer.Pawn && GetAsyncKeyState(0x31) // 1 key )
			{
				if (!NeoPlayer.IsInAircraft())
				{
					if (!bHasShowedPickaxe)
					{
						bHasShowedPickaxe = !bHasShowedPickaxe;
						NeoPlayer.StopMontageIfEmote();
						bWantsToShowPickaxe = true;
					}
				}
			}
			else bHasShowedPickaxe = false;
			*/
			//	void OpenLevel(struct UObject* WorldContextObject, struct FName LevelName, bool bAbsolute, struct FString Options); // Function Engine.GameplayStatics.OpenLevel // (Final|Native|Static|Public|BlueprintCallable) // @ game+0x638115c

			if (NeoPlayer.Pawn && GetAsyncKeyState(VK_F6))
			{

				ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
				ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
				ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));
				
				auto Default__GameplayStatics = FindObject<UObject*>(L"GameplayStatics /Script/Engine.Default__GameplayStatics");
				auto fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameplayStatics.LoadStreamLevel"));
				
				FName map = KismetFunctions::GetFName(RIFT_TOUR_BUBBLES_MAP);

				FLatentActionInfo LatentInfo{};
				
				UGameplayStatics_LoadStreamLevel_Params params;
				params.WorldContextObject = WorldFinder.GetObj();
				params.LevelName = map;
				params.bMakeVisibleAfterLoad = true;
				params.bShouldBlockOnLoad = true;
				params.LatentInfo = LatentInfo;
				//MessageBoxA(nullptr, XOR("Called a special function!"), XOR("Carbon"), MB_OK);
				
				ProcessEvent(Default__GameplayStatics, fn, &params);
			}
			
			if (NeoPlayer.Pawn && GetAsyncKeyState(VK_F7))
			{

				ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
				ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
				ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));

				auto Default__GameplayStatics = FindObject<UObject*>(L"GameplayStatics /Script/Engine.Default__GameplayStatics");
				auto fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameplayStatics.LoadStreamLevel"));

				FName map = KismetFunctions::GetFName(TEST_STREAM);

				FLatentActionInfo LatentInfo{};

				UGameplayStatics_LoadStreamLevel_Params params;
				params.WorldContextObject = WorldFinder.GetObj();
				params.LevelName = map;
				params.bMakeVisibleAfterLoad = true;
				params.bShouldBlockOnLoad = true;
				params.LatentInfo = LatentInfo;
				//MessageBoxA(nullptr, XOR("Called a special function!"), XOR("Carbon"), MB_OK);

				ProcessEvent(Default__GameplayStatics, fn, &params);
			}
			if (NeoPlayer.Pawn && GetAsyncKeyState(VK_F3))
			{
				Stop();
				break;
			}

			Sleep(1000 / 30);
		}
	}

	/*inline void StartBuffet()
	{
		auto BP_Buffet_Master_Scripting_C = FindObject<UObject*>(XOR(L"BP_Buffet_Master_Scripting_C /Buffet/Levels/Buffet_P.Buffet_P.PersistentLevel.BP_Event_Master_Scripting_2"));
		auto startevent = FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.startevent"));

		ProcessEvent(BP_Buffet_Master_Scripting_C, startevent, nullptr);
	}*/

	inline void Init()
	{
		Console::CheatManager();

		UFunctions::DestroyAllHLODs();

		NeoPlayer.Summon(XOR(L"PlayerPawn_Athena_C"));

		NeoPlayer.Pawn = ObjectFinder::FindActor(XOR(L"PlayerPawn_Athena_C"));

		NeoPlayer.Authorize();

		if (NeoPlayer.Pawn)
		{
			NeoPlayer.Possess();

			NeoPlayer.ShowSkin();

			//NeoPlayer.ShowPickaxe();

			NeoPlayer.ToggleInfiniteAmmo();

			//NeoPlayer.ApplyOverride();

			//LOL
			NeoPlayer.ExecuteConsoleCommand(XOR(L"god"));
			NeoPlayer.SetMovementSpeed(1.1);

			if (gVersion > 17.10f)
			{
				NeoPlayer.Respawn();
				NeoPlayer.Respawn();
				NeoPlayer.Respawn();
			}

			UFunctions::SetPlaylist();

			UFunctions::SetGamePhase();
			
			
			if (gVersion == 12.41f)
			{
				UFunctions::LoadAndStreamInLevel(JERKY_EVENT_MAP);
			}
			else if (gVersion == 12.61f)
			{
				UFunctions::LoadAndStreamInLevel(DEVICE_EVENT_MAP);
			}
			else if (gVersion == 17.30f)
			{
				//UFunctions::LoadAndStreamInLevel(RIFT_TOUR_BUBBLES_MAP);
			}
			//InitCombos();

			UFunctions::StartMatch();

			UFunctions::ServerReadyToStartMatch();

			CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&Thread), nullptr, NULL, nullptr);

			UFunctions::ConsoleLog(XOR(L"\n\nWelcome to Carbon V1.4\nMade with ♥ By Kemo (@xkem0x), Timmy(@mawmet) and PeQu(ImPeQu)")); //feel free to add your credits when you do something

			//ConnectServer();

			bIsInit = !bIsInit;
		}
	}
}
