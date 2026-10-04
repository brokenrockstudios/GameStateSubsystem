# Changelog

Version format `YYMM.DDRR` (year, month, day, revision of that day). Newest first. Keep entries to one line where possible.

## 2610.0302
- `UTickableGameStateSubsystem` now overrides `IsTickable` instead of the deprecated `IsAllowedToTick` (no longer `final`; child overrides should chain to `Super::IsTickable()`). Added a test for a child `IsTickable` override.

## 2610.0301
- Added initial unit tests (`GameStateSubsystemTests`): subsystem lifecycle on `AExtendableGameStateBase`, lookup, `ShouldCreateSubsystem`, networking hooks, and tickable game state / local player subsystems.
