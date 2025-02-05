// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "GameStateSubsystem.h"
#include "ModularGameState.h"
#include "ExtendableGameStateBase.generated.h"

UCLASS()
class GAMESTATESUBSYSTEM_API AExtendableGameStateBase : public AModularGameStateBase
{
	GENERATED_BODY()
public:

	AExtendableGameStateBase(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

	/**
	 * Get a Subsystem of specified type
	 */
	UGameStateSubsystem* GetSubsystemBase(TSubclassOf<UGameStateSubsystem> SubsystemClass) const
	{
		return SubsystemCollection.GetSubsystem<UGameStateSubsystem>(SubsystemClass);
	}

	/**
	 * Get a Subsystem of specified type
	 */
	template <typename TSubsystemClass>
	TSubsystemClass* GetSubsystem() const
	{
		return SubsystemCollection.GetSubsystem<TSubsystemClass>(TSubsystemClass::StaticClass());
	}

	/**
	 * Get a Subsystem of specified type from the provided GameState
	 * returns nullptr if the Subsystem cannot be found or the GameState is null
	 */
	template <typename TSubsystemClass>
	static FORCEINLINE TSubsystemClass* GetSubsystem(const AExtendableGameStateBase* GameState)
	{
		if (GameState)
		{
			return GameState->GetSubsystem<TSubsystemClass>();
		}
		return nullptr;
	}

private:

	FObjectSubsystemCollection<UGameStateSubsystem> SubsystemCollection;
};