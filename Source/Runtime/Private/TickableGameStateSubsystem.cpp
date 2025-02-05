// Copyright (c) 2024 Daft Software
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "TickableGameStateSubsystem.h"

ETickableTickType UTickableGameStateSubsystem::GetTickableTickType() const
{
	// By default, (if the child class doesn't override GetTickableTickType), don't let CDOs ever tick: 
	return IsTemplate() ? ETickableTickType::Never : FTickableGameObject::GetTickableTickType(); 
}

bool UTickableGameStateSubsystem::IsAllowedToTick() const
{
	// No matter what IsTickable says, don't let CDOs or uninitialized subsystems tick.
	// Note: even if GetTickableTickType was overridden by the child class and returns something else than ETickableTickType::Never for CDOs, 
	//  it's probably a mistake, so by default, don't allow ticking. If the child class really intends its CDO to tick, it can always override IsAllowedToTick...
	return !IsTemplate() && bInitialized;
}

void UTickableGameStateSubsystem::Tick(float DeltaTime)
{
	checkf(IsInitialized(), TEXT("Ticking should have been disabled for an uninitialized subsystem : remember to call IsInitialized in the subsystem's IsTickable, IsTickableInEditor and/or IsTickableWhenPaused implementation"));
}

void UTickableGameStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized);
	bInitialized = true;
}

void UTickableGameStateSubsystem::Deinitialize()
{
	check(bInitialized);
	bInitialized = false;
}
