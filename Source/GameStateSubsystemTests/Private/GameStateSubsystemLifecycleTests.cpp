// Copyright Broken Rock Studios LLC. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"
#include "GameStateSubsystemTestFixture.h"

#include "Engine/World.h"
#include "TimerManager.h"

// AExtendableGameStateBase owns a subsystem collection: it is created in PostInitializeComponents, begun in BeginPlay and
// deinitialized in EndPlay. Tests use AGSSTestGameState, which logs its own BeginPlay and EndPlay into GGSSTestLog.

TEST_CLASS(GameStateSubsystemLifecycleTests, "BRS.GameStateSubsystem.Lifecycle")
{
	FGameStateSubsystemFixture Fixture;

	BEFORE_EACH()
	{
		Fixture.Init();
	}

	TEST_METHOD(Spawn_CreatesTheSubsystem)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		ASSERT_THAT(IsNotNull(GameState.GetSubsystem<UGSSTestSubsystem>()));
		ASSERT_THAT(IsNotNull(GameState.GetSubsystem<UGSSTestTickableSubsystem>()));
	}

	/** Initialize runs from PostInitializeComponents, so a subsystem is usable before BeginPlay. */
	TEST_METHOD(Spawn_InitializesTheSubsystemOnce_BeforeBeginPlay)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		ASSERT_THAT(IsFalse(GameState.HasActorBegunPlay()));
		ASSERT_THAT(AreEqual(FString(TEXT("Initialize")), GGSSTestLog.Sequence()));
	}

	/** The subsystems begin inside the game state's BeginPlay. */
	TEST_METHOD(BeginPlay_BeginsTheSubsystemInsideTheGameStateBeginPlay)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();

		FGameStateSubsystemFixture::BeginPlay(GameState);

		ASSERT_THAT(AreEqual(FString(TEXT("Initialize,GameState.BeginPlay.Enter,BeginPlay,GameState.BeginPlay.Exit")), GGSSTestLog.Sequence()));
	}

	TEST_METHOD(Destroy_AfterBeginPlay_DeinitializesTheSubsystemInsideEndPlay)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		FGameStateSubsystemFixture::BeginPlay(GameState);
		GGSSTestLog.Events.Reset();

		GameState.Destroy();

		ASSERT_THAT(AreEqual(FString(TEXT("GameState.EndPlay.Enter,Deinitialize,GameState.EndPlay.Exit")), GGSSTestLog.Sequence()));
	}

	/**
	 * The engine skips EndPlay for an actor that never began play, so the collection is also deinitialized from
	 * Destroyed(). Without that, the collection is garbage collected while still initialized (T-15).
	 */
	TEST_METHOD(Destroy_BeforeBeginPlay_DeinitializesTheSubsystem)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();

		GameState.Destroy();

		ASSERT_THAT(AreEqual(FString(TEXT("Initialize,Deinitialize")), GGSSTestLog.Sequence()));
	}

	TEST_METHOD(TwoGameStates_EachGetTheirOwnSubsystem)
	{
		AGSSTestGameState& First = Fixture.Spawn();
		AGSSTestGameState& Second = Fixture.Spawn();

		UGSSTestSubsystem* FirstSubsystem = First.GetSubsystem<UGSSTestSubsystem>();
		UGSSTestSubsystem* SecondSubsystem = Second.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(FirstSubsystem));
		ASSERT_THAT(IsNotNull(SecondSubsystem));
		ASSERT_THAT(IsTrue(FirstSubsystem != SecondSubsystem));
		ASSERT_THAT(AreEqual(FString(TEXT("Initialize,Initialize")), GGSSTestLog.Sequence()));
	}

	/** A subsystem whose ShouldCreateSubsystem returns false is not created, and lookups return null instead of failing. */
	TEST_METHOD(SubsystemThatOptsOut_IsNotCreated)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		ASSERT_THAT(IsNull(GameState.GetSubsystem<UGSSTestOptOutSubsystem>()));
		ASSERT_THAT(IsNull(GameState.GetSubsystemBase(UGSSTestOptOutSubsystem::StaticClass())));
	}
};

