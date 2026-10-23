// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "GameStateSubsystemBlueprintLibrary.h"
#include "ExtendableGameStateBase.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameStateSubsystemBlueprintLibrary)

UGameStateSubsystem* UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystem(UObject* ContextObject, TSubclassOf<UGameStateSubsystem> Class)
{
	// The context is the calling Blueprint's self (a widget, pawn, component...), so look the game state up through its world.
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	return GetGameStateSubsystemFromGameState(World ? World->GetGameState<AExtendableGameStateBase>() : nullptr, Class);
}

UGameStateSubsystem* UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystemFromGameState(AExtendableGameStateBase* GameState, TSubclassOf<UGameStateSubsystem> Class)
{
	return GameState ? GameState->GetSubsystemBase(Class) : nullptr;
}
