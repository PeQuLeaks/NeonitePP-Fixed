#pragma once

class Player
{
public:
	UObject* Controller;
	UObject* Pawn;
	UObject* Mesh;
	UObject* QuickBar;
	UObject* FortInventory;
	UObject* AnimInstance;;
	std::wstring SkinOverride;

	void UpdatePlayerController()
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder LocalPlayer = EngineFinder.Find(XOR(L"GameInstance")).Find(XOR(L"LocalPlayers"));

		ObjectFinder PlayerControllerFinder = LocalPlayer.Find(XOR(L"PlayerController"));
		this->Controller = PlayerControllerFinder.GetObj();
	}

	void UpdateMesh()
	{
		ObjectFinder PawnFinder = ObjectFinder::EntryPoint(uintptr_t(this->Pawn));

		ObjectFinder MeshFinder = PawnFinder.Find(XOR(L"Mesh"));
		this->Mesh = MeshFinder.GetObj();
	}

	void UpdateAnimInstance()
	{
		if (!this->Mesh || !Util::IsBadReadPtr(this->Mesh))
		{
			this->UpdateMesh();
		}

		UFunction* FUNC_GetAnimInstance;
		if (gVersion > 16.00f)
			FUNC_GetAnimInstance = FindObject<UFunction*>(XOR(L"Function /Script/Engine.SkeletalMeshComponent.GetAnimInstance"));
		else 
			FUNC_GetAnimInstance = FindObject<UFunction*>(XOR(L"Function /Script/Engine.SkeletalMeshComponent:GetAnimInstance"));

		USkeletalMeshComponent_GetAnimInstance_Params GetAnimInstance_Params;

		ProcessEvent(this->Mesh, FUNC_GetAnimInstance, &GetAnimInstance_Params);

		this->AnimInstance = GetAnimInstance_Params.ReturnValue;
	}

	void Authorize()
	{
		const auto LocalRole = reinterpret_cast<TEnumAsByte<ENetRole>*>(reinterpret_cast<uintptr_t>(this->Pawn) + ObjectFinder::FindOffset(XOR(L"Class /Script/Engine.Actor"), XOR(L"Role")));

		*LocalRole = ENetRole::ROLE_Authority;

		const auto RemoteRole = reinterpret_cast<TEnumAsByte<ENetRole>*>(reinterpret_cast<uintptr_t>(this->Pawn) + ObjectFinder::FindOffset(XOR(L"Class /Script/Engine.Actor"), XOR(L"RemoteRole")));

		*RemoteRole = ENetRole::ROLE_Authority;
	}

	void Respawn()
	{
		if (this->Pawn)
		{
			this->Summon(XOR(L"PlayerPawn_Athena_C"));
			this->Pawn = ObjectFinder::FindActor(XOR(L"PlayerPawn_Athena_C"));

			if (this->Pawn)
			{
				this->Possess();
				this->ShowSkin();
				this->UpdateAnimInstance();
				this->UpdatePlayerController();
			}
		}
	}

	void TeleportTo(FVector Location, FRotator Rotation)
	{
		UFunction* FUNC_K2_TeleportTo;
		if (gVersion > 16.00f)
			FUNC_K2_TeleportTo = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor.K2_TeleportTo"));
		else
			FUNC_K2_TeleportTo = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor:K2_TeleportTo"));

		AActor_K2_TeleportTo_Params K2_TeleportTo_Params;
		K2_TeleportTo_Params.DestLocation = Location;
		K2_TeleportTo_Params.DestRotation = Rotation;

		ProcessEvent(this->Pawn, FUNC_K2_TeleportTo, &K2_TeleportTo_Params);
	}

	void Summon(const wchar_t* ClassToSummon)
	{
		if (!this->Controller || Util::IsBadReadPtr(this->Controller))
		{
			UpdatePlayerController();
		}

		ObjectFinder PlayerControllerFinder = ObjectFinder::EntryPoint(uintptr_t(this->Controller));

		ObjectFinder CheatManagerFinder = PlayerControllerFinder.Find(XOR(L"CheatManager"));

		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.CheatManager.Summon"));
		else 
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.CheatManager:Summon"));

		const FString ClassName = ClassToSummon;

		UCheatManager_Summon_Params params;
		params.ClassName = ClassName;

		ProcessEvent(CheatManagerFinder.GetObj(), fn, &params);
		printf("\n[NeoRoyale] %ls was summoned!\n", ClassToSummon);
	}

	void Possess()
	{
		if (!this->Controller || Util::IsBadReadPtr(this->Controller))
		{
			UpdatePlayerController();
		}

		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Controller.Possess"));
		else 
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Controller:Possess"));

		AController_Possess_Params params;
		params.InPawn = this->Pawn;

		ProcessEvent(this->Controller, fn, &params);
		printf(XOR("\n[NeoRoyale] PlayerPawn was possessed!\n"));
	}

	auto StartSkydiving(float height)
	{
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawnAthena.TeleportToSkyDive"));
		else
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawnAthena:TeleportToSkyDive"));

		AFortPlayerPawnAthena_TeleportToSkyDive_Params params;
		params.HeightAboveGround = height;

		ProcessEvent(this->Pawn, fn, &params);
		printf("\nSkydiving!, Redeploying at %fm.\n", height);
	}

	auto IsJumpProvidingForce()
	{
		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Character:IsJumpProvidingForce"));

		ACharacter_IsJumpProvidingForce_Params params;

		ProcessEvent(this->Pawn, fn, &params);

		return params.ReturnValue;
	}

	auto StopMontageIfEmote()
	{
		{
			if (!this->Mesh || !this->AnimInstance || !Util::IsBadReadPtr(this->Mesh) || !Util::IsBadReadPtr(this->AnimInstance))
			{
				this->UpdateMesh();
				this->UpdateAnimInstance();
			}

			UFunction* FUNC_GetCurrentActiveMontage;
			if (gVersion > 16.00f)
				FUNC_GetCurrentActiveMontage = FindObject<UFunction*>(XOR(L"Function /Script/Engine.AnimInstance.GetCurrentActiveMontage"));
			else
				FUNC_GetCurrentActiveMontage = FindObject<UFunction*>(XOR(L"Function /Script/Engine.AnimInstance:GetCurrentActiveMontage"));

			UAnimInstance_GetCurrentActiveMontage_Params GetCurrentActiveMontage_Params;

			ProcessEvent(this->AnimInstance, FUNC_GetCurrentActiveMontage, &GetCurrentActiveMontage_Params);

			auto CurrentPlayingMontage = GetCurrentActiveMontage_Params.ReturnValue;

			if (CurrentPlayingMontage && CurrentPlayingMontage->GetName().starts_with(XOR(L"Emote_")))
			{
				UFunction* FUNC_Montage_Stop;
				if (gVersion > 16.00f)
					FUNC_Montage_Stop = FindObject<UFunction*>(XOR(L"Function /Script/Engine.AnimInstance.Montage_Stop"));
				else
					FUNC_Montage_Stop = FindObject<UFunction*>(XOR(L"Function /Script/Engine.AnimInstance:Montage_Stop"));

				UAnimInstance_Montage_Stop_Params Montage_Stop_Params;
				Montage_Stop_Params.InBlendOutTime = 0;
				Montage_Stop_Params.Montage = CurrentPlayingMontage;

				ProcessEvent(this->AnimInstance, FUNC_Montage_Stop, &Montage_Stop_Params);
			}
		}
	}

	auto ServerSetClientHasFinishedLoading()
	{
		UFunction* ServerSetClientHasFinishedLoading;
		if (gVersion > 16.00f)
			ServerSetClientHasFinishedLoading = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController.ServerSetClientHasFinishedLoading"));
		else
			ServerSetClientHasFinishedLoading = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController:ServerSetClientHasFinishedLoading"));

		bool HasFinishedLoading = true;

		ProcessEvent(this->Controller, ServerSetClientHasFinishedLoading, &HasFinishedLoading);

		auto bHasServerFinishedLoading = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this->Controller) + __int64(ObjectFinder::FindOffset(L"FortPlayerController", L"bHasServerFinishedLoading")));
		*bHasServerFinishedLoading = true;

		UFunction* OnRep_bHasServerFinishedLoading;
		if (gVersion > 16.00f)
			OnRep_bHasServerFinishedLoading = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController.OnRep_bHasServerFinishedLoading"));
		else
			OnRep_bHasServerFinishedLoading = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController:OnRep_bHasServerFinishedLoading"));

		ProcessEvent(this->Controller, OnRep_bHasServerFinishedLoading, nullptr);
	}

	//DEPRECATED, use abilities 
	auto IsSkydiving()
	{
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn.IsSkydiving"));
		else 
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn:IsSkydiving"));

		ACharacter_IsSkydiving_Params params;

		ProcessEvent(this->Pawn, fn, &params);

		return params.ReturnValue;
	}

	//DEPRECATED, use abilities 
	auto IsParachuteOpen()
	{
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn.IsParachuteOpen"));
		else 
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn:IsParachuteOpen"));

		ACharacter_IsParachuteOpen_Params params;

		ProcessEvent(this->Pawn, fn, &params);

		return params.ReturnValue;
	}

	//DEPRECATED, use abilities 
	auto IsParachuteForcedOpen()
	{
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn.IsParachuteForcedOpen"));
		else 
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn:IsParachuteForcedOpen"));

		ACharacter_IsParachuteForcedOpen_Params params;

		ProcessEvent(this->Pawn, fn, &params);

		return params.ReturnValue;
	}

	//DEPRECATED, use abilities 
	auto Jump()
	{
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Character.Jump"));
		else 
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Character:Jump"));

		Empty_Params params;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto SetSkeletalMesh(const wchar_t* meshname)
	{
		if (!this->Mesh || !Util::IsBadReadPtr(this->Mesh))
		{
			this->UpdateMesh();
		}

		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.SkinnedMeshComponent:SetSkeletalMesh"));

		std::wstring MeshName = meshname;

		std::wstring name = MeshName + L"." + MeshName;

		auto Mesh = FindObject<UObject*>(name.c_str(), true);

		if (Mesh)
		{
			USkinnedMeshComponent_SetSkeletalMesh_Params params;
			params.NewMesh = Mesh;
			params.bReinitPose = false;

			ProcessEvent(this->Mesh, fn, &params);
		}
	}

	//we dont use this
	void ApplyOverride()
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));
		ObjectFinder PawnFinder = ObjectFinder::EntryPoint(uintptr_t(this->Pawn));
		ObjectFinder PlayerStateFinder = PawnFinder.Find(XOR(L"PlayerState"));

		auto Hero = FindObject<UObject*>(XOR(L"FortHero /Engine/Transient.FortHero_"));

		/*
		 * CharacterParts Array Indexes (typeof UCustomCharacterPart)
		 * 0 - Body (e.g: CP_031_Athena_Body_Retro)
		 * 1 - Head (e.g: M_Med_HIS_Diego_Head_01)
		 * 2 - Hat (e.g: M_Med_HIS_Diego_Hat_02)
		 * 3 - Charm (e.g: M_Commando_UR_01_Grenades)
		 */

		auto CharacterParts = reinterpret_cast<TArray<UObject*>*>(reinterpret_cast<uintptr_t>(Hero) + ObjectFinder::FindOffset(
			XOR(L"Class /Script/FortniteGame.FortHero"), XOR(L"CharacterParts")));


		if (SkinOverride == L"Thanos")
		{
			CharacterParts->operator[](1) = FindObject<UObject*>(XOR(L"CustomCharacterPart /Game/Athena/Heroes/Meshes/Heads/Dev_TestAsset_Head_M_XL.Dev_TestAsset_Head_M_XL"));
			CharacterParts->operator[](0) = FindObject<UObject*>(XOR(L"CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/Dev_TestAsset_Body_M_XL.Dev_TestAsset_Body_M_XL"));
		}
		else if (SkinOverride == L"Chituari")
		{
			CharacterParts->operator[](1) = FindObject<UObject*>(
				XOR(L"CustomCharacterPart /Game/Characters/CharacterParts/Male/Medium/Heads/CP_Athena_Head_M_AshtonMilo.CP_Athena_Head_M_AshtonMilo"));
			CharacterParts->operator[](0) = FindObject<UObject*>(XOR(L"CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/CP_Athena_Body_M_AshtonMilo.CP_Athena_Body_M_AshtonMilo"));
		}
