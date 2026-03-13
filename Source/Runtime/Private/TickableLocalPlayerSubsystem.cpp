// Copyright Broken Rock Studios LLC. All Rights Reserved.


#include "TickableLocalPlayerSubsystem.h"


ETickableTickType UTickableLocalPlayerSubsystem::GetTickableTickType() const
{
	// By default, (if the child class doesn't override GetTickableTickType), don't let CDOs ever tick: 
	return IsTemplate() ? ETickableTickType::Never : FTickableGameObject::GetTickableTickType();
}

bool UTickableLocalPlayerSubsystem::IsTickable() const
{
	// No matter what IsTickable says, don't let CDOs or uninitialized subsystems tick.
	// Note: even if GetTickableTickType was overridden by the child class and returns something else than ETickableTickType::Never for CDOs, 
	//  it's probably a mistake, so by default, don't allow ticking. If the child class really intends its CDO to tick, it can always override IsAllowedToTick...
	return !IsTemplate() && bInitialized;
}

void UTickableLocalPlayerSubsystem::Tick(float DeltaTime)
{
	checkf(IsInitialized(), TEXT("Ticking should have been disabled for an uninitialized subsystem : remember to call IsInitialized in the subsystem's IsTickable, IsTickableInEditor and/or IsTickableWhenPaused implementation"));
}

TStatId UTickableLocalPlayerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTickableLocalPlayerSubsystem, STATGROUP_Tickables);
}

void UTickableLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized);
	bInitialized = true;
}

void UTickableLocalPlayerSubsystem::Deinitialize()
{
	check(bInitialized);
	bInitialized = false;
}
