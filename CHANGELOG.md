# Changelog

Version format `YYMM.DDRR` (year, month, day, revision of that day). Newest first. Keep entries to one line where possible.

## 2610.2302

- The "Get GameState Subsystem" Blueprint library moved from the `UncookedOnly` editor module to the runtime module so cooked Blueprints can call it, finds the game state through the context's world instead of casting the context, and the K2 nodes no longer crash on a class given through the pin; the "from GameState" class pin is typed `UGameStateSubsystem` (T-741).

## 2610.2301

- `AExtendableGameStateBase` renames each subsystem to its class name after creating the collection, so replicated properties and RPCs resolve between a packaged client and server (cooked `NewObject` names differ per process) (T-740; packaged check is T-1114).

## 2610.0901

- Tests module is `UncookedOnly` instead of `DeveloperTool`, so the staged game target no longer links it (T-220).

## 2610.0501
- `AExtendableGameStateBase` now deinitializes its subsystem collection in `Destroyed()` when the game state never began play (EndPlay is skipped for it, so the collection used to be garbage collected still initialized). Fixes the `LogSubsystemCollection` leak warning that failed an unrelated test (T-15).

## 2610.0302
- `UTickableGameStateSubsystem` now overrides `IsTickable` instead of the deprecated `IsAllowedToTick` (no longer `final`; child overrides should chain to `Super::IsTickable()`). Added a test for a child `IsTickable` override.

## 2610.0301
- Added initial unit tests (`GameStateSubsystemTests`): subsystem lifecycle on `AExtendableGameStateBase`, lookup, `ShouldCreateSubsystem`, networking hooks, and tickable game state / local player subsystems.
