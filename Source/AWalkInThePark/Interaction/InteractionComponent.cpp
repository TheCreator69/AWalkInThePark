


#include "InteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "InteractiveActor.h"
#include "../Core/WalkDefines.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* PreviousTarget = TargetedActor;

	TraceForInteractiveActor();
	FireTargetEventsIfNecessary(PreviousTarget);
}

void UInteractionComponent::TraceForInteractiveActor()
{
	FVector Start;
	FVector End;
	GetStartAndEndOfTrace(Start, End);

	FHitResult OutHit;
	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility);
	if (OutHit.bBlockingHit && OutHit.GetActor()->Implements<UInteractiveActor>())
	{
		AActor* HitActor = OutHit.GetActor();
		if (!IInteractiveActor::Execute_IsInteractive(HitActor)) return;

		TargetedActor = HitActor;
		UE_LOGFMT(LogInteraction, Verbose, "Interactive actor being targeted: {0}", TargetedActor->GetName());
	}
	else
	{
		TargetedActor = nullptr;
		UE_LOGFMT(LogInteraction, Verbose, "No interactive actor being targeted");
	}
}

void UInteractionComponent::GetStartAndEndOfTrace(FVector& OutStart, FVector& OutEnd) const
{
	APawn* Owner = Cast<APawn>(GetOwner());
	if (!Owner) return;
	if (!Owner->HasActiveCameraComponent()) return;

	OutStart = Owner->FindComponentByClass<UCameraComponent>()->GetComponentLocation();
	OutEnd = OutStart + Owner->FindComponentByClass<UCameraComponent>()->GetForwardVector() * TraceDistance;
}

void UInteractionComponent::FireTargetEventsIfNecessary(AActor* PreviousTarget)
{
	if (PreviousTarget == TargetedActor) return;

	if (!PreviousTarget && TargetedActor)
	{
		OnInteractiveActorStartTarget.Broadcast(TargetedActor);
	}
	else if (PreviousTarget && !TargetedActor)
	{
		OnInteractiveActorEndTarget.Broadcast(PreviousTarget);
	}
}
