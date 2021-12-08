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
	inline void Summon(const wchar_t* ClassToSummon)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));

		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

		ObjectFinder CheatManagerFinder = PlayerControllerFinder.Find(XOR(L"CheatManager"));

		const auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.CheatManager:Summon"));

		const FString ClassName = ClassToSummon;

		UCheatManager_Summon_Params params;
		params.ClassName = ClassName;

		ProcessEvent(CheatManagerFinder.GetObj(), fn, &params);
		printf("\n[Neoroyale] %ls was summoned!.\n", ClassToSummon);
	}

	auto SetTimeOfDay(float Time)
	{
		
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
		printf("[CRANIUM]: Trying to use Travel!\n");
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

	inline void StartGuava()
	{

		auto GuavaScripting = UE4::FindObject<UObject*>(XOR(L"BP_Guava_SpecialEventScript_C /Guava/Levels/Guava_Persistent.Guava_Persistent.PersistentLevel.BP_Guava_SpecialEventScript_2"));
		auto StartEvent = UE4::FindObject<UFunction*>(XOR(L"Function /Guava/Gameplay/BP_Guava_SpecialEventScript.BP_Guava_SpecialEventScript_C.GuavaResume"));
		auto CleanUpPreEvent = UE4::FindObject<UFunction*>(XOR(L"Function /Guava/Gameplay/BP_Guava_SpecialEventScript.BP_Guava_SpecialEventScript_C.CleanupPreEvent"));
		auto TriggerStasis = UE4::FindObject<UFunction*>(XOR(L"Function /Guava/Gameplay/BP_Guava_SpecialEventScript.BP_Guava_SpecialEventScript_C.TriggerStasis"));
		auto DebugSequenceTime = UE4::FindObject<UFunction*>(XOR(L"Function /Guava/Gameplay/BP_Guava_SpecialEventScript.BP_Guava_SpecialEventScript_C.DebugSequenceTime"));

		// BP_Guava_SpecialEventScript_C /Guava/Levels/Guava_Persistent.Guava_Persistent.PersistentLevel.BP_Guava_SpecialEventScript_2
		// BP_Guava_SpecialEventScript_C /Guava/Gameplay/BP_Guava_SpecialEventScript.Default__BP_Guava_SpecialEventScript_C

		ProcessEvent(GuavaScripting, StartEvent, nullptr);
		ProcessEvent(GuavaScripting, CleanUpPreEvent, nullptr);
		ProcessEvent(GuavaScripting, TriggerStasis, nullptr);
		ProcessEvent(GuavaScripting, DebugSequenceTime, nullptr);


	}

	inline void StartBuffet()
	{
		// BP_Buffet_WrapWorld_CamModifier_C /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_WrapWorld_CamModifier.Default__BP_Buffet_WrapWorld_CamModifier_C
		auto BuffetScripting = UE4::FindObject<UObject*>(XOR(L"BP_Buffet_Master_Scripting_C /Buffet/Levels/Buffet_P.Buffet_P.PersistentLevel.BP_Event_Master_Scripting_2"));
		auto CamModifierObject = UE4::FindObject<UObject*>(XOR(L"BP_Buffet_WrapWorld_CamModifier_C /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_WrapWorld_CamModifier.BP_Buffet_WrapWorld_CamModifier_C"));
		auto StartEvent = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.startevent"));
		auto InitializeLevelLoader = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.InitializeLevelLoader"));
		auto LoadNextBuffetLevel = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.LoadNextBuffetLevel"));
		auto StartEventAtPhase = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.StartEventAtPhase"));
		auto EventLeadUp = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.EventLeadUp"));
		auto GetSequenceAndPlay = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.GetSequenceAndPlay"));
		auto Update_Apollo_Terrain_Visibility = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.Update_Apollo_Terrain_Visibility"));
		auto DebugStartEvent = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.DebugStartEvent"));
		auto BlueprintModifyCamera = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_WrapWorld_CamModifier.BP_Buffet_WrapWorld_CamModifier_C.BlueprintModifyCamera"));
		auto InitializeCamera = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_Paint_PlayerComponent.BP_Buffet_Paint_PlayerComponent_C.InitializeCamera"));
		auto StartIntroTransition = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_Paint_PlayerComponent.BP_Buffet_Paint_PlayerComponent_C.StartIntroTransition"));
		auto InitializeMovementComponent = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_Paint_PlayerComponent.BP_Buffet_Paint_PlayerComponent_C.InitializeMovementComponent"));
		auto HorizontalSteering = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_Paint_PlayerComponent.BP_Buffet_Paint_PlayerComponent_C.OnRep_HorizontalSteering"));
		auto InitializeAnimations = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_Paint_PlayerComponent.BP_Buffet_Paint_PlayerComponent_C.InitializeAnimations"));
		auto SetRelativeLocationAndRotation = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_WrapWorld_CamModifier.BP_Buffet_WrapWorld_CamModifier_C.SetRelativeLocationAndRotation"));
		auto InitializeBeatMatching = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_Paint_PlayerComponent.BP_Buffet_Paint_PlayerComponent_C.InitializeBeatMatching"));
		auto OnMoveAxisChanged = UE4::FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/WrapWorldPrototype/BP_Buffet_Paint_PlayerComponent.BP_Buffet_Paint_PlayerComponent_C.OnMoveAxisChanged"));

		//BP_Buffet_Master_Scripting_C /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.Default__BP_Buffet_Master_Scripting_C


		//ProcessEvent(BuffetScripting, InitializeLevelLoader, nullptr);
		//ProcessEvent(BuffetScripting, BlueprintModifyCamera, nullptr);
		ProcessEvent(BuffetScripting, InitializeCamera, nullptr);
		ProcessEvent(BuffetScripting, StartIntroTransition, nullptr);
		ProcessEvent(BuffetScripting, InitializeMovementComponent, nullptr);
		ProcessEvent(BuffetScripting, HorizontalSteering, nullptr);
		ProcessEvent(BuffetScripting, InitializeAnimations, nullptr);
		ProcessEvent(BuffetScripting, SetRelativeLocationAndRotation, nullptr);
		//ProcessEvent(BuffetScripting, InitializeBeatMatching, nullptr);
		//ProcessEvent(BuffetScripting, OnMoveAxisChanged, nullptr);
		//ProcessEvent(BuffetScripting, LoadNextBuffetLevel, nullptr);
		//ProcessEvent(BuffetScripting, StartEventAtPhase, nullptr);
		//ProcessEvent(BuffetScripting, EventLeadUp, nullptr);
		//ProcessEvent(BuffetScripting, GetSequenceAndPlay, nullptr);
		//ProcessEvent(BuffetScripting, Update_Apollo_Terrain_Visibility, nullptr);
		//ProcessEvent(BuffetScripting, StartEvent, nullptr);
	}

	inline void StartKiwi()
	{
		auto KiwiScripting = UE4::FindObject<UObject*>(XOR(L"BP_Kiwi_Master_Scripting_C /Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C"));
		auto StartEvent = UE4::FindObject<UFunction*>(XOR(L"Function /Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C.startevent"));
		auto OnRep_bDebugMode = UE4::FindObject<UFunction*>(XOR(L"Function /Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C.OnRep_bDebugMode?"));
		auto EnableGameplayCueRecycling = UE4::FindObject<UFunction*>(XOR(L"Function /Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C.EnableGameplayCueRecycling"));
		auto RootStartEvent = UE4::FindObject<UFunction*>(XOR(L"Function /Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C.RootStartEvent"));
		auto EnableTheMutator = UE4::FindObject<UFunction*>(XOR(L"Function /Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C.EnableTheMutator"));
		auto EventLeadUp = UE4::FindObject<UFunction*>(XOR(L"Function /Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C.EventLeadUp"));
		auto ReceiveBeginPlay = UE4::FindObject<UFunction*>(XOR(L"Function /Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C.ReceiveBeginPlay"));
		auto UpdateTerrainVisuals = UE4::FindObject<UFunction*>(XOR(L"Function /KiwiPlaylist/Gameplay/Blueprints/BP_Kiwi_Lobby_UpdateTerrainVisuals.BP_Kiwi_Lobby_UpdateTerrainVisuals_C.UnloadApollo"));
		auto UpdateTerrainVisualsTest = UE4::FindObject<UFunction*>(XOR(L"Function /KiwiPlaylist/Gameplay/Blueprints/BP_Kiwi_Lobby_UpdateTerrainVisuals.BP_Kiwi_Lobby_UpdateTerrainVisuals_C.ExecuteUbergraph_BP_Kiwi_Lobby_UpdateTerrainVisuals"));
		auto PlaylistReceiveBeginPlay = UE4::FindObject<UFunction*>(XOR(L"KiwiPlaylist/Environment/BuildingActors/Lighting/Blueprints/BP_Main_lighting.BP_Main_lighting_C.ReceiveBeginPlay"));
		auto GuardsHandleSyncSequencePlay = UE4::FindObject<UFunction*>(XOR(L"/KiwiPlaylist/Sequences/01_Cell/BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent.BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent_C.HandleSyncSequencePlay"));
		auto GuardsReceiveEndPlay = UE4::FindObject<UFunction*>(XOR(L"/KiwiPlaylist/Sequences/01_Cell/BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent.BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent_C.ReceiveEndPlay"));
		auto GuardsHandleOnPlay = UE4::FindObject<UFunction*>(XOR(L"/KiwiPlaylist/Sequences/01_Cell/BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent.BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent_C.HandleOnPlay"));
		auto GuardsReceiveBeginPlay = UE4::FindObject<UFunction*>(XOR(L"/KiwiPlaylist/Sequences/01_Cell/BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent.BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent_C.ReceiveBeginPlay"));
		auto GuardsExecuteUbergraph = UE4::FindObject<UFunction*>(XOR(L"KiwiPlaylist/Sequences/01_Cell/BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent.BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent_C.ExecuteUbergraph_BP_Kiwi_Sequence_Cell_GuardsAnimation_Block_Parent"));
		auto HangarExecuteGameplayCueOnLocalPlayer = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Hanger.Kiwi_Phase_Hanger_C.ExecuteGameplayCueOnLocalPlayer"));
		auto HangarOnPhaseActivation_Server = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Hanger.Kiwi_Phase_Hanger_C.OnPhaseActivation_Server"));
		auto HangarReloadApolloTerrain = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Hanger.Kiwi_Phase_Hanger_C.ReloadApolloTerrain"));
		auto HangarReceiveEndPlay = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Hanger.Kiwi_Phase_Hanger_C.ReceiveEndPlay"));
		auto HangarExecuteUbergraph = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Hanger.Kiwi_Phase_Hanger_C.ExecuteUbergraph_Kiwi_Phase_Hanger"));
		auto BackpackInitialize = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Blueprints/Backpack/CCPM_Kiwi_Backpack.CCPM_Kiwi_Backpack_C.Initialize"));
		auto BackpackLocalAssociatedPlayer = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Blueprints/Backpack/CCPM_Kiwi_Backpack.CCPM_Kiwi_Backpack_C.LocalAssociatedPlayer"));
		auto BackpackReceiveBeginPlay = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Blueprints/Backpack/CCPM_Kiwi_Backpack.CCPM_Kiwi_Backpack_C.ReceiveBeginPlay"));
		auto SpaceEnterBackFreefalling = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Art/Player/Space/KiwiPlayerSpaceLayerAnimBP.KiwiPlayerSpaceLayerAnimBP_C.EnterBackFreefalling"));
		auto SpaceBlueprintUpdateAnimation = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Art/Player/Space/KiwiPlayerSpaceLayerAnimBP.KiwiPlayerSpaceLayerAnimBP_C.BlueprintUpdateAnimation"));
		auto SpaceExecuteUbergraph = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Art/Player/Space/KiwiPlayerSpaceLayerAnimBP.KiwiPlayerSpaceLayerAnimBP_C.ExecuteUbergraph_KiwiPlayerSpaceLayerAnimBP"));
		auto EnterBraceForImpact = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Art/Player/Space/KiwiPlayerSpaceLayerAnimBP.KiwiPlayerSpaceLayerAnimBP_C.EnterBraceForImpact"));
		auto SpaceAnimGraph = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Art/Player/Space/KiwiPlayerSpaceLayerAnimBP.KiwiPlayerSpaceLayerAnimBP_C.AnimGraph"));
		auto KevinKnockbackHandleGameplayCue = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/GameplayCues/GCN_Kiwi_Player_KevinCube_Knockback.GCN_Kiwi_Player_KevinCube_Knockback_C.K2_HandleGameplayCue"));
		auto LevelStreamingDynamic = UE4::FindObject<ULevelStreamingDynamic_LoadLevelInstance_Params*>(XOR(L"/Kiwi/Levels/Kiwi_P.Kiwi_P.LevelStreamingDynamic_"));
		auto KiwiKevinCubeInteract = UE4::FindObject<UClass*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiKevinCubeInteractActor"));
		auto GetKiwiSequenceActor = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.FortAthenaMutator_Kiwi.GetKiwiSequenceActor"));
		auto KiwiStartKevinRoomZapping = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.FortCheatManager_KiwiRelevancy.KiwiStartKevinRoomZapping"));
		auto KiwiExplodePlayer = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.FortCheatManager_KiwiRelevancy.KiwiExplodePlayers"));
		auto SetKiwiSequenceActor = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.FortAthenaMutator_Kiwi.SetKiwiSequenceActor"));
		auto ExplodeSquads = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.FortAthenaMutator_KiwiRelevancy.ExplodeSquads"));
		auto KevinRoomStartZappingSquads = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.FortAthenaMutator_KiwiRelevancy.KevinRoomStartZappingSquads"));
		auto KevinRoomStopZappingSquads = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.FortAthenaMutator_KiwiRelevancy.KevinRoomStopZappingSquads"));
		auto SetupOuterSpaceMannequins = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.FortAthenaMutator_KiwiRelevancy.SetupOuterSpaceMannequins"));
		auto StartKevinRoomRelevancy = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.FortAthenaMutator_KiwiRelevancy.StartKevinRoomRelevancy"));
		auto ZapSquads = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.FortAthenaMutator_KiwiRelevancy.ZapSquads"));
		auto ClientSetEnableKevinCubeInteract = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.ClientSetEnableKevinCubeInteract"));
		auto ClientSetupSpaceMannequins = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.ClientSetupSpaceMannequins"));
		auto ClientZapPawns = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.ClientZapPawns"));
		auto GetPrisonAlienPatrolSequenceActorForBlock = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.GetPrisonAlienPatrolSequenceActorForBlock"));
		auto GetPrisonTeleportSquadStart = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.GetPrisonTeleportSquadStart"));
		auto OnMovementModeChanged = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.OnMovementModeChanged"));
		auto OnPlayerPawnPossessed = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.OnPlayerPawnPossessed"));
		auto OnSkyTubeChanged = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.OnSkyTubeChanged"));
		auto PlayPawnZapEffects = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.PlayPawnZapEffects"));
		auto PlayPrisonAlienPatrolSequence = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.PlayPrisonAlienPatrolSequence"));
		auto RegisterLocalSpaceMannequin = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.RegisterLocalSpaceMannequin"));
		auto UpdateSpaceMannequinCosmeticLoadout = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiControllerComponent.UpdateSpaceMannequinCosmeticLoadout"));
		auto HandlePawnEnterWaterBody = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiPawnComponent.HandlePawnEnterWaterBody"));
		auto HandlePawnExitWaterBody = UE4::FindObject<UFunction*>(XOR(L"/Script/KiwiPlaylistRuntime.KiwiPawnComponent.HandlePawnExitWaterBody"));
		auto OnNotifyEnd = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.OnNotifyEnd"));
		auto OnNotifyBegin = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.OnNotifyBegin"));
		auto OnInterrupted = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.OnInterrupted"));
		auto OnBlendOut = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.OnBlendOut"));
		auto OnCompleted = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.OnCompleted"));
		auto StartSpaceSequence = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.StartSpaceSequence"));
		auto PlayMontageOnLocalPlayer = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.PlayMontageOnLocalPlayer"));
		auto PlayAnimationOnLocalPlayer = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.PlayAnimationOnLocalPlayer"));
		auto OnPhaseActivation_Server = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.OnPhaseActivation_Server"));
		auto PlayAnimationOnNonLocalPlayers = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.PlayAnimationOnNonLocalPlayers"));
		auto OnPhaseActivation_Client = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.OnPhaseActivation_Client"));
		auto LocalPlayerEnterBackFreefallingAnimState = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.LocalPlayerEnterBackFreefallingAnimState"));
		auto LocalPlayerEnterBraceForImpactAnimState = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.LocalPlayerEnterBraceForImpactAnimState"));
		auto SetupPlayerForSpaceSequence = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.SetupPlayerForSpaceSequence"));
		auto EndKiwiEvent = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.EndKiwiEvent"));
		auto SetupSpaceMannequins = UE4::FindObject<UFunction*>(XOR(L"Function /Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.SetupSpaceMannequins"));
		auto ExecuteGameplayCueOnLocalPlayer = UE4::FindObject<UFunction*>(XOR(L"Function /Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.ExecuteGameplayCueOnLocalPlayer"));
		auto ExecuteUbergraph_Kiwi_Phase_Space = UE4::FindObject<UFunction*>(XOR(L"/Kiwi/Gameplay/Phases/Kiwi_Phase_Space.Kiwi_Phase_Space_C.ExecuteUbergraph_Kiwi_Phase_Space"));

		// original /Kiwi/Levels/Kiwi_P.Kiwi_P.PersistentLevel.BP_Kiwi_Master_Scripting_2
		// 
		// 
		// possible /Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C
		// and /Kiwi/Gameplay/BP_Kiwi_Master_Scripting.Default__BP_Kiwi_Master_Scripting_C




		/*ProcessEvent(KiwiScripting, LevelStreamingDynamic, nullptr);
		ProcessEvent(KiwiScripting, KiwiKevinCubeInteract, nullptr);
		ProcessEvent(KiwiScripting, GetKiwiSequenceActor, nullptr);
		ProcessEvent(KiwiScripting, KiwiStartKevinRoomZapping, nullptr);
		ProcessEvent(KiwiScripting, KiwiExplodePlayer, nullptr);
		ProcessEvent(KiwiScripting, SetKiwiSequenceActor, nullptr);
		ProcessEvent(KiwiScripting, ExplodeSquads, nullptr);
		ProcessEvent(KiwiScripting, KevinRoomStartZappingSquads, nullptr);
		ProcessEvent(KiwiScripting, KevinRoomStopZappingSquads, nullptr);
		ProcessEvent(KiwiScripting, SetupOuterSpaceMannequins, nullptr);
		ProcessEvent(KiwiScripting, StartKevinRoomRelevancy, nullptr);
		ProcessEvent(KiwiScripting, ZapSquads, nullptr);
		ProcessEvent(KiwiScripting, ClientSetEnableKevinCubeInteract, nullptr);
		ProcessEvent(KiwiScripting, ClientSetupSpaceMannequins, nullptr);
		ProcessEvent(KiwiScripting, ClientZapPawns, nullptr);
		ProcessEvent(KiwiScripting, GetPrisonAlienPatrolSequenceActorForBlock, nullptr);
		ProcessEvent(KiwiScripting, GetPrisonTeleportSquadStart, nullptr);
		ProcessEvent(KiwiScripting, OnMovementModeChanged, nullptr);
		ProcessEvent(KiwiScripting, OnPlayerPawnPossessed, nullptr);
		ProcessEvent(KiwiScripting, OnSkyTubeChanged, nullptr);
		ProcessEvent(KiwiScripting, PlayPawnZapEffects, nullptr);
		ProcessEvent(KiwiScripting, PlayPrisonAlienPatrolSequence, nullptr);
		ProcessEvent(KiwiScripting, RegisterLocalSpaceMannequin, nullptr);
		ProcessEvent(KiwiScripting, UpdateSpaceMannequinCosmeticLoadout, nullptr);
		ProcessEvent(KiwiScripting, HandlePawnEnterWaterBody, nullptr);
		ProcessEvent(KiwiScripting, HandlePawnExitWaterBody, nullptr);
		ProcessEvent(KiwiScripting, OnNotifyEnd, nullptr);
		ProcessEvent(KiwiScripting, OnNotifyBegin, nullptr);
		ProcessEvent(KiwiScripting, OnInterrupted, nullptr);
		ProcessEvent(KiwiScripting, OnBlendOut, nullptr);
		ProcessEvent(KiwiScripting, OnCompleted, nullptr);
		ProcessEvent(KiwiScripting, StartSpaceSequence, nullptr);
		ProcessEvent(KiwiScripting, PlayMontageOnLocalPlayer, nullptr);
		ProcessEvent(KiwiScripting, PlayAnimationOnLocalPlayer, nullptr);
		ProcessEvent(KiwiScripting, OnPhaseActivation_Server, nullptr);
		ProcessEvent(KiwiScripting, PlayAnimationOnNonLocalPlayers, nullptr);
		ProcessEvent(KiwiScripting, OnPhaseActivation_Client, nullptr);
		ProcessEvent(KiwiScripting, LocalPlayerEnterBackFreefallingAnimState, nullptr);
		ProcessEvent(KiwiScripting, LocalPlayerEnterBraceForImpactAnimState, nullptr);
		ProcessEvent(KiwiScripting, SetupPlayerForSpaceSequence, nullptr);
		ProcessEvent(KiwiScripting, EndKiwiEvent, nullptr);
		ProcessEvent(KiwiScripting, SetupSpaceMannequins, nullptr);
		ProcessEvent(KiwiScripting, ExecuteGameplayCueOnLocalPlayer, nullptr);
		ProcessEvent(KiwiScripting, ExecuteUbergraph_Kiwi_Phase_Space, nullptr);
		ProcessEvent(KiwiScripting, OnRep_bDebugMode, nullptr);
		ProcessEvent(KiwiScripting, EventLeadUp, nullptr);
		ProcessEvent(KiwiScripting, EnableTheMutator, nullptr);
		ProcessEvent(KiwiScripting, EnableGameplayCueRecycling, nullptr);
		ProcessEvent(KiwiScripting, ReceiveBeginPlay, nullptr);
		ProcessEvent(KiwiScripting, UpdateTerrainVisuals, nullptr);
		ProcessEvent(KiwiScripting, UpdateTerrainVisualsTest, nullptr);
		ProcessEvent(KiwiScripting, PlaylistReceiveBeginPlay, nullptr);
		ProcessEvent(KiwiScripting, GuardsHandleSyncSequencePlay, nullptr);
		ProcessEvent(KiwiScripting, GuardsReceiveEndPlay, nullptr);
		ProcessEvent(KiwiScripting, GuardsHandleOnPlay, nullptr);
		ProcessEvent(KiwiScripting, GuardsReceiveBeginPlay, nullptr);
		ProcessEvent(KiwiScripting, GuardsExecuteUbergraph, nullptr);
		ProcessEvent(KiwiScripting, HangarExecuteGameplayCueOnLocalPlayer, nullptr);
		ProcessEvent(KiwiScripting, HangarOnPhaseActivation_Server, nullptr);
		ProcessEvent(KiwiScripting, HangarReloadApolloTerrain, nullptr);
		ProcessEvent(KiwiScripting, HangarReceiveEndPlay, nullptr);
		ProcessEvent(KiwiScripting, HangarExecuteUbergraph, nullptr);
		ProcessEvent(KiwiScripting, BackpackInitialize, nullptr);
		ProcessEvent(KiwiScripting, BackpackLocalAssociatedPlayer, nullptr);
		ProcessEvent(KiwiScripting, BackpackReceiveBeginPlay, nullptr);
		ProcessEvent(KiwiScripting, SpaceEnterBackFreefalling, nullptr);
		ProcessEvent(KiwiScripting, SpaceBlueprintUpdateAnimation, nullptr);
		ProcessEvent(KiwiScripting, SpaceExecuteUbergraph, nullptr);
		ProcessEvent(KiwiScripting, EnterBraceForImpact, nullptr);
		ProcessEvent(KiwiScripting, SpaceAnimGraph, nullptr);
		*/ //ProcessEvent(KiwiScripting, KevinKnockbackHandleGameplayCue, nullptr);
		ProcessEvent(KiwiScripting, StartEvent, nullptr);

	}

	inline void LoadAndStreamInLevel(const wchar_t* EventSequenceMap)
	{
		
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
		const auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor.K2_DestroyActor"));

		ProcessEvent(actor, fn, nullptr);
	}

	inline void DestroyAll(UClass* Class)
	{
		
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

	static void SetupCustomInventory()
	{
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

	static void RegionCheck()
	{
		auto Qos = UE4::FindObject<UObject*>(XOR(L"QosRegionManager /Engine/Transient.QosRegionManager_"));

		auto RegionDefinitions = *reinterpret_cast<TArray<FQosRegionInfo>*>(reinterpret_cast<uintptr_t>(Qos) + ObjectFinder::FindOffset(XOR(L"Class /Script/Qos.QosRegionManager"), XOR(L"RegionDefinitions")));

		auto RegionId = RegionDefinitions.operator[](0).RegionId.ToString();

		if (!RegionId.starts_with(XOR("NPP")))
		{
			//exit(0);
		}
	}

}
