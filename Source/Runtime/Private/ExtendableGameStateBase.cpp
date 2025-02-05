// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ExtendableGameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExtendableGameStateBase)

AExtendableGameStateBase::AExtendableGameStateBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicateUsingRegisteredSubObjectList = true;
}

void AExtendableGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	check(!SubsystemCollection.IsInitialized());
	SubsystemCollection.Initialize(this);
}

void AExtendableGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	SubsystemCollection.ForEachSubsystem([this](UGameStateSubsystem* Subsystem)
	{
		AddReplicatedSubObject(Subsystem);
		Subsystem->BeginPlay();
	}, UGameStateSubsystem::StaticClass());;
}

void AExtendableGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SubsystemCollection.ForEachSubsystem([this](UGameStateSubsystem* Subsystem)
	{
		RemoveReplicatedSubObject(Subsystem);
	}, UGameStateSubsystem::StaticClass());
	
	SubsystemCollection.Deinitialize();
	
	Super::EndPlay(EndPlayReason);
}

void AExtendableGameStateBase::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	AExtendableGameStateBase* This = CastChecked<AExtendableGameStateBase>(InThis);
	This->SubsystemCollection.AddReferencedObjects(InThis, Collector);
	Super::AddReferencedObjects(InThis, Collector);
}
