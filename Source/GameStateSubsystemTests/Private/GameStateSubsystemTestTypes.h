// Copyright Broken Rock Studios LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExtendableGameStateBase.h"
#include "GameStateSubsystem.h"
#include "TickableGameStateSubsystem.h"

#include "GameStateSubsystemTestTypes.generated.h"

/**
 * What the test game state and subsystems did, in order. Subsystems are deinitialized (and may be collected) before a test
 * can look at them, so they report here instead of keeping state of their own. Reset by FGameStateSubsystemFixture::Init().
 */
struct FGSSTestLog
{
	/** Lifecycle events of AGSSTestGameState and UGSSTestSubsystem, in the order they happened. */
	TArray<FString> Events;

	/** Times UGSSTestTickableSubsystem::Tick ran, and the delta time of the last one. */
	int32 TickCount = 0;
	float LastDeltaTime = 0.0f;

	/** UGSSTestConditionalTickableSubsystem: its own IsTickable condition, and how often it ticked. */
	bool bConditionalTickEnabled = false;
	int32 ConditionalTickCount = 0;

	/** UGSSTestTickableSubsystem::IsTickable() seen at three points of its life. The defaults are the opposite of what is expected. */
	bool bAllowedToTickBeforeInitialize = true;
	bool bAllowedToTickAfterInitialize = false;
	bool bAllowedToTickAfterDeinitialize = true;

	void Reset() { *this = FGSSTestLog(); }

	FString Sequence() const { return FString::Join(Events, TEXT(",")); }
};

inline FGSSTestLog GGSSTestLog;

/**
 * A game state that logs its BeginPlay and EndPlay. The test subsystems only exist on this class (see their
 * ShouldCreateSubsystem), so they never appear in a real game state such as ALyraGameState.
 */
UCLASS()
class AGSSTestGameState : public AExtendableGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override
	{
		GGSSTestLog.Events.Add(TEXT("GameState.BeginPlay.Enter"));
		Super::BeginPlay();
		GGSSTestLog.Events.Add(TEXT("GameState.BeginPlay.Exit"));
	}

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override
	{
		GGSSTestLog.Events.Add(TEXT("GameState.EndPlay.Enter"));
		Super::EndPlay(EndPlayReason);
		GGSSTestLog.Events.Add(TEXT("GameState.EndPlay.Exit"));
	}
};

/** Logs Initialize, BeginPlay and Deinitialize, and has a server RPC. */
UCLASS()
class UGSSTestSubsystem : public UGameStateSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		return Super::ShouldCreateSubsystem(Outer) && Outer->IsA<AGSSTestGameState>();
	}

	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		Super::Initialize(Collection);
		GGSSTestLog.Events.Add(TEXT("Initialize"));
	}

	virtual void Deinitialize() override
	{
		GGSSTestLog.Events.Add(TEXT("Deinitialize"));
		Super::Deinitialize();
	}

	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		GGSSTestLog.Events.Add(TEXT("BeginPlay"));
	}

	UFUNCTION(Server, Reliable)
	void ServerPing();
	void ServerPing_Implementation() { GGSSTestLog.Events.Add(TEXT("Ping")); }
};

/** Opts out of creation, to check that the collection honors ShouldCreateSubsystem. */
UCLASS()
class UGSSTestOptOutSubsystem : public UGameStateSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return false; }
};

/** Adds its own condition on top of the base IsTickable, to check that the base rules still apply. */
UCLASS()
class UGSSTestConditionalTickableSubsystem : public UTickableGameStateSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		return Super::ShouldCreateSubsystem(Outer) && Outer->IsA<AGSSTestGameState>();
	}

	virtual bool IsTickable() const override
	{
		return Super::IsTickable() && GGSSTestLog.bConditionalTickEnabled;
	}

	virtual void Tick(float DeltaTime) override
	{
		Super::Tick(DeltaTime);
		++GGSSTestLog.ConditionalTickCount;
	}

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UGSSTestConditionalTickableSubsystem, STATGROUP_Tickables);
	}
};

/** Reports when it is allowed to tick, and counts its ticks. */
UCLASS()
class UGSSTestTickableSubsystem : public UTickableGameStateSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		return Super::ShouldCreateSubsystem(Outer) && Outer->IsA<AGSSTestGameState>();
	}

	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		GGSSTestLog.bAllowedToTickBeforeInitialize = IsTickable();
		Super::Initialize(Collection);
		GGSSTestLog.bAllowedToTickAfterInitialize = IsTickable();
	}

	virtual void Deinitialize() override
	{
		Super::Deinitialize();
		GGSSTestLog.bAllowedToTickAfterDeinitialize = IsTickable();
	}

	virtual void Tick(float DeltaTime) override
	{
		Super::Tick(DeltaTime);
		++GGSSTestLog.TickCount;
		GGSSTestLog.LastDeltaTime = DeltaTime;
	}

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UGSSTestTickableSubsystem, STATGROUP_Tickables);
	}
};
