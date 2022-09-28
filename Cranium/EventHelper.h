#pragma once
#include "mods.h"
#include "server.h"
#include "pawn.h"
#include "detours.h"

using namespace NeoRoyale;

namespace Events
{
	//General Event/level functions
	inline void LoadStreamLevel(const wchar_t* FNLoadLevel)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));
		ObjectFinder NetworkManagerFinder = WorldFinder.Find(XOR(L"NetworkManager"));
		ObjectFinder PersistentLevelFinder = WorldFinder.Find(XOR(L"PersistentLevel"));

		/*Loads Levels that are in unloaded stage. Can be seen by typing the "stat levels" command.
		Please not for Chapter 2 and lower, you can notice the unloaded stage by the Levels that are marked as red! (Should also work for the ones that are marked yellow!)
		For more info about the meanings of the colors, have a look here! https://docs.unrealengine.com/5.0/en-US/stat-commands-in-unreal-engine /*/
		UFunction* LoadStreamLevelFunc;

		if (gVersion > 16.00f)
		{
			LoadStreamLevelFunc = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameplayStatics.LoadStreamLevel"));
		}
		else {
			LoadStreamLevelFunc = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameplayStatics:LoadStreamLevel"));
		}
		auto LevelStreamingDynamic = FindObject<UObject*>(XOR(L"GameplayStatics /Script/Engine.Default__GameplayStatics"));

		FName LevelName = KismetFunctions::GetFName(FNLoadLevel);

		FLatentActionInfo LatentInfo{};

		UGameplayStatics_LoadStreamLevel_Params LoadStreamLevelParams;
		LoadStreamLevelParams.WorldContextObject = WorldFinder.GetObj();
		LoadStreamLevelParams.LevelName = LevelName;
		LoadStreamLevelParams.bShouldBlockOnLoad = true;
		LoadStreamLevelParams.bMakeVisibleAfterLoad = true;
		LoadStreamLevelParams.LatentInfo = LatentInfo;

		ProcessEvent(LevelStreamingDynamic, LoadStreamLevelFunc, &LoadStreamLevelParams);
	}

	inline void UnloadStreamLevel(const wchar_t* FNUnloadLevel)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));
		ObjectFinder NetworkManagerFinder = WorldFinder.Find(XOR(L"NetworkManager"));
		ObjectFinder PersistentLevelFinder = WorldFinder.Find(XOR(L"PersistentLevel"));

		//Unloads active Levels - notkrae
		UFunction* UnloadStreamLevelFunc;

		if (gVersion > 16.00f)
		{
			UnloadStreamLevelFunc = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameplayStatics.UnloadStreamLevel"));
		}
		else {
			UnloadStreamLevelFunc = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameplayStatics:UnloadStreamLevel"));
		}
		auto LevelStreamingDynamic = FindObject<UObject*>(XOR(L"GameplayStatics /Script/Engine.Default__GameplayStatics"));

		FName LevelName = KismetFunctions::GetFName(FNUnloadLevel);

		FLatentActionInfo LatentInfo{};

		UGameplayStatics_UnloadStreamLevel_Params UnloadStreamLevelParams;
		UnloadStreamLevelParams.WorldContextObject = WorldFinder.GetObj();
		UnloadStreamLevelParams.LevelName = LevelName;
		UnloadStreamLevelParams.bShouldBlockOnUnload = false;
		UnloadStreamLevelParams.LatentInfo = LatentInfo;

		ProcessEvent(LevelStreamingDynamic, UnloadStreamLevelFunc, &UnloadStreamLevelParams);
	}

	inline void LoadAndStreamInLevel(const wchar_t* FNLevel, FVector Loc, FRotator Rot)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));
		ObjectFinder NetworkManagerFinder = WorldFinder.Find(XOR(L"NetworkManager"));
		ObjectFinder PersistentLevelFinder = WorldFinder.Find(XOR(L"PersistentLevel"));

		//Loading the level instance in memory
		UFunction* LoadLevelInstance;
		if (gVersion > 16.00f)
		{
			LoadLevelInstance = FindObject<UFunction*>(XOR(L"Function /Script/Engine.LevelStreamingDynamic.LoadLevelInstance"));
		}
		else {
			LoadLevelInstance = FindObject<UFunction*>(XOR(L"Function /Script/Engine.LevelStreamingDynamic:LoadLevelInstance"));
		}
		auto LevelStreamingDynamic = FindObject<UObject*>(XOR(L"LevelStreamingDynamic /Script/Engine.Default__LevelStreamingDynamic"));

		FRotator WorldRotation;
		WorldRotation.Yaw = 0;
		WorldRotation.Roll = 0;
		WorldRotation.Pitch = 0;

		ULevelStreamingDynamic_LoadLevelInstance_Params LoadLevelInstanceParams;
		LoadLevelInstanceParams.WorldContextObject = WorldFinder.GetObj();
		LoadLevelInstanceParams.LevelName = FNLevel;
		LoadLevelInstanceParams.Location = Loc;
		LoadLevelInstanceParams.Rotation = Rot;

		ProcessEvent(LevelStreamingDynamic, LoadLevelInstance, &LoadLevelInstanceParams);

		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));
		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

		auto KismetSysLib = FindObject<UObject*>(XOR(L"KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary"));
		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand"));
		}
		else {
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.KismetSystemLibrary:ExecuteConsoleCommand"));

		}

		std::wstring command = L"streamlevelin " + std::wstring(FNLevel);

		UKismetSystemLibrary_ExecuteConsoleCommand_Params params;
		params.WorldContextObject = WorldFinder.GetObj();
		params.Command = command.c_str();
		params.SpecificPlayer = PlayerControllerFinder.GetObj();

		ProcessEvent(KismetSysLib, fn, &params);
	}

	inline void Play(const wchar_t* AnimationPlayerFullName)
	{
		UFunction* Play;
		if (gVersion > 16.00f)
			Play = FindObject<UFunction*>(XOR(L"Function /Script/MovieScene.MovieSceneSequencePlayer.Play"));
		else
			Play = FindObject<UFunction*>(XOR(L"Function /Script/MovieScene.MovieSceneSequencePlayer:Play"));

		auto Sequence = FindObject<UObject*>(AnimationPlayerFullName);


		ProcessEvent(Sequence, Play, nullptr);
	}

	//Functions for Junior Event
	void JuniorPreShowManager(bool IncludeCarrierLoader) // Since for some reason the Playlist doesn't contain Countdown, Lobby Event Carrier Loader, Settings etc. I made this small Manager who does all that stuff ^^ - notkrae
	{
		if (IncludeCarrierLoader == true)
		{
			LoadAndStreamInLevel(JUNIOR_HELICARRIER, FVector{ 1535, 61840, 40814 }, FRotator{ 0, 270, 0 }); // Event Helicarrier which isn't visible for some reason (someone please fix this if possible!). - notkrae
			LoadAndStreamInLevel(S14_APOLLO_HELICARRIER, FVector{ 1535, 61840, 40814 }, FRotator{ 0, 270, 0 }); // Helicarrier Lobby which is the one being used in normal matches in S14. Current replacement for the Event Helicarrier. - notkrae
			LoadAndStreamInLevel(JUNIOR_HELICARRIER_OVERLAY, FVector{ 1535, 61840, 40814 }, FRotator{ 0, 270, 0 }); // Event Helicarrier Overlay, which contains the Block/Skydiving Volume (the Skydiving Volume doesn't push up the player. Maybe fixable?) - notkrae
		}
		else
			UFunctions::SetTimeOfDaySpeed(0.0);
		UFunctions::SetTimeOfDay(20.5);

		auto JuniorGameCountdown = FindObject<UObject*>(XOR(L"BP_JCountdown_InGame_C /Game/Athena/Apollo/Maps/Test/S14/Apollo_POI_Hightower_HC_LobbyEvent_LevelInstance_1.Apollo_POI_Hightower_HC_LobbyEvent:PersistentLevel.BP_JCountdown_InGame_2"));
		auto GenericTimeManager = FindObject<UObject*>(XOR(L"BP_Generic_TimerManager_C /Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations:PersistentLevel.BP_Generic_TimerManager_2"));

		if (JuniorGameCountdown)
		{
			auto Stage2 = FindObject<UFunction*>(XOR(L"Function /Game/Athena/Events/Junior/BP_JCountdown_InGame.BP_JCountdown_InGame_C:Stage2"));
			Sleep(100); // Countdown has to be reloaded after calling the Function after LS drops or else it won't be visible. - notkrae
			auto Stage2Reload1 = FindObject<UFunction*>(XOR(L"Function /Game/Athena/Events/Junior/BP_JCountdown_InGame.BP_JCountdown_InGame_C:Stage2"));

			ProcessEvent(JuniorGameCountdown, Stage2, nullptr);
			ProcessEvent(JuniorGameCountdown, Stage2Reload1, nullptr);
		}
		if (GenericTimeManager)
		{
			auto CountdownTimer_New = FindObject<UFunction*>(XOR(L"Function /Game/Athena/Events/Generic/BP_Generic_TimerManager.BP_Generic_TimerManager_C:CountdownTimer_New"));
			auto VisualUpdateEverySecond = FindObject<UFunction*>(XOR(L"Function /Game/Athena/Events/Generic/BP_Generic_TimerManager.BP_Generic_TimerManager_C:VisualUpdateEverySecond"));
			auto ReceiveBeginPlay = FindObject<UFunction*>(XOR(L"Function /Game/Athena/Events/Generic/BP_Generic_TimerManager.BP_Generic_TimerManager_C:ReceiveBeginPlay"));

			ProcessEvent(GenericTimeManager, CountdownTimer_New, nullptr);
			ProcessEvent(GenericTimeManager, VisualUpdateEverySecond, nullptr);
			ProcessEvent(GenericTimeManager, ReceiveBeginPlay, nullptr);
		}
	}

	void JuniorCountdownManager(bool ShouldDestroyCountdown)
	{
		UFunctions::SetTimeOfDaySpeed(0.0);
		bool DoDestroyCountdown = true;
		auto JuniorGameCountdown = FindObject<UObject*>(XOR(L"BP_JCountdown_InGame_C /Game/Athena/Apollo/Maps/Test/S14/Apollo_POI_Hightower_HC_LobbyEvent_LevelInstance_1.Apollo_POI_Hightower_HC_LobbyEvent:PersistentLevel.BP_JCountdown_InGame_2"));
		auto JuniorScripting = FindObject<UObject*>(XOR(L"BP_Junior_Scripting_C /Junior/Levels/Junior_Map.Junior_Map:PersistentLevel.BP_Junior_Scripting_Child_2"));

		auto Stage2 = FindObject<UFunction*>(XOR(L"Function /Game/Athena/Events/Junior/BP_JCountdown_InGame.BP_JCountdown_InGame_C:Stage2"));

		ProcessEvent(JuniorGameCountdown, Stage2, nullptr);

		if (ShouldDestroyCountdown == true)
		{
			auto DestroyEventCountdown = FindObject<UFunction*>(XOR(L"Function /Junior/Blueprints/BP_Junior_Scripting.BP_Junior_Scripting_C:DestroyEventCountdown"));
			ProcessEvent(JuniorScripting, DestroyEventCountdown, &DoDestroyCountdown);
		}
	}

	void JuniorCarrierManager(bool ShouldUnloadHeliCarrier) // Scuffed Carrier Manager for Load/Unloading it. - notkrae
	{
		if (ShouldUnloadHeliCarrier)
		{
			UnloadStreamLevel(L"Apollo_POI_Hightower_HC_01_LevelInstance_2");
			UnloadStreamLevel(L"Apollo_POI_Hightower_HC_LobbyEvent_Overlay_LevelInstance_3");
			//UFunctions::UnloadStreamLevel(L"Apollo_POI_Hightower_HC_LobbyEvent_LevelInstance_1");
		}
		else
		{
			LoadStreamLevel(L"Apollo_POI_Hightower_HC_01_LevelInstance_2");
			LoadStreamLevel(L"Apollo_POI_Hightower_HC_LobbyEvent_Overlay_LevelInstance_3");
			//UFunctions::LoadStreamLevel(L"Apollo_POI_Hightower_HC_LobbyEvent_LevelInstance_1");
		}
	}

	void StartJuniorEvent()
	{
		float SecondsSinceEventBegan = 0.0f;
		bool FirstPerson = true;

		auto JuniorScripting = FindObject<UObject*>(XOR(L"BP_Junior_Scripting_C /Junior/Levels/Junior_Map.Junior_Map:PersistentLevel.BP_Junior_Scripting_Child_2"));
		auto JuniorGameCountdown = FindObject<UObject*>(XOR(L"BP_JCountdown_InGame_C /Game/Athena/Apollo/Maps/Test/S14/Apollo_POI_Hightower_HC_LobbyEvent_LevelInstance_1.Apollo_POI_Hightower_HC_LobbyEvent:PersistentLevel.BP_JCountdown_InGame_2"));

		UFunctions::SetTimeOfDaySpeed(0.25);
		UFunctions::SetTimeOfDay(20.5);

		if (JuniorGameCountdown)
		{
			auto FinalDisplay = FindObject<UFunction*>(XOR(L"Function /Game/Athena/Events/Junior/BP_JCountdown_InGame.BP_JCountdown_InGame_C:FinalDisplay"));

			ProcessEvent(JuniorGameCountdown, FinalDisplay, nullptr);
		}

		if (JuniorScripting)
		{
			UFunctions::SetTimeOfDaySpeed(0.25);

			auto startevent = FindObject<UFunction*>(XOR(L"Function /Junior/Blueprints/BP_Junior_Scripting.BP_Junior_Scripting_C:startevent"));
			auto HideDistantObject = FindObject<UFunction*>(XOR(L"Function /Junior/Blueprints/BP_Junior_Scripting.BP_Junior_Scripting_C:HideDistantObject")); // Destroys static Galactus Object - notkrae
			Sleep(100);

			ProcessEvent(JuniorScripting, startevent, &SecondsSinceEventBegan);
			ProcessEvent(JuniorScripting, HideDistantObject, nullptr);

		}

	}

	//Travis Scott(Jerky, 12.41)
	void StartJerky()
	{
		Play(JERKY_EVENT_PLAYER);
		//load maps like stages etc.
	}
	
	//The Device(Fritter, 12.61)
	void StartFritter()
	{
		Play(DEVICE_EVENT_PLAYER);
		//load maps like stages etc.
	}
	
	//Devourer of Worlds / Galacuts(Junior, 14.60)
	void StartJunior()
	{
		JuniorCarrierManager(false);
		JuniorCarrierManager(false);
		StartJuniorEvent();
	}

	//Rift Tour(Buffet, 17.30)
	void StartBuffet()
	{
		Play(RIFT_TOUR_EVENT_PLAYER);
	}

	//Skyfire(Kiwi, 17.50)
	void StartKiwi()
	{
		LoadAndStreamInLevel(KIWI_PRISONJUNCTION, FVector{ 50009.137f, 49958.379f, 100026.398f }, FRotator{});
		LoadAndStreamInLevel(KIWI_TUBES, FVector{ 50009.137f, 49958.379f, 100026.398f }, FRotator{});
		LoadAndStreamInLevel(KIWI_OBSERVATIONHALLWAY, FVector{ 50009.137f, 49958.379f, 100026.398f }, FRotator{});
		LoadAndStreamInLevel(KIWI_HANGAR, FVector{ 50009.137f, 49958.379f, 100026.398f }, FRotator{});
		LoadAndStreamInLevel(KIWI_KEVINROOM, FVector{ 50009.137f, 49958.379f, 100026.398f }, FRotator{});
		LoadAndStreamInLevel(KIWI_SPACE, FVector{ 50009.137f, 49958.379f, 100026.398f }, FRotator{});
		Play(KIWI_EVENT_PLAYER);
	}

	//The End(Guava, 18.40)
	void StartGuava()
	{
		Play(GUAVA_EVENT_PLAYER);
	}
	
	bool FindEventAndPlay()
	{
		if (gVersion == 12.41f)
		{
			StartJerky();
			return true;
		}
		else if (gVersion == 12.61f)
		{
			StartFritter();
			return true;
		}
		else if (gVersion == 14.60f)
		{
			StartJunior();
			return true;
		}
		else if (gVersion == 17.30f)
		{
			StartBuffet();
			return true;
		}
		else if (gVersion == 17.50f)
		{
			StartKiwi();
			return true;
		}
		else if (gVersion == 18.40f)
		{
			StartGuava();
			return true;
		}
		else
			return false;
	}

	void LoadPreEvent()
	{
		bool bCondition = true;
		
		if (gVersion == 12.41f)
		{
			auto JerkyLoader = FindObject<UObject*>(XOR(L"BP_Jerky_Loader_C /CycloneJerky/Levels/JerkyLoaderLevel.JerkyLoaderLevel:PersistentLevel.BP_Jerky_Loader_2"));
			UFunction* fn = FindObject<UFunction*>(XOR(L"Function /CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C:LoadJerkyLevel"));
			ProcessEvent(JerkyLoader, fn, &bCondition);
			//UFunctions::LoadAndStreamInLevel(JERKY_EVENT_MAP);
		}
		else if (gVersion == 12.61f)
		{
			auto FritterLoader = FindObject<UObject*>(XOR(L"BP_Fritter_Loader_C /Fritter/Level/FritterLoaderLevel.FritterLoaderLevel:PersistentLevel.BP_Fritter_Loader_0"));
			UFunction* fn = FindObject<UFunction*>(XOR(L"Function /Fritter/BP_Fritter_Loader.BP_Fritter_Loader_C:LoadFritterLevel"));
			ProcessEvent(FritterLoader, fn, &bCondition);
			//UFunctions::LoadAndStreamInLevel(DEVICE_EVENT_MAP);
		}
		else if (gVersion == 14.60f)
		{
			auto JuniorLoader = FindObject<UObject*>(XOR(L"BP_Junior_Loader_C /Junior/Levels/JuniorLoaderLevel.JuniorLoaderLevel:PersistentLevel.BP_Junior_Loader_2"));
			UFunction* fn = FindObject<UFunction*>(XOR(L"Function /Junior/Blueprints/BP_Junior_Loader.BP_Junior_Loader_C:LoadJuniorLevel"));
			ProcessEvent(JuniorLoader, fn, &bCondition);

			Events::JuniorPreShowManager(true); // If True, it will load the Carrier. - notkrae
			NeoPlayer.ExecuteConsoleCommand(L"bugitgo 1535 61840 42500 0 270");
		}
	}
}
