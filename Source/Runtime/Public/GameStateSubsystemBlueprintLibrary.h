// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameStateSubsystemBlueprintLibrary.generated.h"

class AExtendableGameStateBase;
class UGameStateSubsystem;

/**
 * Calls behind the "Get GameState Subsystem" K2 nodes (GameStateSubsystemEd). It lives in the runtime module because
 * a compiled Blueprint calls it in a cooked game, where the editor module does not exist.
 */
UCLASS(MinimalAPI)
class UGameStateSubsystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/** Get a GameState Subsystem from the world's GameState, found through the provided context. Null if the world has no AExtendableGameStateBase. */
	UFUNCTION(BlueprintPure, Category = "GameState Subsystems", meta = (WorldContext = "ContextObject", BlueprintInternalUseOnly = "true"))
	static GAMESTATESUBSYSTEM_API UGameStateSubsystem* GetGameStateSubsystem(UObject* ContextObject, TSubclassOf<UGameStateSubsystem> Class);

	/**
	 * Get a GameState Subsystem from the given GameState.
	 * If the GameState isn't valid a nullptr is returned
	 */
	UFUNCTION(BlueprintPure, Category = "GameState Subsystems", meta = (BlueprintInternalUseOnly = "true"))
	static GAMESTATESUBSYSTEM_API UGameStateSubsystem* GetGameStateSubsystemFromGameState(AExtendableGameStateBase* GameState, TSubclassOf<UGameStateSubsystem> Class);
};
