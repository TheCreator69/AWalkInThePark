


#include "SplineMovementComponent.h"
#include "WalkPawn.h"
#include "WalkPath.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
USplineMovementComponent::USplineMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


void USplineMovementComponent::UpdateDistanceAlongSpline(float DeltaTime)
{
	DistanceAlongSpline += DeltaTime * CurrentSpeed;
}

void USplineMovementComponent::SetOwnerTransformAlongSpline() const
{
	AActor* Owner = GetOwner();
	FTransform SplineTransform = CurrentPath->Spline->GetTransformAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	Owner->SetActorLocation(SplineTransform.GetLocation());

	// If owner is a pawn, use control rotation to prevent camera rotating weirdly if the component has a relative offset on any axis
	APawn* PawnOwner = Cast<APawn>(Owner);
	if (PawnOwner)
	{
		PawnOwner->GetController()->SetControlRotation(SplineTransform.Rotator() + CameraRotationOffset);
	}
	else
	{
		Owner->SetActorRotation(SplineTransform.Rotator() + CameraRotationOffset);
	}
	
}

// Called when the game starts
void USplineMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USplineMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateDistanceAlongSpline(DeltaTime);
	SetOwnerTransformAlongSpline();
}

void USplineMovementComponent::AddToMovementSpeed(float SpeedOffsetPerSecond)
{
	SpeedOffsetPerSecond *= UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	CurrentSpeed = FMath::Clamp(CurrentSpeed + SpeedOffsetPerSecond, 0.f, MaxSpeed);
}

void USplineMovementComponent::AddCameraRotationOffset(FRotator Offset)
{
	CameraRotationOffset += Offset;
	CameraRotationOffset.Pitch = FMath::Clamp(CameraRotationOffset.Pitch, -MaxPitchOffset, MaxPitchOffset);
	CameraRotationOffset.Yaw = FMath::Clamp(CameraRotationOffset.Yaw, -MaxYawOffset, MaxYawOffset);
}

