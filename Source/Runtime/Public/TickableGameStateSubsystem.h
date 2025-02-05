// Copyright (c) 2024 Daft Software
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "GameStateSubsystem.h"

#include "TickableGameStateSubsystem.generated.h"

/**
 * UTickableGameStateSubsystem
 * GameStateSubsystem that has tick functionality.
 */
UCLASS(Abstract, MinimalAPI)
class UTickableGameStateSubsystem : public UGameStateSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
	
	//~ Begin FTickableGameObject
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	GAMESTATESUBSYSTEM_API virtual ETickableTickType GetTickableTickType() const override;
	GAMESTATESUBSYSTEM_API virtual bool IsAllowedToTick() const override final;
	GAMESTATESUBSYSTEM_API virtual void Tick(float DeltaTime) override;
	GAMESTATESUBSYSTEM_API virtual TStatId GetStatId() const override PURE_VIRTUAL(UTickableWorldSubsystem::GetStatId, return TStatId(););
	//~ End FTickableGameObject
	
	//~ Begin USubsystem
	GAMESTATESUBSYSTEM_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	GAMESTATESUBSYSTEM_API virtual void Deinitialize() override;
	//~End USubsystem

	bool IsInitialized() const { return bInitialized; }
	
private:
	bool bInitialized = false;
};
