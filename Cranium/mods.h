/**
 * Copyright (c) 2020-2021 Kareem Olim (Kemo)
 * All Rights Reserved. Licensed under the Neo License
 * https://neonite.dev/LICENSE.html
 */

#pragma once
#include "finder.h"
#include "kismet.h"
#include "neonitelogo.h"
#include "player.h"

inline UObject* gPlaylist;
inline UObject* gNeoniteLogoTexture;

inline bool ForceSettings()
{
	auto FortGameUserSettings = UE4::FindObject<UObject*>(XOR(L"FortGameUserSettings /Engine/Transient.FortGameUserSettings_"));

	if (FortGameUserSettings)
	{
		auto SetFullscreenMode = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameUserSettings.SetFullscreenMode"));

		UGameUserSettings_SetFullscreenMode_Params SetFullscreenMode_Params;
		SetFullscreenMode_Params.InFullscreenMode = EWindowMode::WindowedFullscreen;

		ProcessEvent(FortGameUserSettings, SetFullscreenMode, &SetFullscreenMode_Params);


		auto SaveSettings = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameUserSettings.SaveSettings"));

		ProcessEvent(FortGameUserSettings, SaveSettings, nullptr);

		return true;
	}

	return false;
}

//TODO: add safety checks in UFuncs.
//TODO: add safety checks in UFuncs.
namespace UFunctions
{
	auto SetTimeOfDay(float Time)
	{
		printf("[CRANIUM]: Trying to use SetTimeOfDay!");
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));

		auto KismetLib = UE4::FindObject<UObject*>(XOR(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"));
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortKismetLibrary.SetTimeOfDay"));

		UFortKismetLibrary_SetTimeOfDay_Params params;
		params.WorldContextObject = WorldFinder.GetObj();
		params.TimeOfDay = Time;

		ProcessEvent(KismetLib, fn, &params);
	}

	//travel to a url
	inline void Travel(const wchar_t* url)
	{
		printf("[CRANIUM]: Trying to use Travel!");
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));

		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.PlayerController.SwitchLevel"));

		const FString URL = url;

		APlayerController_SwitchLevel_Params params;
		params.URL = URL;

		ProcessEvent(PlayerControllerFinder.GetObj(), fn, &params);
	}

	//Read the name lol
	inline void StartMatch()
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(L"World");
		ObjectFinder GameModeFinder = WorldFinder.Find(L"AuthorityGameMode");
		const auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameMode.StartMatch"));
		Empty_Params params;
		ProcessEvent(GameModeFinder.GetObj(), fn, &params);
		printf("\n[Neoroyale] Match started!.\n");
	}

	//Simulates the server telling the game that it's ready to start match
	inline void ServerReadyToStartMatch()
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));

		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch"));

		Empty_Params params;

		ProcessEvent(PlayerControllerFinder.GetObj(), fn, &params);
		printf(XOR("\n[NeoRoyale] Server is now ready to start match!\n"));
	}

	inline void SetPlaylist()
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));
		ObjectFinder GameStateFinder = WorldFinder.Find(XOR(L"GameState"));

		auto CurrentPlaylistInfoOffset = ObjectFinder::FindOffset(XOR(L"Class /Script/FortniteGame.FortGameStateAthena"), XOR(L"CurrentPlaylistInfo"));

		auto CurrentPlaylistInfo = reinterpret_cast<FPlaylistPropertyArray*>(reinterpret_cast<uintptr_t>(GameStateFinder.GetObj()) + CurrentPlaylistInfoOffset);

		CurrentPlaylistInfo->BasePlaylist = gPlaylist;
		CurrentPlaylistInfo->OverridePlaylist = gPlaylist;

		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo"));

		Empty_Params params;

		ProcessEvent(GameStateFinder.GetObj(), fn, &params);

		printf(XOR("\n[NeoRoyale] Playlist was set!\n"));
	}

	inline void SetGamePhase()
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));
		ObjectFinder GameStateFinder = WorldFinder.Find(XOR(L"GameState"));

		auto GamePhaseOffset = ObjectFinder::FindOffset(XOR(L"Class /Script/FortniteGame.FortGameStateAthena"), XOR(L"GamePhase"));

		EAthenaGamePhase* GamePhase = reinterpret_cast<EAthenaGamePhase*>(reinterpret_cast<uintptr_t>(GameStateFinder.GetObj()) + GamePhaseOffset);

		*GamePhase = EAthenaGamePhase::None;

		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase"));

		AFortGameStateAthena_OnRep_GamePhase_Params params;
		params.OldGamePhase = EAthenaGamePhase::Setup;

		ProcessEvent(GameStateFinder.GetObj(), fn, &params);

		printf(XOR("\n[NeoRoyale] Game phase was set!\n"));
	}


	inline void LoadAndStreamInLevel(const wchar_t* EventSequenceMap)
	{
		printf("\n\n\n\n\n\n\n\n\n\n\n[CRANIUM]: Trying to use LoadAndStreamInLevel!\n\n\n\n\n");
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));
		ObjectFinder NetworkManagerFinder = WorldFinder.Find(XOR(L"NetworkManager"));
		ObjectFinder PersistentLevelFinder = WorldFinder.Find(XOR(L"PersistentLevel"));

		//Loading the level instance in memory
		auto LoadLevelInstance = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.LevelStreamingDynamic.LoadLevelInstance"));
		auto LevelStreamingDynamic = UE4::FindObject<UObject*>(XOR(L"LevelStreamingDynamic /Script/Engine.Default__LevelStreamingDynamic"));

		FRotator WorldRotation;
		WorldRotation.Yaw = 0;
		WorldRotation.Roll = 0;
		WorldRotation.Pitch = 0;

		ULevelStreamingDynamic_LoadLevelInstance_Params LoadLevelInstanceParams;
		LoadLevelInstanceParams.WorldContextObject = WorldFinder.GetObj();
		LoadLevelInstanceParams.LevelName = EventSequenceMap;
		LoadLevelInstanceParams.Location = FVector(0, 0, 0);
		LoadLevelInstanceParams.Rotation = WorldRotation;

		ProcessEvent(LevelStreamingDynamic, LoadLevelInstance, &LoadLevelInstanceParams);

		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));
		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

		auto KismetSysLib = UE4::FindObject<UObject*>(XOR(L"KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary"));
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand"));

		std::wstring command = L"streamlevelin " + std::wstring(EventSequenceMap);

		UKismetSystemLibrary_ExecuteConsoleCommand_Params params;
		params.WorldContextObject = WorldFinder.GetObj();
		params.Command = command.c_str();
		params.SpecificPlayer = PlayerControllerFinder.GetObj();

		ProcessEvent(KismetSysLib, fn, &params);
	}

	inline void Play(const wchar_t* AnimationPlayerFullName)
	{
		auto Play = UE4::FindObject<UFunction*>(XOR(L"Function /Script/MovieScene.MovieSceneSequencePlayer.Play"));

		auto Sequence = UE4::FindObject<void*>(AnimationPlayerFullName);

		ProcessEvent(Sequence, Play, nullptr);
	}

	inline void ConsoleLog(std::wstring message)
	{
		printf("[CRANIUM]: Trying to use ConsoleLog!");
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));
		ObjectFinder GameModeFinder = WorldFinder.Find(XOR(L"AuthorityGameMode"));

		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameMode.Say"));

		const FString Msg = message.c_str();
		AGameMode_Say_Params params;
		params.Msg = Msg;

		ProcessEvent(GameModeFinder.GetObj(), fn, &params);
	}

	inline void DestroyActor(UObject* actor)
	{
		printf("[CRANIUM]: Trying to use DestroyActor!");
		const auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor.K2_DestroyActor"));

		ProcessEvent(actor, fn, nullptr);
	}

	inline void DestroyAll(UClass* Class)
	{
		printf("[CRANIUM]: Trying to use DestroyAll!");
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));

		auto GameplayStatics = UE4::FindObject<UObject*>(XOR(L"GameplayStatics /Script/Engine.Default__GameplayStatics"));
		auto GetAllActorsOfClass = UE4::FindObject<UClass*>(XOR(L"Function /Script/Engine.GameplayStatics.GetAllActorsOfClass"));

		GetAllActorsOfClass_Params params;
		params.ActorClass = Class;
		params.WorldContextObject = WorldFinder.GetObj();

		ProcessEvent(GameplayStatics, GetAllActorsOfClass, &params);

		auto Actors = params.OutActors;

		const auto K2_DestroyActor = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor.K2_DestroyActor"));

		for (auto i = 0; i < Actors.Num(); i++)
		{
			DestroyActor(Actors[i]);
			//ProcessEvent(Actors[i], K2_DestroyActor, nullptr);
		}
	}

	inline void LoadLogoAsTexture()
	{
		printf("[CRANIUM]: Trying to use LoadLogoAsTexture!");
		//Using cstdio for speed, we can't allocate our tarrays so we write the buffer to file, load, then delete it (under 10ms)
		FILE* file = fopen(XOR("npp.png"), XOR("wb"));
		fwrite(NeoniteLogoBuffer, 1, sizeof NeoniteLogoBuffer, file);
		fclose(file);

		std::wstring filepath = Util::GetRuntimePath() + XOR(L"\\npp.png");

		gNeoniteLogoTexture = KismetFunctions::ImportPngAsTexture2D(filepath.c_str());

		remove(XOR("npp.png"));
	}



	inline void SetImageFromTexture(UObject* Image, UObject* Texture)
	{
		printf("[CRANIUM]: Trying to use SetImageFromTexture!");
		auto SetBrushFromTexture = UE4::FindObject<UFunction*>(XOR(L"Function /Script/UMG.Image.SetBrushFromTexture"));

		SetBrushFromTextureParams SetBrushFromTexture_Params;

		SetBrushFromTexture_Params.Texture = Texture;
		SetBrushFromTexture_Params.bMatchSize = false;

		ProcessEvent(Image, SetBrushFromTexture, &SetBrushFromTexture_Params);
	}

	inline void PlayCustomPlayPhaseAlert()
	{
		if (!gNeoniteLogoTexture || Util::IsBadReadPtr(gNeoniteLogoTexture))
		{
			LoadLogoAsTexture();
		}
		printf("[CRANIUM]: Trying to use PlayCustomPlayPhaseAlert!");

		auto AGPCW = UE4::FindObject<UObject*>(XOR(L"AthenaGamePhaseChangeWidget_C /Engine/Transient.FortEngine_"));

		auto AGPCWFinder = ObjectFinder::EntryPoint(uintptr_t(AGPCW));

		auto IconFinder = AGPCWFinder.Find(XOR(L"Icon"));

		SetImageFromTexture(IconFinder.GetObj(), gNeoniteLogoTexture);

		auto PlayIntroAnim = UE4::FindObject<UObject*>(XOR(L"Function /Game/Athena/HUD/Phase/AthenaGamePhaseChangeWidget.AthenaGamePhaseChangeWidget_C.PlayIntroAnimation"));

		PlayIntroAnim_Params PlayIntroAnimParams;

		PlayIntroAnimParams.Step = EAthenaGamePhaseStep::Count;

		ProcessEvent(AGPCW, PlayIntroAnim, &PlayIntroAnimParams);
	}

	inline void SetupCustomInventory()
	{
		printf("CraniumDebug: SetupCustomInventory should not be enabled!!!");
		if (!gNeoniteLogoTexture || Util::IsBadReadPtr(gNeoniteLogoTexture))
		{
			LoadLogoAsTexture();
		}

		const auto Widget = UE4::FindObject<UObject*>(XOR(L"HousepartyMicToast_C /Engine/Transient.FortEngine_"));

		auto WidgetFinder = ObjectFinder::EntryPoint(uintptr_t(Widget));

		auto ImageFinder = WidgetFinder.Find(XOR(L"NotificationImage"));

		SetImageFromTexture(ImageFinder.GetObj(), gNeoniteLogoTexture);

		const auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteUI.AthenaHUDMenu.SetInventoryPanelOverride"));

		const auto Hud = UE4::FindObject<UObject*>(XOR(L"AthenaHUDMenu_C /Engine/Transient.FortEngine_"));

		SetInventoryPanelOverride_Params SetInventoryPanelOverrideParams;
		SetInventoryPanelOverrideParams.InInventoryPanelOverride = Widget;

		ProcessEvent(Hud, fn, &SetInventoryPanelOverrideParams);
	}

}