TEST_CLASS(GameStateSubsystemLookupTests, "BRS.GameStateSubsystem.Lookup")
{
	FGameStateSubsystemFixture Fixture;

	BEFORE_EACH()
	{
		Fixture.Init();
	}

	TEST_METHOD(GetSubsystemBase_ByClass_ReturnsTheSameInstanceAsTheTemplate)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		ASSERT_THAT(IsTrue(GameState.GetSubsystemBase(UGSSTestSubsystem::StaticClass()) == Subsystem));
	}

	TEST_METHOD(StaticGetSubsystem_ReturnsTheSameInstanceAsTheMember)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		ASSERT_THAT(IsNotNull(AExtendableGameStateBase::GetSubsystem<UGSSTestSubsystem>(&GameState)));
		ASSERT_THAT(IsTrue(AExtendableGameStateBase::GetSubsystem<UGSSTestSubsystem>(&GameState) == GameState.GetSubsystem<UGSSTestSubsystem>()));
	}

	TEST_METHOD(StaticGetSubsystem_NullGameState_ReturnsNull)
	{
		ASSERT_THAT(IsNull(AExtendableGameStateBase::GetSubsystem<UGSSTestSubsystem>(nullptr)));
	}

	TEST_METHOD(GetGameState_ReturnsTheOwningGameState)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		ASSERT_THAT(IsTrue(Subsystem->GetGameState() == &GameState));
	}

	TEST_METHOD(GetGameState_Templated_CastsToTheRequestedType)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		ASSERT_THAT(IsTrue(Subsystem->GetGameState<AGSSTestGameState>() == &GameState));
		ASSERT_THAT(IsTrue(Subsystem->GetGameState<AGameStateBase>() == &GameState));
	}

	TEST_METHOD(GetWorld_IsTheWorldOfTheGameState)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		ASSERT_THAT(IsTrue(Subsystem->GetWorld() == GameState.GetWorld()));
		ASSERT_THAT(IsTrue(&Subsystem->GetWorldRef() == &Fixture.Spawner.GetWorld()));
		ASSERT_THAT(IsTrue(&Subsystem->GetWorldTimerManager() == &Fixture.Spawner.GetWorld().GetTimerManager()));
	}

	TEST_METHOD(HasAuthority_OnAnAuthoritativeGameState_IsTrue)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		ASSERT_THAT(IsTrue(Subsystem->GetLocalRole() == ROLE_Authority));
		ASSERT_THAT(IsTrue(Subsystem->HasAuthority()));
	}

	TEST_METHOD(HasAuthority_FollowsTheGameStateRole)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));

		GameState.SetRole(ROLE_SimulatedProxy);

		ASSERT_THAT(IsTrue(Subsystem->GetLocalRole() == ROLE_SimulatedProxy));
		ASSERT_THAT(IsFalse(Subsystem->HasAuthority()));
	}
};

namespace
{
	/** Creates a world of the given type with no world context and destroys it on scope exit. */
	struct FScopedWorld
	{
		explicit FScopedWorld(EWorldType::Type Type) : World(UWorld::CreateWorld(Type, /*bInformEngineOfWorld*/ false)) {}
		~FScopedWorld()
		{
			if (World)
			{
				World->DestroyWorld(/*bInformEngineOfWorld*/ false);
			}
		}

		UWorld* World = nullptr;
	};
}

// The base ShouldCreateSubsystem is called through the base class name, because the test subsystem narrows it to AGSSTestGameState.
TEST_CLASS(GameStateSubsystemShouldCreateTests, "BRS.GameStateSubsystem.ShouldCreate")
{
	FGameStateSubsystemFixture Fixture;

	BEFORE_EACH()
	{
		Fixture.Init();
	}

	TEST_METHOD(OuterInAGameWorld_IsAccepted)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		const UGSSTestSubsystem* Default = GetDefault<UGSSTestSubsystem>();
		ASSERT_THAT(IsTrue(Default->UGameStateSubsystem::ShouldCreateSubsystem(&GameState)));
	}

	TEST_METHOD(OuterWithoutAWorld_IsRejected)
	{
		const UGSSTestSubsystem* Default = GetDefault<UGSSTestSubsystem>();
		ASSERT_THAT(IsFalse(Default->UGameStateSubsystem::ShouldCreateSubsystem(GetTransientPackage())));
	}

	TEST_METHOD(OuterInANonGameWorld_IsRejected)
	{
		FScopedWorld Scoped(EWorldType::Editor);
		ASSERT_THAT(IsNotNull(Scoped.World));
		ASSERT_THAT(IsFalse(Scoped.World->IsGameWorld()));

		const UGSSTestSubsystem* Default = GetDefault<UGSSTestSubsystem>();
		ASSERT_THAT(IsFalse(Default->UGameStateSubsystem::ShouldCreateSubsystem(Scoped.World)));
	}

	/** The test subsystems only exist on AGSSTestGameState. */
	TEST_METHOD(TestSubsystem_IsRejectedForOtherOuters)
	{
		AExtendableGameStateBase& Plain = Fixture.Spawner.SpawnActor<AExtendableGameStateBase>();
		const UGSSTestSubsystem* Default = GetDefault<UGSSTestSubsystem>();
		ASSERT_THAT(IsFalse(Default->ShouldCreateSubsystem(&Plain)));
		ASSERT_THAT(IsNull(Plain.GetSubsystem<UGSSTestSubsystem>()));
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
