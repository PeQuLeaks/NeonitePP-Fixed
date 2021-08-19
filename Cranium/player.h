/**
 * Copyright (c) 2020-2021 Kareem Olim (Kemo)
 * All Rights Reserved. Licensed under the Neo License
 * https://neonite.dev/LICENSE.html
 */

#pragma once

class Player
{
public:
	UObject* Controller;
	UObject* Pawn;
	UObject* Mesh;
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

		auto FUNC_GetAnimInstance = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.SkeletalMeshComponent.GetAnimInstance"));

		USkeletalMeshComponent_GetAnimInstance_Params GetAnimInstance_Params;

		ProcessEvent(this->Mesh, FUNC_GetAnimInstance, &GetAnimInstance_Params);

		this->AnimInstance = GetAnimInstance_Params.ReturnValue;
	}

	void Authorize()
	{
		const auto LocalRole = reinterpret_cast<TEnumAsByte<ENetRole>*>(reinterpret_cast<uintptr_t>(this->Pawn) + ObjectFinder::FindOffset(
			XOR(L"Class /Script/Engine.Actor"), XOR(L"Role")));

		*LocalRole = ENetRole::ROLE_Authority;

		const auto RemoteRole = reinterpret_cast<TEnumAsByte<ENetRole>*>(reinterpret_cast<uintptr_t>(this->Pawn) + ObjectFinder::FindOffset(
			XOR(L"Class /Script/Engine.Actor"), XOR(L"RemoteRole")));

		*RemoteRole = ENetRole::ROLE_Authority;
	}

	void Respawn()
	{
		if (this->Pawn)
		{
			this->Pawn = UE4::SpawnActorEasy(UE4::FindObject<UClass*>(XOR(L"BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C")));

			if (this->Pawn)
			{
				this->Possess();
				this->ShowSkin();
				this->ShowPickaxe();
				this->UpdateAnimInstance();
			}
		}
	}

	void TeleportTo(FVector Location, FRotator Rotation = FRotator())
	{
		const auto FUNC_K2_TeleportTo = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor.K2_TeleportTo"));

		AActor_K2_TeleportTo_Params K2_TeleportTo_Params;
		K2_TeleportTo_Params.DestLocation = Location;
		K2_TeleportTo_Params.DestRotation = Rotation;

		ProcessEvent(this->Pawn, FUNC_K2_TeleportTo, &K2_TeleportTo_Params);
	}

	void TeleportToSpawn()
	{
		TeleportTo(FVector(-156128.36, -159492.78, -2996.30));
	}

	void Possess()
	{
		if (!this->Controller || Util::IsBadReadPtr(this->Controller))
		{
			UpdatePlayerController();
		}

		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.Controller.Possess"));

		AController_Possess_Params params;
		params.InPawn = this->Pawn;

		ProcessEvent(this->Controller, fn, &params);
		printf(XOR("\n[NeoRoyale] PlayerPawn was possessed!\n"));
	}



	auto IsJumpProvidingForce()
	{
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.Character.IsJumpProvidingForce"));

		ACharacter_IsJumpProvidingForce_Params params;

		ProcessEvent(this->Pawn, fn, &params);

		return params.ReturnValue;
	}

	auto StopMontageIfEmote()
	{
		if (!this->Mesh || !this->AnimInstance || !Util::IsBadReadPtr(this->Mesh) || !Util::IsBadReadPtr(this->AnimInstance))
		{
			this->UpdateMesh();
			this->UpdateAnimInstance();
		}

		auto FUNC_GetCurrentActiveMontage = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.AnimInstance.GetCurrentActiveMontage"));

		UAnimInstance_GetCurrentActiveMontage_Params GetCurrentActiveMontage_Params;

		ProcessEvent(this->AnimInstance, FUNC_GetCurrentActiveMontage, &GetCurrentActiveMontage_Params);

		auto CurrentPlayingMontage = GetCurrentActiveMontage_Params.ReturnValue;

		if (CurrentPlayingMontage && CurrentPlayingMontage->GetName().starts_with(XOR(L"Emote_")))
		{
			auto FUNC_Montage_Stop = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.AnimInstance.Montage_Stop"));

			UAnimInstance_Montage_Stop_Params Montage_Stop_Params;
			Montage_Stop_Params.InBlendOutTime = 0;
			Montage_Stop_Params.Montage = CurrentPlayingMontage;

			ProcessEvent(this->AnimInstance, FUNC_Montage_Stop, &Montage_Stop_Params);
		}
	}

	auto IsSkydiving()
	{
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn.IsSkydiving"));

		ACharacter_IsSkydiving_Params params;

		ProcessEvent(this->Pawn, fn, &params);

		return params.ReturnValue;
	}

	auto IsParachuteOpen()
	{
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn.IsParachuteOpen"));

		ACharacter_IsParachuteOpen_Params params;

		ProcessEvent(this->Pawn, fn, &params);

		return params.ReturnValue;
	}

	auto IsParachuteForcedOpen()
	{
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn.IsParachuteForcedOpen"));

		ACharacter_IsParachuteForcedOpen_Params params;

		ProcessEvent(this->Pawn, fn, &params);

		return params.ReturnValue;
	}

	auto Jump()
	{
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.Character.Jump"));

		Empty_Params params;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto SetSkeletalMesh(const wchar_t* meshname)
	{
		if (!this->Mesh || !Util::IsBadReadPtr(this->Mesh))
		{
			this->UpdateMesh();
		}

		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.SkinnedMeshComponent.SetSkeletalMesh"));

		std::wstring MeshName = meshname;

		std::wstring name = MeshName + L"." + MeshName;

		auto Mesh = UE4::FindObject<UObject*>(name.c_str(), true);

		if (Mesh)
		{
			USkinnedMeshComponent_SetSkeletalMesh_Params params;
			params.NewMesh = Mesh;
			params.bReinitPose = false;

			ProcessEvent(this->Mesh, fn, &params);
		}
	}



	void ShowSkin()
	{
		ObjectFinder PawnFinder = ObjectFinder::EntryPoint(uintptr_t(this->Pawn));
		ObjectFinder PlayerStateFinder = PawnFinder.Find(XOR(L"PlayerState"));

		auto KismetLib = UE4::FindObject<UObject*>(XOR(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"));
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortKismetLibrary.UpdatePlayerCustomCharacterPartsVisualization"));

		UFortKismetLibrary_UpdatePlayerCustomCharacterPartsVisualization_Params params;
		params.PlayerState = PlayerStateFinder.GetObj();

		ProcessEvent(KismetLib, fn, &params);
		printf(XOR("\n[NeoRoyale] Character Parts should be visible now!.\n"));
	}

	auto PickupActor(UObject* PlacementDecoItemDefinition, UObject* PickupTarget = nullptr)
	{
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn.PickUpActor"));

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

		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn.EquipWeaponDefinition"));

		std::wstring WeaponName = weaponname;

		std::wstring name = WeaponName + L"." + WeaponName;

		auto WeaponData = UE4::FindObject<UObject*>(name.c_str(), true);

		if (WeaponData && !Util::IsBadReadPtr(WeaponData))
		{
			std::wstring objectName = WeaponData->GetFullName();

			if (objectName.starts_with(L"FortWeapon") || objectName.starts_with(L"AthenaGadget") || objectName.starts_with(L"FortPlayset"))
			{
				if (objectName.starts_with(L"AthenaGadget"))
				{
					auto FUN_weapondef = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortGadgetItemDefinition.GetWeaponItemDefinition"));

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

				ProcessEvent(this->Pawn, fn, &params);
			}
			else if (objectName.starts_with(L"FortTrap"))
			{
				this->PickupActor(WeaponData);
			}
		}
		else
		{
			MessageBoxA(nullptr, XOR("This item doesn't exist!"), XOR("Cranium"), MB_OK);
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
			auto FUNC_GetAnimationHardReference = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortMontageItemDefinitionBase.GetAnimationHardReference"));

			UFortMontageItemDefinitionBase_GetAnimationHardReference_Params GetAnimationHardReference_Params;
			GetAnimationHardReference_Params.BodyType = EFortCustomBodyType::All;
			GetAnimationHardReference_Params.Gender = EFortCustomGender::Both;
			GetAnimationHardReference_Params.PawnContext = this->Pawn;

			ProcessEvent(EmoteDef, FUNC_GetAnimationHardReference, &GetAnimationHardReference_Params);

			auto Animation = GetAnimationHardReference_Params.ReturnValue;

			//got the animation, now play :JAM:
			auto FUNC_Montage_Play = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.AnimInstance.Montage_Play"));

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
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.Actor.K2_GetActorLocation"));

		AActor_K2_GetActorLocation_Params params;

		ProcessEvent(this->Pawn, fn, &params);

		return params.ReturnValue;
	}

	auto SetMovementMode(TEnumAsByte<EMovementMode> NewMode, unsigned char CustomMode)
	{
		ObjectFinder PawnFinder = ObjectFinder::EntryPoint(uintptr_t(this->Pawn));

		ObjectFinder CharMovementFinder = PawnFinder.Find(XOR(L"CharacterMovement"));

		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.CharacterMovementComponent.SetMovementMode"));

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
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn.SetHealth"));

		AFortPawn_SetHealth_Params params;

		params.NewHealthVal = SetHealthInput;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto SetShield(float SetShieldInput)
	{
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn.SetShield"));

		AFortPawn_SetShield_Params params;

		params.NewShieldValue = SetShieldInput;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto SetMaxHealth(float SetMaxHealthInput)
	{
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn.SetMaxHealth"));

		AFortPawn_SetMaxHealth_Params params;

		params.NewHealthVal = SetMaxHealthInput;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto SetMaxShield(float SetMaxShieldInput)
	{
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn.SetMaxShield"));

		AFortPawn_SetMaxShield_Params params;

		params.NewValue = SetMaxShieldInput;

		ProcessEvent(this->Pawn, fn, &params);
	}

	auto SetMovementSpeed(float SetMovementSpeedInput)
	{
		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPawn.SetMovementSpeed"));

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

	auto Skydive()
	{
		/*
		if (this->IsSkydiving())
		{
			auto fn =UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn.EndSkydiving"));

			ProcessEvent(this->Pawn, fn, nullptr);
		}*/

		/*auto fn =UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn.BeginSkydiving"));

		AFortPlayerPawn_BeginSkydiving_Params params;
		params.bFromBus = true;

		ProcessEvent(this->Pawn, fn, &params);*/

		this->SetMovementMode(EMovementMode::MOVE_Custom, 4);
	}

	auto ForceOpenParachute()
	{
		/*
		auto fn =UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerPawn.BP_ForceOpenParachute"));

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

		auto fn = UE4::FindObject<UFunction*>(XOR(L"Function /Script/FortniteGame.FortPlayerController.IsInAircraft"));
		ACharacter_IsInAircraft_Params params;

		ProcessEvent(this->Controller, fn, &params);
		return params.ReturnValue;
	}

	void HideHead(bool bIsHidden)
	{
		auto Mesh = UE4::FindObject<UObject*>(XOR(L"SkeletalMeshComponentBudgeted /Game/Athena/Apollo/Maps/Apollo_Terrain.Apollo_Terrain.PersistentLevel.PlayerPawn_Athena_C_"));

		auto SetOwnerNoSee = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.PrimitiveComponent.SetOwnerNoSee"));

		UPrimitiveComponent_SetOwnerNoSee_Params SetOwnerNoSee_Params;
		SetOwnerNoSee_Params.bNewOwnerNoSee = bIsHidden;

		ProcessEvent(Mesh, SetOwnerNoSee, &SetOwnerNoSee_Params);
	}

	void SetCameraMode(const wchar_t* NewMode)
	{
		auto ClientSetCameraMode = UE4::FindObject<UFunction*>(XOR(L"Function /Script/Engine.PlayerController.ClientSetCameraMode"));

		APlayerController_ClientSetCameraMode_Params ClientSetCameraMode_Params;
		ClientSetCameraMode_Params.NewCamMode = KismetFunctions::GetFName(NewMode);

		ProcessEvent(this->Controller, ClientSetCameraMode, &ClientSetCameraMode_Params);
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

			auto Weapon = WeaponDefFinder.GetObj()->GetName();

			this->EquipWeapon(Weapon.c_str());

			printf(XOR("\n[NeoRoyale] Equipped the pickaxe!\n"));
		}
	}
};
