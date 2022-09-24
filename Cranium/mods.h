#pragma once
#include "finder.h"
#include "kismet.h"
#include "neonitelogo.h"
#include "pawn.h"

inline UObject* gPlaylist;
inline UObject* gNeoniteLogoTexture;

inline bool ForceSettings()
{
	auto FortGameUserSetttings = FindObject<UObject*>(XOR(L"FortGameUserSettings /Engine/Transient.FortGameUserSettings_"));

	if (FortGameUserSetttings)
	{
		auto SetFullscreenMode = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameUserSettings:SetFullscreenMode"));

		UGameUserSettings_SetFullscreenMode_Params SetFullscreenMode_Params;
		SetFullscreenMode_Params.InFullscreenMode = EWindowMode::WindowedFullscreen;

		ProcessEvent(FortGameUserSetttings, SetFullscreenMode, &SetFullscreenMode_Params);


		auto SaveSettings = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameUserSettings:SaveSettings"));

		ProcessEvent(FortGameUserSetttings, SaveSettings, nullptr);

		return true;
	}

	return false;
}

//TODO: add safety checks in UFuncs.
namespace UFunctions
{
	static void UnlockConsole()
	{
		auto FortGameViewportClient = FindObject<UObject*>(XOR(L"FortGameViewportClient /Engine/Transient.FortEngine_"));
		auto fn = FindObject<UObject*>(XOR(L"Function /Script/Engine.GameplayStatics.SpawnObject"));
		auto statics = FindObject<UFunction*>(XOR(L"GameplayStatics /Script/Engine.Default__GameplayStatics"));
		auto ConsoleClass = FindObject<UObject*>(XOR(L"/Script/Engine.Console"));
		auto ViewportConsole = reinterpret_cast<UObject**>((uintptr_t)FortGameViewportClient + 0x40);

		SpawnObjectParams params;
		params.ObjectClass = ConsoleClass;
		params.Outer = FortGameViewportClient;

		ProcessEvent(statics, fn, &params);

		//std::cout << "Return Value: " << params.ReturnValue;
		*ViewportConsole = params.ReturnValue;

	}

