


#include "PlayerFollowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Player/WalkPawn.h"
#include "../Player/SplineMovementComponent.h"

// Sets default values for this component's properties
UPlayerFollowComponent::UPlayerFollowComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UPlayerFollowComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

// Called every frame
void UPlayerFollowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SnapToPlayerWithOffset();
}

void UPlayerFollowComponent::SnapToPlayerWithOffset()
{
	if (!PlayerPawn) return;

	FQuat PlayerLookaheadRotation = PlayerPawn->SplineMovementComponent->GetPlayerSplineRotation().Quaternion();
	FVector RotatedOffset = PlayerPawn->GetActorLocation() + PlayerLookaheadRotation.RotateVector(OffsetToPlayer);

	GetOwner()->SetActorLocationAndRotation(RotatedOffset, PlayerLookaheadRotation);
}

