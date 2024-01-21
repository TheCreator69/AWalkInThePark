


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
	APawn* Owner = Cast<APawn>(GetOwner());
	if (!Owner) return;

	FTransform SplineTransform = CurrentPath->Spline->GetTransformAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	Owner->SetActorLocation(SplineTransform.GetLocation());
	Owner->GetController()->SetControlRotation(SplineTransform.Rotator() + CameraRotationOffset);
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
	SetCameraRotationOffset(CameraRotationOffset += Offset);
}

FRotator USplineMovementComponent::GetCameraRotationOffset() const
{
	return CameraRotationOffset;
}

void USplineMovementComponent::SetCameraRotationOffset(FRotator NewOffset)
{
	CameraRotationOffset = NewOffset;
	CameraRotationOffset.Pitch = FMath::Clamp(CameraRotationOffset.Pitch, -MaxPitchOffset, MaxPitchOffset);
	CameraRotationOffset.Yaw = FMath::Clamp(CameraRotationOffset.Yaw, -MaxYawOffset, MaxYawOffset);
}