	auto SetTimeOfDaySpeed(float TimeSpeed)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));

		auto KismetLib = FindObject<UObject*>(XOR(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"));
		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortKismetLibrary:SetTimeOfDaySpeed"));

		UFortKismetLibrary_SetTimeOfDay_Params params;
		params.WorldContextObject = WorldFinder.GetObj();
		params.TimeOfDay = TimeSpeed;

		ProcessEvent(KismetLib, fn, &params);
	}

	auto SetTimeOfDay(float Time)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));

		auto KismetLib = FindObject<UObject*>(XOR(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"));
		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortKismetLibrary:SetTimeOfDay"));

		UFortKismetLibrary_SetTimeOfDay_Params params;
		params.WorldContextObject = WorldFinder.GetObj();
		params.TimeOfDay = Time;

		ProcessEvent(KismetLib, fn, &params);
	}

	inline void TeleportToSpawn()
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));

		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

		ObjectFinder CheatManagerFinder = PlayerControllerFinder.Find(XOR(L"CheatManager"));

		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.CheatManager:BugItGo"));

		UCheatManager_BugItGo_Params params;
		params.X = -156128.36;
		params.Y = -159492.78;
		params.Z = -2996.30;
		params.Pitch = 0;
		params.Yaw = 0;
		params.Roll = 0;

		ProcessEvent(CheatManagerFinder.GetObj(), fn, &params);

		printf(XOR("\n[NeoRoyale] Teleported to spawn island.\n"));
	}

	inline void TeleportToMain()
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));

		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

		ObjectFinder CheatManagerFinder = PlayerControllerFinder.Find(XOR(L"CheatManager"));

		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.CheatManager:BugItGo"));

		UCheatManager_BugItGo_Params params;
		params.X = 0;
		params.Y = 0;
		params.Z = 0;
		params.Pitch = 0;
		params.Yaw = 0;
		params.Roll = 0;

		ProcessEvent(CheatManagerFinder.GetObj(), fn, &params);
	}

	inline void TeleportToCoords(float X, float Y, float Z)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));

		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

		ObjectFinder CheatManagerFinder = PlayerControllerFinder.Find(XOR(L"CheatManager"));

		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.CheatManager:BugItGo"));

		UCheatManager_BugItGo_Params params;
		params.X = X;
		params.Y = Y;
		params.Z = Z;
		params.Pitch = 0;
		params.Yaw = 0;
		params.Roll = 0;

		ProcessEvent(CheatManagerFinder.GetObj(), fn, &params);
	}

	inline UObject* FindActor(UObject* Class, int Skip = 0)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(L"GameViewport");
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(L"World");

		static UObject* Default__GameplayStatics = FindObject<UObject*>(L"GameplayStatics /Script/Engine.Default__GameplayStatics");
		static UObject* GetAllActorsOfClass = FindObject<UFunction*>(L"Function /Script/Engine.GameplayStatics:GetAllActorsOfClass");

		struct
		{
			UObject* World;
			UObject* Class;
			TArray<UObject*> ReturnValue;
		} Params;

		Params.World = WorldFinder.GetObj();
		Params.Class = Class;

		ProcessEvent(Default__GameplayStatics, GetAllActorsOfClass, &Params);

		return Params.ReturnValue[Skip];
	}

	inline void DestroyAllHLODs()
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));

		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

		ObjectFinder CheatManagerFinder = PlayerControllerFinder.Find(XOR(L"CheatManager"));

		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.CheatManager.DestroyAll"));
		}
		else
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.CheatManager:DestroyAll"));

		}
		auto HLODSMActor = FindObject<UClass*>(XOR(L"Class /Script/FortniteGame.FortHLODSMActor"));

		UCheatManager_DestroyAll_Params params;
		params.Class = HLODSMActor;

		ProcessEvent(CheatManagerFinder.GetObj(), fn, &params);
		printf(XOR("[NeoRoyale] HLODSM Actor was destroyed."));
	}

	//travel to a url
	inline void Travel(const wchar_t* url)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));
		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.PlayerController.SwitchLevel"));
		}
		else {
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.PlayerController:SwitchLevel"));

		}
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
		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameMode.StartMatch"));
		}
		else {
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameMode:StartMatch"));
		}
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

		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch"));
		}
		else {
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController:ServerReadyToStartMatch"));
		}
		Empty_Params params;

		ProcessEvent(PlayerControllerFinder.GetObj(), fn, &params);
		printf(XOR("\n[NeoRoyale] Server is now ready to start match!\n"));
	}

	inline void StartBuffet()
	{
		auto BP_Buffet_Master_Scripting_C = FindObject<UObject*>(XOR(L"BP_Buffet_Master_Scripting_C /Buffet/Levels/Buffet_P.Buffet_P.PersistentLevel.BP_Event_Master_Scripting_2"));
		auto LoadNextBuffetLevel = FindObject<UFunction*>(XOR(L"Function /Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.LoadNextBuffetLevel"));

		FLoadNextBuffetLevel Params;
		/*Params.CallFuncConcatStrStrReturnValue = false;
		Params.CallFuncConvIntToStringReturnValue = false;
		Params.PhaseIndex = false;*/

		ProcessEvent(BP_Buffet_Master_Scripting_C, LoadNextBuffetLevel, &Params);
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

		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo"));
		}
		else {
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortGameStateAthena:OnRep_CurrentPlaylistInfo"));

		}

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

		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase"));
		}
		else {
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortGameStateAthena:OnRep_GamePhase"));
		}

		AFortGameStateAthena_OnRep_GamePhase_Params params;
		params.OldGamePhase = EAthenaGamePhase::Setup;

		ProcessEvent(GameStateFinder.GetObj(), fn, &params);

		printf(XOR("\n[NeoRoyale] Game phase was set!\n"));
	}

	inline void ConsoleLog(std::wstring message)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));
		ObjectFinder GameModeFinder = WorldFinder.Find(XOR(L"AuthorityGameMode"));

		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameMode.Say"));
		}
		else {
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.GameMode:Say"));

		}
		const FString Msg = message.c_str();
		AGameMode_Say_Params params;
		params.Msg = Msg;

		ProcessEvent(GameModeFinder.GetObj(), fn, &params);
	}

	inline void DestroyActor(UObject* actor)
	{
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor.K2_DestroyActor"));
		else
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor:K2_DestroyActor"));
		ProcessEvent(actor, fn, nullptr);
	}

	inline void LoadLogoAsTexture()
	{
		//Using cstdio for speed, we can't allocate our tarrays so we write the buffer to file, load, then delete it (under 10ms)
		FILE* file = fopen(XOR("npp.png"), XOR("wb"));
		fwrite(NeoniteLogoBuffer, 1, sizeof NeoniteLogoBuffer, file);
		fclose(file);

		std::wstring filepath = Util::GetRuntimePath() + XOR(L"\\npp.png");

		gNeoniteLogoTexture = KismetFunctions::ImportPngAsTexture2D(filepath.c_str());

		remove(XOR("npp.png"));
	}

	inline void SetBodyCustomTextureFromPng(const wchar_t* PngFileFullPath, bool bIsHead = false)
	{
		const auto SetTextureParameterValue = FindObject<UFunction*>(XOR(L"Function /Script/Engine.MaterialInstanceDynamic:SetTextureParameterValue"));

		UMaterialInstanceDynamic_SetTextureParameterValue_Params params;
		params.ParameterName = KismetFunctions::GetFName(XOR(L"Diffuse"));
		params.Value = KismetFunctions::ImportPngAsTexture2D(PngFileFullPath);

		for (auto i = 0; i < 6; i++)
		{
			auto MaterialInstanceDynamic = FindObject<UObject*>(
				XOR(L"MaterialInstanceDynamic /Game/Athena/Apollo/Maps/Apollo_Terrain.Apollo_Terrain:PersistentLevel.PlayerPawn_Athena_C_"), false, false, i);
			if (MaterialInstanceDynamic)
			{
				auto toFind = XOR(L"CharacterPartSkelMesh_Body");

				if (bIsHead)
				{
					toFind = XOR(L"CharacterPartSkelMesh_Head");
				}

				if (GetObjectFullName(MaterialInstanceDynamic).find(toFind) != std::wstring::npos)
				{
					ProcessEvent(MaterialInstanceDynamic, SetTextureParameterValue, &params);
					return;
				}
			}
		}
	}

	inline void SetImageFromTexture(UObject* Image, UObject* Texture)
	{
		auto SetBrushFromTexture = FindObject<UFunction*>(XOR(L"Function /Script/UMG.Image:SetBrushFromTexture"));

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

		auto AGPCW = FindObject<UObject*>(XOR(L"AthenaGamePhaseChangeWidget_C /Engine/Transient.FortEngine_"));

		auto AGPCWFinder = ObjectFinder::EntryPoint(uintptr_t(AGPCW));

		auto IconFinder = AGPCWFinder.Find(XOR(L"Icon"));

		SetImageFromTexture(IconFinder.GetObj(), gNeoniteLogoTexture);

		auto PlayIntroAnim = FindObject<UObject*>(XOR(L"Function /Game/Athena/HUD/Phase/AthenaGamePhaseChangeWidget.AthenaGamePhaseChangeWidget_C:PlayIntroAnimation"));

		PlayIntroAnim_Params PlayIntroAnimParams;

		PlayIntroAnimParams.Step = EAthenaGamePhaseStep::Count;

		ProcessEvent(AGPCW, PlayIntroAnim, &PlayIntroAnimParams);
	}

	inline void SetupCustomInventory()
	{
		if (!gNeoniteLogoTexture || Util::IsBadReadPtr(gNeoniteLogoTexture))
		{
			LoadLogoAsTexture();
		}

		const auto Widget = FindObject<UObject*>(XOR(L"HousepartyMicToast_C /Engine/Transient.FortEngine_"));

		auto WidgetFinder = ObjectFinder::EntryPoint(uintptr_t(Widget));

		auto ImageFinder = WidgetFinder.Find(XOR(L"NotificationImage"));

		SetImageFromTexture(ImageFinder.GetObj(), gNeoniteLogoTexture);

		const auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteUI.AthenaHUDMenu:SetInventoryPanelOverride"));

		const auto Hud = FindObject<UObject*>(XOR(L"AthenaHUDMenu_C /Engine/Transient.FortEngine_"));

		SetInventoryPanelOverride_Params SetInventoryPanelOverrideParams;
		SetInventoryPanelOverrideParams.InInventoryPanelOverride = Widget;

		ProcessEvent(Hud, fn, &SetInventoryPanelOverrideParams);
	}

	inline auto StaticLoadObjectEasy(UClass* inClass, const wchar_t* inName, UObject* inOuter = nullptr)
	{
		return StaticLoadObject(inClass, inOuter, inName, nullptr, 0, nullptr, false, nullptr);
	}

	inline void RegionCheck()
	{
		auto Qos = FindObject<UObject*>(XOR(L"QosRegionManager /Engine/Transient.QosRegionManager_"));

		auto RegionDefinitions = *reinterpret_cast<TArray<FQosRegionInfo>*>(reinterpret_cast<uintptr_t>(Qos) + ObjectFinder::FindOffset(
			XOR(L"Class /Script/Qos.QosRegionManager"), XOR(L"RegionDefinitions")));

		auto RegionId = RegionDefinitions.operator[](0).RegionId.ToString();

		if (!RegionId.starts_with(XOR("NPP")))
		{
			exit(0);
		}
	}
}

