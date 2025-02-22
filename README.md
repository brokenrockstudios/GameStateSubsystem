
NOTE: This is a fork of the original 

## GameState Subsystems

Replicated Subsystems - Extensible GameState via subsystems rather than GameState Components.
To set up please enable the plugin and reparent your GameState to AExtensibleGameStateBase, then just create a UGameStateSubsystem and it will be replicated and initialized automatically.

You can find an example of the functionality in UExampleGameStateSubsystem, it's disabled by default, to turn it on just change CreateExampleSubsystem = true in ShouldCreateSubsystem.

Hope you find a good usage for them in your projects <3

## Why didn't you just replicate world subsystems?

Because World Subsystems have bizarre and awkward lifetimes that highly complicate replication and networking. It is completely possible with Iris, but in practice it was actually very awkward to use and it didn't really offer many considerable benefits over GameState Subsystems other than a few niche usages.

The other more technical reason is because a subsystem is a singleton, by it's nature the patterns end up matching that of the GameState when it's replicated anyway - you can't really meaningfully implement a Subsystem that behaves like the Player Controller because the Server would require an authoritative version for each player with individual remotes having their local proxy of said subsystem. However that doesn't work because it breaks the singleton pattern on the server due to requiring multiple instances.

## Why is this useful?

I am not the biggest fan of the Lyra Experience system, however I do see it's value - especially in GameState Components. However, for some cases I think that it would be nice if they sort of just registered themselves and just worked, rather than needing to be manually added as a default subobject or added dynamically like Lyra Experience does. So I made this Subsystem type, and I find this solution to be a bit of a best of both worlds.

It can make it more straightforward to add code in auxiliary modules that require a GameState or replication without having to do any extra work other than reparenting the top-level GameState class. You can achieve the same with GameState Components with the right setup of course, especially if you're using Lyra, but it can be relatively easy to do it with these Subsystems.

## When isn't it useful?

It is still prone to the same limitations as the GameState and GameState Components - These subsystems do not exist on clients until the GameState has replicated. Therefore, if you are looking for a solution that is immediately available on clients upon a World being created, this Subsystem type doesn't offer that.

When you want logic that can be dynamically added or removed based on specific GameStates you are probably better off just using a GameState Component for that as it's much better suited and designed for that purpose.

## Support

This code was designed to work on a version > 5.3 but it's likely possible to backport the code. It doesn't necessarily do any black magic or special things that are new to the engine. If something is broken, you can ping me at my discord handle "snaps" and i'll try push a fix asap.

## Contributions

Big thanks to Vori for helping me battle test it. PRs are welcome! Feel free to contribute <3


## Changes

* Moved Example code to this readme
* Split TickableGameState Subsystem into its own class file
* Fix GetSubsystemArray error and warning for UE5.5
* Changed to DefaultModule

# Example GameState Subsystem

## ExampleGameStateSubsystem.h

```cpp
UCLASS(MinimalAPI)
class UExampleGameStateSubsystem : public UTickableGameStateSubsystem
{
GENERATED_BODY()
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Tick(float DeltaTime) override;
    TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UExampleGameStateSubsystem, STATGROUP_Tickables); }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerDoSomething();

	UFUNCTION(Client, Reliable)
	void ClientDoSomething();

	UPROPERTY(Transient, Replicated)
	int32 RepVar1 = 0;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_RepVar2)
	int32 RepVar2 = 0;

	UFUNCTION()
	void OnRep_RepVar2(int32 NewVar2);

	UFUNCTION(BlueprintCallable)
	void BlueprintDoAThing();

private:

	FTimerHandle RPCTestTimer;
};
```

## ExampleGameStateSubsystem.cpp
```cpp

void UExampleGameStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	checkf(GetOuter(), TEXT("No Outer."));

	GetWorldTimerManager().SetTimer(RPCTestTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		if(HasAuthority())
		{
			ServerDoSomething();
		}
		else
		{
			ClientDoSomething();
		}
	}), 5.0f, true);

	UE_LOGFMT(LogNet, Display, "Initialized Example Gamestate Subsystem, NetMode - {0}", NetModeToString(GetWorld()->GetNetMode()));
}

void UExampleGameStateSubsystem::Deinitialize()
{
	Super::Deinitialize();

	RPCTestTimer.Invalidate();
	
	UE_LOGFMT(LogNet, Display, "Deinitialized Example Gamestate Subsystem, NetMode - {0}", NetModeToString(GetWorld()->GetNetMode()));
}

bool UExampleGameStateSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	constexpr bool CreateExampleSubsystem = false;
	return Super::ShouldCreateSubsystem(Outer) && CreateExampleSubsystem;
}

void UExampleGameStateSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		RepVar2 += 1;
	}
}

void UExampleGameStateSubsystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UExampleGameStateSubsystem, RepVar1, COND_OwnerOnly);
	DOREPLIFETIME(UExampleGameStateSubsystem, RepVar2);
}

void UExampleGameStateSubsystem::ServerDoSomething_Implementation()
{
	RepVar1 = FMath::Rand();
	UE_LOGFMT(LogNet, Display, "Logging Something on Server! {0}", NetModeToString(GetWorld()->GetNetMode()));
}

void UExampleGameStateSubsystem::ClientDoSomething_Implementation()
{
	UE_LOGFMT(LogNet, Display, "Logging Something on Client! {0}", NetModeToString(GetWorld()->GetNetMode()));
}

void UExampleGameStateSubsystem::OnRep_RepVar2(int32 NewVar2)
{
	if(!HasAuthority())
	{
		UE_LOGFMT(LogNet, Display, "RepVar2 Changed on Example GameState Subsystem! {0}", NewVar2);
	}
}

void UExampleGameStateSubsystem::BlueprintDoAThing()
{
	UE_LOGFMT(LogTemp, Display, "Do a thing called from Blueprint.");
}


```