// Copyright Broken Rock Studios LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorTestSpawner.h"
#include "GameStateSubsystemTestTypes.h"
#include "TestGameInstance.h"

/**
 * A transient game world with a game instance, and helpers to spawn AGSSTestGameState.
 *
 * Declare it as a TEST_CLASS member and call Init() in BEFORE_EACH (CQTest also constructs test classes at module load,
 * so nothing here can run in a constructor). Init() also clears GGSSTestLog.
 *
 * The test world never begins play. Drive BeginPlay with BeginPlay(Actor) and EndPlay by destroying the actor
 * (AActor::RouteEndPlay only runs EndPlay for an actor that has begun play).
 */
class FGameStateSubsystemFixture
{
public:
	FActorTestSpawner Spawner;

	void Init()
	{
		GGSSTestLog.Reset();
		Spawner.InitializeGameSubsystems();
	}

	AGSSTestGameState& Spawn()
	{
		return Spawner.SpawnActor<AGSSTestGameState>();
	}

	/** The test world never begins play, so BeginPlay is dispatched by hand. */
	static void BeginPlay(AActor& Actor)
	{
		Actor.DispatchBeginPlay();
	}
};