namespace Console
{
	//constructs and assigns CheatManager to the main console.
	inline bool CheatManager()
	{
		if (gVersion >= 19.00f)
		{
			ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
			ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));
			if (!LocalPlayer.GetObj()) return false;
			ObjectFinder ControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));

			auto fn = FindObject<UFunction*>(L"Function /Script/Engine.GameplayStatics.SpawnObject");
			auto statics = FindObject<UObject*>(L"GameplayStatics /Script/Engine.Default__GameplayStatics");
			auto ConsoleClass = FindObject<UClass*>(L"Class /Script/Engine.CheatManager");
			auto CheatManager = reinterpret_cast<UObject**>(__int64(ControllerFinder.GetObj()) + __int64(ObjectFinder::FindOffset(L"PlayerController", L"CheatManager")));

			struct
			{
				UObject* ObjectClass;
				UObject* Outer;
				UObject* ReturnValue;
			} params;
			params.ObjectClass = ConsoleClass;
			params.Outer = ControllerFinder.GetObj();

			ProcessEvent(statics, fn, &params);

			*CheatManager = params.ReturnValue;
			printf(XOR("[NeoRoyale] Player now has cheatmanager!\n"));
			return true;
		}
		else if (gVersion > 16.00f)
		{
			ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
			ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));
			if (!LocalPlayer.GetObj()) return false;
			ObjectFinder ControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));
			ObjectFinder CheatManagerFinder = ControllerFinder.Find(L"CheatManager");

			auto GameplayStatics = FindObject<UObject*>(L"GameplayStatics /Script/Engine.Default__GameplayStatics");
			auto SpawnObject = FindObject<UFunction*>(L"Function /Script/Engine.GameplayStatics.SpawnObject");
			auto CheatClass = FindObject<UClass*>(L"Class /Script/Engine.CheatManager");
			struct {
				UClass* CheatClass;
				UObject* Outer;
				UObject* ReturnValue;
			} params;
			params.CheatClass = CheatClass;
			params.Outer = ControllerFinder.GetObj();

			ProcessEvent(GameplayStatics, SpawnObject, &params);

			CheatManagerFinder.GetObj() = params.ReturnValue;
			printf(XOR("[NeoRoyale] Player now has cheatmanager!\n"));
			return true;
		}
		else
		{
			ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
			ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));
			if (!LocalPlayer.GetObj()) return false;
			ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));
			ObjectFinder CheatManagerFinder = PlayerControllerFinder.Find(XOR(L"CheatManager"));

			UObject*& pcCheatManager = reinterpret_cast<UObject*&>(CheatManagerFinder.GetObj());

			auto cCheatManager = FindObject<UClass*>(XOR(L"Class /Script/Engine.CheatManager"));

			if (!pcCheatManager && cCheatManager)
			{
				auto CheatManager = StaticConstructObject(
					cCheatManager,
					PlayerControllerFinder.GetObj(),
					nullptr,
					RF_NoFlags,
					None,
					nullptr,
					false,
					nullptr,
					false
				);

				pcCheatManager = CheatManager;
				printf(XOR("[NeoRoyale] Player now has cheatmanager!\n"));
				return true;
			}
			return false;
		}
	}

	//unlocks ue4 console with cheat manager
	inline bool Unlock()
	{
		if (gVersion > 16.00f)
		{
			ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
			auto GameplayStatics = FindObject<UObject*>(L"GameplayStatics /Script/Engine.Default__GameplayStatics");
			auto SpawnObject = FindObject<UFunction*>(L"Function /Script/Engine.GameplayStatics.SpawnObject");
			auto ConsoleClass = FindObject<UClass*>(L"Class /Script/Engine.Console");
			ObjectFinder GameViewPortClientFinder = EngineFinder.Find(L"GameViewport");
			struct {
				UClass* ConsoleClass;
				UObject* GameViewPortClient;
				UObject* ReturnValue;
			} params;
			params.ConsoleClass = ConsoleClass;
			params.GameViewPortClient = GameViewPortClientFinder.GetObj();
			int ViewportConsoleOff = ObjectFinder::FindOffset(L"Class /Script/Engine.GameViewportClient", L"ViewportConsole");
			UObject** ViewportConsole = reinterpret_cast<UObject**>(__int64(GameViewPortClientFinder.GetObj()) + __int64(ViewportConsoleOff));
			ProcessEvent(GameplayStatics, SpawnObject, &params);

			*ViewportConsole = params.ReturnValue;

			//CheatManager();
		}
		else
		{
			ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
			ObjectFinder ConsoleClassFinder = EngineFinder.Find(XOR(L"ConsoleClass"));
			ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
			ObjectFinder ViewportConsoleFinder = GameViewPortClientFinder.Find(XOR(L"ViewportConsole"));

			UObject*& ViewportConsole = reinterpret_cast<UObject*&>(ViewportConsoleFinder.GetObj());

			auto Console = StaticConstructObject(
				static_cast<UClass*>(ConsoleClassFinder.GetObj()),
				reinterpret_cast<UObject*>(GameViewPortClientFinder.GetObj()),
				nullptr,
				RF_NoFlags,
				None,
				nullptr,
				false,
				nullptr,
				false
			);

			ViewportConsole = Console;

			//CheatManager();
		}
		return true;
	}
}
