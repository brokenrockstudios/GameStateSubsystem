// Copyright Broken Rock Studios LLC. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"
#include "GameStateSubsystemTestFixture.h"

#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Tickable.h"
#include "TickableLocalPlayerSubsystem.h"

// UTickableGameStateSubsystem ticks through FTickableGameObject, but never as a CDO and never before Initialize or after Deinitialize.

TEST_CLASS(GameStateSubsystemTickableTests, "BRS.GameStateSubsystem.Tickable")
{
	FGameStateSubsystemFixture Fixture;

	BEFORE_EACH()
	{
		Fixture.Init();
	}

	/** Class default objects are created by the engine for every subsystem class and must never tick. */
	TEST_METHOD(DefaultObject_NeverTicks)
	{
		const UGSSTestTickableSubsystem* Default = GetDefault<UGSSTestTickableSubsystem>();
		ASSERT_THAT(IsFalse(Default->IsTickable()));
		ASSERT_THAT(IsTrue(Default->GetTickableTickType() == ETickableTickType::Never));
		ASSERT_THAT(IsFalse(Default->IsInitialized()));
	}

	TEST_METHOD(IsInitialized_FollowsTheGameStateLifecycle)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestTickableSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestTickableSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		ASSERT_THAT(IsTrue(Subsystem->IsInitialized()));
	}

	/** An instance may tick only once Initialize has run (the log was taken inside Initialize, before and after the base call). */
	TEST_METHOD(Instance_MayTickOnlyOnceInitialized)
	{
		Fixture.Spawn();
		ASSERT_THAT(IsFalse(GGSSTestLog.bAllowedToTickBeforeInitialize));
		ASSERT_THAT(IsTrue(GGSSTestLog.bAllowedToTickAfterInitialize));
	}

	TEST_METHOD(Deinitialize_StopsTheSubsystemFromTicking)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		FGameStateSubsystemFixture::BeginPlay(GameState);

		GameState.Destroy();

		ASSERT_THAT(IsFalse(GGSSTestLog.bAllowedToTickAfterDeinitialize));
	}

	TEST_METHOD(TickableWorld_IsTheWorldOfTheGameState)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestTickableSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestTickableSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		ASSERT_THAT(IsTrue(Subsystem->GetTickableGameObjectWorld() == GameState.GetWorld()));
	}

	/** The engine's tick of this world reaches the subsystem exactly once. */
	TEST_METHOD(EngineTick_TicksTheInitializedSubsystemOnce)
	{
		Fixture.Spawn();

		FTickableGameObject::TickObjects(&Fixture.Spawner.GetWorld(), LEVELTICK_All, /*bIsPaused*/ false, 0.25f);

		ASSERT_THAT(AreEqual(1, GGSSTestLog.TickCount));
		ASSERT_THAT(IsTrue(GGSSTestLog.LastDeltaTime == 0.25f));
	}

	/** A child's IsTickable that chains to Super is honored by the engine tick, and the base rules still apply on top of it. */
	TEST_METHOD(EngineTick_HonorsAChildIsTickableOverride)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UWorld* World = &Fixture.Spawner.GetWorld();

		GGSSTestLog.bConditionalTickEnabled = false;
		FTickableGameObject::TickObjects(World, LEVELTICK_All, /*bIsPaused*/ false, 0.1f);
		ASSERT_THAT(AreEqual(0, GGSSTestLog.ConditionalTickCount));
		// The plain subsystem is unaffected by the other one's condition.
		ASSERT_THAT(AreEqual(1, GGSSTestLog.TickCount));

		GGSSTestLog.bConditionalTickEnabled = true;
		FTickableGameObject::TickObjects(World, LEVELTICK_All, /*bIsPaused*/ false, 0.1f);
		ASSERT_THAT(AreEqual(1, GGSSTestLog.ConditionalTickCount));

		// Once the subsystem is deinitialized the base rule wins over the child's "true".
		UGSSTestConditionalTickableSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestConditionalTickableSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		ASSERT_THAT(IsTrue(Subsystem->IsTickable()));
		FGameStateSubsystemFixture::BeginPlay(GameState);
		GameState.Destroy();
		FTickableGameObject::TickObjects(World, LEVELTICK_All, /*bIsPaused*/ false, 0.1f);
		ASSERT_THAT(AreEqual(1, GGSSTestLog.ConditionalTickCount));
	}

	/** Ticking stops with EndPlay, even though the game state object still exists. */
	TEST_METHOD(EngineTick_AfterTheGameStateIsDestroyed_DoesNotTick)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		FGameStateSubsystemFixture::BeginPlay(GameState);
		GameState.Destroy();

		FTickableGameObject::TickObjects(&Fixture.Spawner.GetWorld(), LEVELTICK_All, /*bIsPaused*/ false, 0.25f);

		ASSERT_THAT(AreEqual(0, GGSSTestLog.TickCount));
	}
};

// UTickableLocalPlayerSubsystem is the same pattern for a local player. It is created by hand on a bare ULocalPlayer
// and initialized with an unused collection, which the base Initialize ignores.

TEST_CLASS(GameStateSubsystemTickableLocalPlayerTests, "BRS.GameStateSubsystem.TickableLocalPlayer")
{
	TStrongObjectPtr<ULocalPlayer> LocalPlayer;
	TStrongObjectPtr<UTickableLocalPlayerSubsystem> Subsystem;
	/** Heap allocated: CQTest constructs test classes at module load, before anything may touch the engine. */
	TUniquePtr<FSubsystemCollection<ULocalPlayerSubsystem>> Collection;

	BEFORE_EACH()
	{
		Collection = MakeUnique<FSubsystemCollection<ULocalPlayerSubsystem>>();
		LocalPlayer.Reset(NewObject<ULocalPlayer>(GEngine));
		Subsystem.Reset(NewObject<UTickableLocalPlayerSubsystem>(LocalPlayer.Get()));
	}

	AFTER_EACH()
	{
		if (Subsystem.IsValid() && Subsystem->IsInitialized())
		{
			Subsystem->Deinitialize();
		}
	}

	TEST_METHOD(DefaultObject_NeverTicks)
	{
		const UTickableLocalPlayerSubsystem* Default = GetDefault<UTickableLocalPlayerSubsystem>();
		ASSERT_THAT(IsFalse(Default->IsTickable()));
		ASSERT_THAT(IsTrue(Default->GetTickableTickType() == ETickableTickType::Never));
	}

	TEST_METHOD(NewInstance_IsNotInitializedAndDoesNotTick)
	{
		ASSERT_THAT(IsFalse(Subsystem->IsInitialized()));
		ASSERT_THAT(IsFalse(Subsystem->IsTickable()));
	}

	TEST_METHOD(Initialize_AllowsTicking_AndDeinitializeStopsIt)
	{
		Subsystem->Initialize(*Collection);
		ASSERT_THAT(IsTrue(Subsystem->IsInitialized()));
		ASSERT_THAT(IsTrue(Subsystem->IsTickable()));

		Subsystem->Deinitialize();
		ASSERT_THAT(IsFalse(Subsystem->IsInitialized()));
		ASSERT_THAT(IsFalse(Subsystem->IsTickable()));
	}

	TEST_METHOD(Tick_WhenInitialized_IsHarmless)
	{
		Subsystem->Initialize(*Collection);
		Subsystem->Tick(0.1f);
		ASSERT_THAT(IsTrue(Subsystem->IsInitialized()));
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
