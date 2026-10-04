// Copyright Broken Rock Studios LLC. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"
#include "GameStateSubsystemTestFixture.h"

// A subsystem replicates as a sub-object of its game state and sends RPCs through the game state's connection.
// The test world has no net driver, so only the local paths run here. Real replication needs a connection (Stage 5a).

TEST_CLASS(GameStateSubsystemNetworkingTests, "BRS.GameStateSubsystem.Networking")
{
	FGameStateSubsystemFixture Fixture;

	BEFORE_EACH()
	{
		Fixture.Init();
	}

	TEST_METHOD(Subsystem_IsSupportedForNetworking_WithAStableName)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		ASSERT_THAT(IsTrue(Subsystem->IsSupportedForNetworking()));
		ASSERT_THAT(IsTrue(Subsystem->IsNameStableForNetworking()));
		ASSERT_THAT(IsTrue(Subsystem->IsFullNameStableForNetworking()));
	}

	TEST_METHOD(GameState_ReplicatesUsingTheRegisteredSubObjectList)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		ASSERT_THAT(IsTrue(GameState.IsUsingRegisteredSubObjectList()));
	}

	/** The subsystems are registered for replication in BeginPlay, not at creation. */
	TEST_METHOD(BeginPlay_RegistersEverySubsystemAsAReplicatedSubObject)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		UGSSTestTickableSubsystem* Tickable = GameState.GetSubsystem<UGSSTestTickableSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		ASSERT_THAT(IsNotNull(Tickable));
		ASSERT_THAT(IsFalse(GameState.IsReplicatedSubObjectRegistered(Subsystem)));

		FGameStateSubsystemFixture::BeginPlay(GameState);

		ASSERT_THAT(IsTrue(GameState.IsReplicatedSubObjectRegistered(Subsystem)));
		ASSERT_THAT(IsTrue(GameState.IsReplicatedSubObjectRegistered(Tickable)));
	}

	/** A subsystem asks the game state where a function should run, so an RPC follows the game state's role. */
	TEST_METHOD(GetFunctionCallspace_ForwardsToTheGameState)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		UFunction* ServerPing = UGSSTestSubsystem::StaticClass()->FindFunctionByName(TEXT("ServerPing"));
		ASSERT_THAT(IsNotNull(ServerPing));

		ASSERT_THAT(AreEqual(GameState.GetFunctionCallspace(ServerPing, nullptr), Subsystem->GetFunctionCallspace(ServerPing, nullptr)));
	}

	/** On an authoritative, standalone game state a server RPC is a plain local call. */
	TEST_METHOD(ServerRpc_OnAnAuthoritativeStandaloneGameState_RunsLocally)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		GGSSTestLog.Events.Reset();

		Subsystem->ServerPing();

		ASSERT_THAT(AreEqual(FString(TEXT("Ping")), GGSSTestLog.Sequence()));
	}

	TEST_METHOD(CallRemoteFunction_WithoutANetDriver_ReturnsFalse)
	{
		AGSSTestGameState& GameState = Fixture.Spawn();
		UGSSTestSubsystem* Subsystem = GameState.GetSubsystem<UGSSTestSubsystem>();
		ASSERT_THAT(IsNotNull(Subsystem));
		UFunction* ServerPing = UGSSTestSubsystem::StaticClass()->FindFunctionByName(TEXT("ServerPing"));
		ASSERT_THAT(IsNotNull(ServerPing));
		ASSERT_THAT(IsNull(GameState.GetNetDriver()));

		ASSERT_THAT(IsFalse(Subsystem->CallRemoteFunction(ServerPing, nullptr, nullptr, nullptr)));
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
