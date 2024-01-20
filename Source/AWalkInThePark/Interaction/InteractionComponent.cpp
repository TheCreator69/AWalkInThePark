


#include "InteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "InteractiveActor.h"

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

	TraceForInteractiveActor();
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
		TargetedActor = OutHit.GetActor();
	}
	else
	{
		TargetedActor = nullptr;
	}
}

void UInteractionComponent::GetStartAndEndOfTrace(FVector& OutStart, FVector& OutEnd) const
{
	AActor* Owner = GetOwner();
	APawn* PawnOwner = Cast<APawn>(Owner);

	OutStart = Owner->GetActorLocation() + TraceStart;
	if (PawnOwner)
	{
		if (PawnOwner->HasActiveCameraComponent())
		{
			OutStart = PawnOwner->FindComponentByClass<UCameraComponent>()->GetComponentLocation();
		}
	}

	OutEnd = OutStart + UKismetMathLibrary::GetForwardVector(Owner->GetActorRotation()) * TraceDistance;
	if (PawnOwner)
	{
		if (PawnOwner->HasActiveCameraComponent())
		{
			OutEnd = OutStart + PawnOwner->FindComponentByClass<UCameraComponent>()->GetForwardVector() * TraceDistance;
		}
	}
}
