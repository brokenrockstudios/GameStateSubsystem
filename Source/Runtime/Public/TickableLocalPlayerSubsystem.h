// Copyright Broken Rock Studios LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TickableLocalPlayerSubsystem.generated.h"

#define UE_API GAMESTATESUBSYSTEM_API

UCLASS(MinimalAPI)
class UTickableLocalPlayerSubsystem : public ULocalPlayerSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//~ Begin FTickableGameObject
	UE_API virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	UE_API virtual ETickableTickType GetTickableTickType() const override;
	UE_API virtual bool IsTickable() const override final;
	UE_API virtual void Tick(float DeltaTime) override;
	UE_API virtual TStatId GetStatId() const override;
	//{
		//PURE_VIRTUAL(UTickableWorldSubsystem::GetStatId, return TStatId();
		//		);
	//	RETURN_QUICK_DECLARE_CYCLE_STAT(FSkeletalMeshEditor, STATGROUP_Tickables);
	//}
	//~ End FTickableGameObject

	//~ Begin USubsystem
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void Deinitialize() override;
	//~End USubsystem

	bool IsInitialized() const { return bInitialized; }

private:
	bool bInitialized = false;
};

#undef UE_API