#ifndef PROD
		else return;
#else
		else
		{
			CharacterParts->operator[](1) = FindObject<UObject*>(XOR(L"CustomCharacterPart /Game/Athena/Heroes/Meshes/Heads/Dev_TestAsset_Head_M_XL.Dev_TestAsset_Head_M_XL"));
			CharacterParts->operator[](0) = FindObject<UObject*>(XOR(L"CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/Dev_TestAsset_Body_M_XL.Dev_TestAsset_Body_M_XL"));
		}

#endif

		auto KismetLib = FindObject<UObject*>(XOR(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"));
		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortKismetLibrary:ApplyCharacterCosmetics"));

		UFortKismetLibrary_ApplyCharacterCosmetics_Params params;
		params.WorldContextObject = WorldFinder.GetObj();
		params.CharacterParts = *CharacterParts;
		params.PlayerState = PlayerStateFinder.GetObj();

		ProcessEvent(KismetLib, fn, &params);

		printf(XOR("\n[NeoRoyale] Character Part overrides were applied.\n"));
	}

	void ShowSkin()
	{
		ObjectFinder PawnFinder = ObjectFinder::EntryPoint(uintptr_t(this->Pawn));
		ObjectFinder PlayerStateFinder = PawnFinder.Find(XOR(L"PlayerState"));

		auto KismetLib = FindObject<UObject*>(XOR(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"));

		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortKismetLibrary.UpdatePlayerCustomCharacterPartsVisualization"));
		else 
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortKismetLibrary:UpdatePlayerCustomCharacterPartsVisualization"));

		UFortKismetLibrary_UpdatePlayerCustomCharacterPartsVisualization_Params params;
		params.PlayerState = PlayerStateFinder.GetObj();

		ProcessEvent(KismetLib, fn, &params);
		printf(XOR("\n[NeoRoyale] Character Parts should be visible now!.\n"));
	}

	auto PickupActor(UObject* PlacementDecoItemDefinition, UObject* PickupTarget = nullptr)
	{
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn.PickUpActor"));
		else
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn:PickUpActor"));

		PickupActor_Params params;
		params.PickupTarget = PickupTarget;
		params.PlacementDecoItemDefinition = PlacementDecoItemDefinition;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto EquipWeapon(const wchar_t* weaponname, int guid = rand())
	{
		FGuid GUID;
		GUID.A = guid;
		GUID.B = guid;
		GUID.C = guid;
		GUID.D = guid;

		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn.EquipWeaponDefinition"));

		std::wstring WeaponName = weaponname;

		std::wstring name = WeaponName + L"." + WeaponName;

		auto WeaponData = FindObject<UObject*>(name.c_str(), true);

		if (WeaponData && !Util::IsBadReadPtr(WeaponData))
		{
			std::wstring objectName = WeaponData->GetFullName();

			if (objectName.starts_with(L"FortWeapon") || objectName.starts_with(L"AthenaGadget") || objectName.starts_with(L"FortPlayset"))
			{
				if (objectName.starts_with(L"AthenaGadget"))
				{
					auto FUN_weapondef = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortGadgetItemDefinition.GetWeaponItemDefinition"));

					UFortGadgetItemDefinition_GetWeaponItemDefinition_Params prm_ReturnValue;

					ProcessEvent(WeaponData, FUN_weapondef, &prm_ReturnValue);

					if (prm_ReturnValue.ReturnValue && !Util::IsBadReadPtr(prm_ReturnValue.ReturnValue))
					{
						WeaponData = prm_ReturnValue.ReturnValue;
					}
				}

				//weapon found equip it
				AFortPawn_EquipWeaponDefinition_Params params;
				params.WeaponData = WeaponData;
				params.ItemEntryGuid = GUID;
				params.Durability = 3232.0000;

				//ProcessEvent(this->Pawn, fn, &params);
			}
			else if (objectName.starts_with(L"FortTrap"))
			{
				this->PickupActor(WeaponData);
			}
		}
		else
		{
			MessageBoxA(nullptr, XOR("This item doesn't exist!"), XOR("Carbon"), MB_OK);
		}
	}

	auto Emote(UObject* EmoteDef)
	{
		//We grab the mesh from the pawn then use it to get the animation instance
		if (!this->Mesh || !this->AnimInstance || !Util::IsBadReadPtr(this->Mesh) || !Util::IsBadReadPtr(this->AnimInstance))
		{
			this->UpdateMesh();
			this->UpdateAnimInstance();
		}

		if (EmoteDef && !Util::IsBadReadPtr(EmoteDef))
		{
			//Emote Def is valid, now we grab the animation montage
			UFunction* FUNC_GetAnimationHardReference;
			UFunction* FUNC_Montage_Play;
			if(gVersion > 16.00f)
				FUNC_GetAnimationHardReference = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortMontageItemDefinitionBase.GetAnimationHardReference"));
			else 
				FUNC_GetAnimationHardReference = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortMontageItemDefinitionBase:GetAnimationHardReference"));

			UFortMontageItemDefinitionBase_GetAnimationHardReference_Params GetAnimationHardReference_Params;
			GetAnimationHardReference_Params.BodyType = EFortCustomBodyType::All;
			GetAnimationHardReference_Params.Gender = EFortCustomGender::Both;
			GetAnimationHardReference_Params.PawnContext = this->Pawn;

			ProcessEvent(EmoteDef, FUNC_GetAnimationHardReference, &GetAnimationHardReference_Params);

			auto Animation = GetAnimationHardReference_Params.ReturnValue;

			//got the animation, now play :JAM:
			if (gVersion > 16.00f)
				FUNC_Montage_Play = FindObject<UFunction*>(XOR(L"Function /Script/Engine.AnimInstance.Montage_Play"));
			else
				FUNC_Montage_Play = FindObject<UFunction*>(XOR(L"Function /Script/Engine.AnimInstance:Montage_Play"));

			UAnimInstance_Montage_Play_Params params;
			params.MontageToPlay = Animation;
			params.InPlayRate = 1;
			params.ReturnValueType = EMontagePlayReturnType::Duration;
			params.InTimeToStartMontageAt = 0;
			params.bStopAllMontages = true;

			ProcessEvent(this->AnimInstance, FUNC_Montage_Play, &params);
		}
		else
		{
			MessageBoxA(nullptr, XOR("This item doesn't exist!"), XOR("Cranium"), MB_OK);
		}
	}

	auto GetLocation() -> FVector
	{
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor.K2_GetActorLocation"));
		else
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor:K2_GetActorLocation"));

		AActor_K2_GetActorLocation_Params params;

		ProcessEvent(this->Pawn, fn, &params);

		return params.ReturnValue;
	}

	auto SetMovementMode(TEnumAsByte<EMovementMode> NewMode, unsigned char CustomMode)
	{
		ObjectFinder PawnFinder = ObjectFinder::EntryPoint(uintptr_t(this->Pawn));

		ObjectFinder CharMovementFinder = PawnFinder.Find(XOR(L"CharacterMovement"));

		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.CharacterMovementComponent.SetMovementMode"));
		}
		else {
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.CharacterMovementComponent:SetMovementMode"));
		}

		UCharacterMovementComponent_SetMovementMode_Params params;

		params.NewMovementMode = NewMode;
		params.NewCustomMode = CustomMode;

		ProcessEvent(CharMovementFinder.GetObj(), fn, &params);
	}

	auto Fly(bool bIsFlying)
	{
		TEnumAsByte<EMovementMode> NewMode;

		if (!bIsFlying) NewMode = EMovementMode::MOVE_Flying;
		else NewMode = EMovementMode::MOVE_Walking;

		SetMovementMode(NewMode, 0);
	}

	auto SetPawnGravityScale(float GravityScaleInput)
	{
		ObjectFinder PawnFinder = ObjectFinder::EntryPoint(uintptr_t(this->Pawn));

		ObjectFinder CharMovementFinder = PawnFinder.Find(XOR(L"CharacterMovement"));

		auto GravityScaleOffset = ObjectFinder::FindOffset(XOR(L"Class /Script/Engine.CharacterMovementComponent"), XOR(L"GravityScale"));

		float* GravityScale = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(CharMovementFinder.GetObj()) + GravityScaleOffset);

		*GravityScale = GravityScaleInput;
	}

	auto SetHealth(float SetHealthInput)
	{
		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn:SetHealth"));

		AFortPawn_SetHealth_Params params;

		params.NewHealthVal = SetHealthInput;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto SetShield(float SetShieldInput)
	{
		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn:SetShield"));

		AFortPawn_SetShield_Params params;

		params.NewShieldValue = SetShieldInput;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto SetMaxHealth(float SetMaxHealthInput)
	{
		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn:SetMaxHealth"));

		AFortPawn_SetMaxHealth_Params params;

		params.NewHealthVal = SetMaxHealthInput;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto SetMaxShield(float SetMaxShieldInput)
	{
		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn:SetMaxShield"));

		AFortPawn_SetMaxShield_Params params;

		params.NewValue = SetMaxShieldInput;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto SetMovementSpeed(float SetMovementSpeedInput)
	{
		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn.SetMovementSpeed"));
		}
		else {
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn:SetMovementSpeed"));
		}
		AFortPawn_SetMovementSpeedMultiplier_Params params;

		params.NewMovementSpeedVal = SetMovementSpeedInput;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto ToggleInfiniteAmmo()
	{
		if (!this->Controller || Util::IsBadReadPtr(this->Controller))
		{
			UpdatePlayerController();
		}

		auto bEnableVoiceChatPTTOffset = ObjectFinder::FindOffset(XOR(L"Class /Script/FortniteGame.FortPlayerController"), XOR(L"bEnableVoiceChatPTT"));

		// TECHNICAL EXPLINATION: (kemo) We are doing this because InfiniteAmmo bool and some other bools live in the same offset
		// the offset has 8 bits (a bitfield), bools are only one bit as it's only 0\1 so we have a struct with 8 bools (1 byte) to be able to edit that specific bool
		auto PlayerControllerBools = reinterpret_cast<PlayerControllerBoolsForInfiniteAmmo*>(reinterpret_cast<uintptr_t>(this->Controller) + bEnableVoiceChatPTTOffset);

		PlayerControllerBools->bInfiniteAmmo = true;
		PlayerControllerBools->bInfiniteMagazine = true;

		printf(XOR("\n[NeoRoyale] You should have infinite ammo now!\n"));
	}

	auto ExecuteConsoleCommand(const wchar_t* command)
	{
		ObjectFinder EngineFinder = ObjectFinder::EntryPoint(uintptr_t(GEngine));
		ObjectFinder GameViewPortClientFinder = EngineFinder.Find(XOR(L"GameViewport"));
		ObjectFinder WorldFinder = GameViewPortClientFinder.Find(XOR(L"World"));

		auto KismetSysLib = FindObject<UObject*>(XOR(L"KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary"));
		UFunction* fn;
		if (gVersion > 16.00f)
		{
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand"));
		}
		else {
			fn = FindObject<UFunction*>(XOR(L"Function /Script/Engine.KismetSystemLibrary:ExecuteConsoleCommand"));
		}

		UKismetSystemLibrary_ExecuteConsoleCommand_Params params;
		params.WorldContextObject = WorldFinder.GetObj();
		params.Command = command;
		params.SpecificPlayer = this->Controller;

		ProcessEvent(KismetSysLib, fn, &params);
		printf(XOR("\n[NeoRoyale] Executed a console command!\n"));
	}

	void SetCameraMode(const wchar_t* NewMode)
	{
		UpdatePlayerController();
		UFunction* ClientSetCameraMode;
		if (gVersion > 16.00f)
			ClientSetCameraMode = FindObject<UFunction*>(XOR(L"Function /Script/Engine.PlayerController.ClientSetCameraMode"));
		else
			ClientSetCameraMode = FindObject<UFunction*>(XOR(L"Function /Script/Engine.PlayerController:ClientSetCameraMode"));

		APlayerController_ClientSetCameraMode_Params ClientSetCameraMode_Params;
		ClientSetCameraMode_Params.NewCamMode = KismetFunctions::GetFName(NewMode);

		ProcessEvent(this->Controller, ClientSetCameraMode, &ClientSetCameraMode_Params);
	}
	
	void HideHead(bool bIsHidden)
	{
		auto Mesh = FindObject<UObject*>(XOR(L"SkeletalMeshComponentBudgeted /Game/Athena/Apollo/Maps/Apollo_Terrain.Apollo_Terrain:PersistentLevel.PlayerPawn_Athena_C_"));

		UFunction* SetOwnerNoSee;
		if (gVersion > 16.00f)
			SetOwnerNoSee = FindObject<UFunction*>(XOR(L"Function /Script/Engine.PrimitiveComponent.SetOwnerNoSee"));
		else
			SetOwnerNoSee = FindObject<UFunction*>(XOR(L"Function /Script/Engine.PrimitiveComponent:SetOwnerNoSee"));

		UPrimitiveComponent_SetOwnerNoSee_Params SetOwnerNoSee_Params;
		SetOwnerNoSee_Params.bNewOwnerNoSee = bIsHidden;

		ProcessEvent(Mesh, SetOwnerNoSee, &SetOwnerNoSee_Params);
	}
	
	auto Skydive()
	{
		/*
		if (this->IsSkydiving())
		{
			auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn:EndSkydiving"));

			ProcessEvent(this->Pawn, fn, nullptr);
		}*/

		/*auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn:BeginSkydiving"));

		AFortPlayerPawn_BeginSkydiving_Params params;
		params.bFromBus = true;

		ProcessEvent(this->Pawn, fn, &params);*/

		this->SetMovementMode(EMovementMode::MOVE_Custom, 4);
	}

	auto ForceOpenParachute()
	{
		/*
		auto fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn:BP_ForceOpenParachute"));

		Empty_Params params;

		ProcessEvent(this->Pawn, fn, &params);
		*/
		this->SetMovementMode(EMovementMode::MOVE_Custom, 3);
	}

	auto IsInAircraft()
	{
		if (!this->Controller || Util::IsBadReadPtr(this->Controller))
		{
			UpdatePlayerController();
		}

		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController.IsInAircraft"));
		else
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController:IsInAircraft"));

		ACharacter_IsInAircraft_Params params;

		ProcessEvent(this->Controller, fn, &params);
		return params.ReturnValue;
	}


	void ShowPickaxe()
	{
		if (!this->Controller || Util::IsBadReadPtr(this->Controller))
		{
			UpdatePlayerController();
		}

		auto CosmeticLoadoutPCOffset = ObjectFinder::FindOffset(XOR(L"Class /Script/FortniteGame.FortPlayerController"), XOR(L"CosmeticLoadoutPC"));

		auto CosmeticLoadoutPC = reinterpret_cast<FFortAthenaLoadout*>(reinterpret_cast<uintptr_t>(this->Controller) + CosmeticLoadoutPCOffset);

		if (!Util::IsBadReadPtr(CosmeticLoadoutPC))
		{
			auto PickaxeFinder = ObjectFinder::EntryPoint(uintptr_t(CosmeticLoadoutPC->Pickaxe));

			auto WeaponDefFinder = PickaxeFinder.Find(XOR(L"WeaponDefinition"));

			auto Weapon = GetObjectFirstName(WeaponDefFinder.GetObj());

			this->EquipWeapon(Weapon.c_str());

			printf(XOR("\n[NeoRoyale] Equipped the pickaxe!\n"));
		}
	}

	void BP_ApplyGameplayEffectToSelf(UObject* AbilitySystemComponent, UObject* GameplayEffectClass)
	{
		UObject* BP_ApplyGameplayEffectToSelf;
		if (gVersion > 16.00f)
			BP_ApplyGameplayEffectToSelf = FindObject<UFunction*>(L"Function /Script/GameplayAbilities.AbilitySystemComponent.BP_ApplyGameplayEffectToSelf");
		else
			BP_ApplyGameplayEffectToSelf = FindObject<UFunction*>(L"Function /Script/GameplayAbilities.AbilitySystemComponent:BP_ApplyGameplayEffectToSelf");

		struct
		{
			UObject* GameplayEffectClass;
			float Level;
			FGameplayEffectContextHandle EffectContext;
			FActiveGameplayEffectHandle ret;
		} Params;

		Params.EffectContext = FGameplayEffectContextHandle();
		Params.GameplayEffectClass = GameplayEffectClass;
		Params.Level = 1.0;

		ProcessEvent(AbilitySystemComponent, BP_ApplyGameplayEffectToSelf, &Params);
	}

	void GrantAbility(UObject* GameplayAbilityClass)
	{
		if (!GameplayAbilityClass) //Not all abilities will exist on all versions, this will check if the abilities actually exist and prevent crashes
			return; 

		UObject** AbilitySystemComponent = reinterpret_cast<UObject**>(__int64(this->Pawn) + __int64(ObjectFinder::FindOffset(L"FortPawn", L"AbilitySystemComponent")));
		static UObject* DefaultGameplayEffect = FindObject<UObject*>(L"GE_Athena_PurpleStuff_C /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff.Default__GE_Athena_PurpleStuff_C");
		if (!DefaultGameplayEffect)
		{
			DefaultGameplayEffect = FindObject<UObject*>(L"GE_Athena_PurpleStuff_Health_C /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff_Health.Default__GE_Athena_PurpleStuff_Health_C");
		}

		TArray<struct FGameplayAbilitySpecDef>* GrantedAbilities = reinterpret_cast<TArray<struct FGameplayAbilitySpecDef>*>(__int64(DefaultGameplayEffect) + __int64(ObjectFinder::FindOffset(L"GameplayEffect", L"GrantedAbilities")));

		GrantedAbilities->operator[](0).Ability = GameplayAbilityClass;

		*reinterpret_cast<EGameplayEffectDurationType*>(__int64(DefaultGameplayEffect) + __int64(ObjectFinder::FindOffset(L"GameplayEffect", L"DurationPolicy"))) = EGameplayEffectDurationType::Infinite;

		static auto GameplayEffectClass = FindObject<UClass*>(XOR(L"BlueprintGeneratedClass /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff.GE_Athena_PurpleStuff_C"));
		if (!GameplayEffectClass)
		{
			GameplayEffectClass = FindObject<UClass*>(XOR(L"BlueprintGeneratedClass /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff_Health.GE_Athena_PurpleStuff_Health_C"));
		}
		BP_ApplyGameplayEffectToSelf(*AbilitySystemComponent, GameplayEffectClass);
	}

	//Give the player all the abilities
	void SetupAbilities()
	{
		GrantAbility(FindObject<UObject*>(L"Class /Script/FortniteGame.FortGameplayAbility_Sprint"));
		GrantAbility(FindObject<UObject*>(L"Class /Script/FortniteGame.FortGameplayAbility_Sprint"));
		GrantAbility(FindObject<UObject*>(L"Class /Script/FortniteGame.FortGameplayAbility_ZiplineSmashBase"));
		GrantAbility(FindObject<UObject*>(L"Class /Script/FortniteGame.FortGameplayAbility_Crouch"));
		GrantAbility(FindObject<UObject*>(L"BlueprintGeneratedClass /Game/Athena/DrivableVehicles/GA_AthenaEnterVehicle.GA_AthenaEnterVehicle_C"));
		GrantAbility(FindObject<UObject*>(L"BlueprintGeneratedClass /Game/Athena/Items/EnvironmentalItems/HidingProps/GA_Athena_HidingProp_JumpOut.GA_Athena_HidingProp_JumpOut_C"));
		GrantAbility(FindObject<UObject*>(L"BlueprintGeneratedClass /Game/Athena/DrivableVehicles/GA_AthenaExitVehicle.GA_AthenaExitVehicle_C"));
		GrantAbility(FindObject<UObject*>(L"BlueprintGeneratedClass /Game/Athena/Items/EnvironmentalItems/HidingProps/GA_Athena_HidingProp_Hide.GA_Athena_HidingProp_Hide_C"));
		GrantAbility(FindObject<UObject*>(L"Class /Script/FortniteGame.FortGameplayAbility_CarryPlayer"));
		GrantAbility(FindObject<UObject*>(L"Class /Script/FortniteGame.FortGameplayAbility_ZiplineSmashBase"));
		GrantAbility(FindObject<UObject*>(L"Class /Script/FortniteGame.FortGameplayAbility_OstrichJump"));
		GrantAbility(FindObject<UObject*>(L"BlueprintGeneratedClass /Game/Abilities/Player/Sliding/GA_Athena_Player_Slide.GA_Athena_Player_Slide_C"));
		GrantAbility(FindObject<UObject*>(L"BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractUse.GA_DefaultPlayer_InteractUse_C"));
		GrantAbility(FindObject<UObject*>(L"BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractSearch.GA_DefaultPlayer_InteractSearch_C"));
	}

	UObject* GetPickaxeDef()
	{
		auto CosmeticLoadoutPC = reinterpret_cast<FFortAthenaLoadout*>((uintptr_t)Controller + __int64(ObjectFinder::FindOffset(L"FortPlayerController", L"CosmeticLoadoutPC")));
		auto Pickaxe = CosmeticLoadoutPC->Pickaxe;
		auto PickaxeDef = *reinterpret_cast<UObject**>((uintptr_t)Pickaxe + __int64(ObjectFinder::FindOffset(L"AthenaPickaxeItemDefinition", L"WeaponDefinition")));
		return PickaxeDef;
	}

	void UpdateInventory()
	{
		UFunction* HandleWorldInventoryLocalUpdate;
		UFunction* HandleInventoryLocalUpdate;
		UFunction* ClientForceUpdateQuickbar;

		if (gVersion > 16.00f)
		{
			HandleWorldInventoryLocalUpdate = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController.HandleWorldInventoryLocalUpdate"));
			HandleInventoryLocalUpdate = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortInventory.HandleInventoryLocalUpdate"));
			ClientForceUpdateQuickbar = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController.ClientForceUpdateQuickbar"));
		}
		else
		{
			HandleWorldInventoryLocalUpdate = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController:HandleWorldInventoryLocalUpdate"));
			HandleInventoryLocalUpdate = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortInventory:HandleInventoryLocalUpdate"));
			ClientForceUpdateQuickbar = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController:ClientForceUpdateQuickbar"));
		}

		ProcessEvent(Controller, HandleWorldInventoryLocalUpdate, nullptr);
		ProcessEvent(FortInventory, HandleInventoryLocalUpdate, nullptr);

		auto PrimaryQuickbar = EFortQuickBars::Primary;
		auto SecondaryQuickbar = EFortQuickBars::Secondary;

		ProcessEvent(Controller, ClientForceUpdateQuickbar, &PrimaryQuickbar);
		ProcessEvent(Controller, ClientForceUpdateQuickbar, &SecondaryQuickbar);
	}

	void AddItemToInventory(UObject* WeaponID, int Count, bool bAddToQuickBars = false, EFortQuickBars QuickBarType = EFortQuickBars::Max_None, int32_t slot = 0)
	{
		if (WeaponID)
		{
			UObject* ItemInstance = CreateItem(WeaponID, Count);

			if (ItemInstance)
			{
				auto ItemEntry = reinterpret_cast<FFortItemEntry*>(reinterpret_cast<uintptr_t>(ItemInstance) + __int64(ObjectFinder::FindOffset(L"FortWorldItem", L"ItemEntry")));
				reinterpret_cast<TArray<FFortItemEntry>*>(__int64(FortInventory) + static_cast<__int64>(__int64(ObjectFinder::FindOffset(L"FortInventory", L"Inventory"))) + static_cast<__int64>(ObjectFinder::FindOffset(L"FortItemList", L"ReplicatedEntries")))->Add(*ItemEntry);
			}
			UpdateInventory();
			OnRep_QuickbarEquippedItems();
		}
	}

	UObject* CreateTemporaryItemInstanceBP(UObject* ItemDefinition, int Count, int Level)
	{
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortItemDefinition.CreateTemporaryItemInstanceBP"));
		else
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortItemDefinition:CreateTemporaryItemInstanceBP"));

		struct
		{
			int Count;
			int Level;
			UObject* ReturnValue;
		} Params;

		Params.Count = Count;
		Params.Level = Level;

		ProcessEvent(ItemDefinition, fn, &Params);

		return Params.ReturnValue;
	}

	UObject* CreateItem(UObject* ItemDefinition, int Count)
	{
		UObject* TemporaryItemInstance = CreateTemporaryItemInstanceBP(ItemDefinition, Count, 1);

		if (TemporaryItemInstance)
		{
			SetOwningControllerForTemporaryItem(TemporaryItemInstance, Controller);
		}

		int* CurrentCount = reinterpret_cast<int*>(__int64(TemporaryItemInstance) + static_cast<__int64>(ObjectFinder::FindOffset(L"FortWorldItem", L"ItemEntry")) + static_cast<__int64>(ObjectFinder::FindOffset(L"FortItemEntry", L"Count")));
		*CurrentCount = Count;

		return TemporaryItemInstance;
	}

	void SetOwningControllerForTemporaryItem(UObject* Item, UObject* Controller)
	{
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortItem.SetOwningControllerForTemporaryItem"));
		else
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortItem:SetOwningControllerForTemporaryItem"));

		ProcessEvent(Item, fn, &Controller);
	}

	void OnRep_QuickbarEquippedItems()
	{
		auto PlayerState = *reinterpret_cast<UObject**>((uintptr_t)Pawn + __int64(ObjectFinder::FindOffset(L"Pawn", L"PlayerState")));
		UFunction* fn;
		if (gVersion > 16.00f)
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerStateZone.OnRep_QuickbarEquippedItems"));
		else
			fn = FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerStateZone:OnRep_QuickbarEquippedItems"));

		ProcessEvent(PlayerState, fn, nullptr);
	}

	//give player items, yes very cool
	void SetupInventory()
	{
		if (!this->Controller || Util::IsBadReadPtr(this->Controller))
		{
			UpdatePlayerController();
		}
		FortInventory = reinterpret_cast<InventoryPointer*>((uintptr_t)this->Controller + __int64(ObjectFinder::FindOffset(L"FortPlayerController", L"WorldInventory")))->Inventory;
		QuickBar = reinterpret_cast<QuickBarPointer*>((uintptr_t)this->Controller + __int64(ObjectFinder::FindOffset(L"FortPlayerController", L"ClientQuickBars")))->QuickBar;

		AddItemToInventory(GetPickaxeDef(), 1, true, EFortQuickBars::Primary, 0);
		if (gVersion > 17.70f) //crashes on sub 18, dk why
		{
			AddItemToInventory(FindObject<UObject*>(L"FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"), 1);
			AddItemToInventory(FindObject<UObject*>(L"FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"), 1);
			AddItemToInventory(FindObject<UObject*>(L"FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"), 1);
			AddItemToInventory(FindObject<UObject*>(L"FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"), 1);
			AddItemToInventory(FindObject<UObject*>(L"FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"), 999);
			AddItemToInventory(FindObject<UObject*>(L"FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"), 999);
			AddItemToInventory(FindObject<UObject*>(L"FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"), 999);
			AddItemToInventory(FindObject<UObject*>(L"FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataEnergyCell.AmmoDataEnergyCell"), 999);
			AddItemToInventory(FindObject<UObject*>(L"FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataExplosive.AmmoDataExplosive"), 999);
			AddItemToInventory(FindObject<UObject*>(L"FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataShells.AmmoDataShells"), 999);
			AddItemToInventory(FindObject<UObject*>(L"FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataBulletsMedium.AmmoDataBulletsMedium"), 999);
			AddItemToInventory(FindObject<UObject*>(L"FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataBulletsLight.AmmoDataBulletsLight"), 999);
			AddItemToInventory(FindObject<UObject*>(L"FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataBulletsHeavy.AmmoDataBulletsHeavy"), 999);
			AddItemToInventory(FindObject<UObject*>(L"FortAmmoItemDefinition /Game/Athena/Items/Ammo/AmmoInfinite_NoIcon.AmmoInfinite_NoIcon"), 999);
		}
	}

	bool IsMatchingGuid(FGuid A, FGuid B)
	{
		return A.A == B.A && A.B == B.B && A.C == B.C && A.D == B.D;
	}
};
