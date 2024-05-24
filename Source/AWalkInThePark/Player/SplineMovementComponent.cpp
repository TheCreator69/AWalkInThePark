


#include "SplineMovementComponent.h"
#include "WalkPawn.h"
#include "WalkPath.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/WalkDefines.h"
#include "Camera/CameraShakeBase.h"
#include "../Monster/ShadowMonster.h"

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
	DistanceAlongSpline = FMath::Clamp(DistanceAlongSpline, 0, CurrentPath->Spline->GetSplineLength());

	UE_LOGFMT(LogSplineMovement, Verbose, "Distance along spline updated: {0}", DistanceAlongSpline);
}

void USplineMovementComponent::SetOwnerTransformAlongSpline() const
{
	APawn* Owner = Cast<APawn>(GetOwner());
	if (!Owner)
	{
		UE_LOGFMT(LogSplineMovement, Warning, "{0} added to actor which doesn't descend from Pawn!", GetOwner()->GetFName());
		return;
	}
	
	// Fuck this.
	if (bSitMode)
	{
		Owner->SetActorLocation(SitModeLocation);

		const FRotator InterpCameraRotationOffset = FMath::RInterpTo(
			Owner->GetControlRotation(), 
			SitModeBaseOffset + CameraRotationOffset, 
			GetWorld()->GetDeltaSeconds(), 
			RotationOffsetInterpSpeed
		);

		Owner->GetController()->SetControlRotation(InterpCameraRotationOffset);
	}
	else
	{
		FTransform SplineTransform = CurrentPath->Spline->GetTransformAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		Owner->SetActorLocation(SplineTransform.GetLocation());

		const FRotator InterpCameraRotationOffset = FMath::RInterpTo(
			Owner->GetControlRotation(), 
			SplineTransform.Rotator() + CameraRotationOffset, 
			GetWorld()->GetDeltaSeconds(), 
			RotationOffsetInterpSpeed
		);

		Owner->GetController()->SetControlRotation(InterpCameraRotationOffset);
	}
}

void USplineMovementComponent::SetOwnerMeshRotation() const
{
	AWalkPawn* Owner = Cast<AWalkPawn>(GetOwner());
	if (!Owner) return;

	if (bSitMode)
	{
		Owner->MeshComponent->SetWorldRotation(SitModeBaseOffset + FRotator(0, -90, 0));
	}
	else
	{
		FTransform SplineTransform = CurrentPath->Spline->GetTransformAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		// A bit of a hack since rotating the skeletal mesh and animations would require more time and expertise from the developer.
		FRotator MeshRotation = SplineTransform.GetRotation().Rotator() + StandingMeshBaseOffset;
		Owner->MeshComponent->SetWorldRotation(MeshRotation);
	}
}

void USplineMovementComponent::SetShadowMeshRotation() const
{
	AShadowMonster* Owner = Cast<AShadowMonster>(GetOwner());
	if (!Owner) return;

	FTransform SplineTransform = CurrentPath->Spline->GetTransformAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	// A bit of a hack since rotating the skeletal mesh and animations would require more time and expertise from the developer.
	FRotator MeshRotation = SplineTransform.GetRotation().Rotator() + StandingMeshBaseOffset;
	Owner->ShadowMesh->SetWorldRotation(MeshRotation);
}

void USplineMovementComponent::StopOwnerWhenEndReached()
{
	float SplineTraversalPercentage = DistanceAlongSpline / CurrentPath->Spline->GetSplineLength();
	if (SplineTraversalPercentage >= 1.f)
	{
		CurrentSpeed = 0.f;
		UE_LOGFMT(LogSplineMovement, Verbose, "Owner reached end of spline");
	}
}

void USplineMovementComponent::PlayAppropriateCameraShake()
{
	APawn* Owner = Cast<APawn>(GetOwner());
	if (!Owner) return;

	float SpeedPercentage = GetPlayerSpeedPercentage();
	TSubclassOf<UCameraShakeBase> ChosenCameraShake;
	if (SpeedPercentage > 0.5f)
	{
		ChosenCameraShake = FastWalkCameraShake;
	}
	else if (SpeedPercentage > 0.f)
	{
		ChosenCameraShake = SlowWalkCameraShake;
	}
	else
	{
		ChosenCameraShake = IdleCameraShake;
	}
	
	if (ChosenCameraShake->IsChildOf<UCameraShakeBase>())
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), ChosenCameraShake, Owner->GetActorLocation(), 0.f, 100.f);
	}
}

void USplineMovementComponent::CorrectCameraOvershoot()
{
	double CurrentPitch = CameraRotationOffset.Pitch;
	if (FMath::Abs<double>(CurrentPitch) - MaxPitchOffset > 0)
	{
		double TargetPitch = MaxPitchOffset * FMath::Sign<double>(CurrentPitch);
		double CorrectedPitch = FMath::FInterpTo<double>(CurrentPitch, TargetPitch, GetWorld()->GetDeltaSeconds(), OvershootCorrectionInterpSpeed);
		CameraRotationOffset.Pitch = CorrectedPitch;
	}

	double CurrentYaw = CameraRotationOffset.Yaw;
	if (FMath::Abs<double>(CurrentYaw) - MaxYawOffset > 0)
	{
		double TargetYaw = MaxYawOffset * FMath::Sign<double>(CurrentYaw);
		double CorrectedYaw = FMath::FInterpTo<double>(CurrentYaw, TargetYaw, GetWorld()->GetDeltaSeconds(), OvershootCorrectionInterpSpeed);
		CameraRotationOffset.Yaw = CorrectedYaw;
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

	if (CurrentPath)
	{
		UpdateDistanceAlongSpline(DeltaTime);
		SetOwnerTransformAlongSpline();
		SetOwnerMeshRotation();
		SetShadowMeshRotation();
		StopOwnerWhenEndReached();
	}

	CorrectCameraOvershoot();
	PlayAppropriateCameraShake();
}

void USplineMovementComponent::ChangePath(AWalkPath* NewPath)
{
	CurrentPath = NewPath;
	DistanceAlongSpline = 0.f;
}

void USplineMovementComponent::AddToMovementSpeed(float SpeedOffsetPerSecond)
{
	float SpeedOffsetThisTick = SpeedOffsetPerSecond * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	CurrentSpeed = FMath::Clamp(CurrentSpeed + SpeedOffsetThisTick, 0.f, MaxSpeed);

	UE_LOGFMT(LogSplineMovement, Verbose, "Movement speed changed by offset: {0}", SpeedOffsetThisTick);
}

void USplineMovementComponent::StopMovement()
{
	CurrentSpeed = 0.f;
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
	CameraRotationOffset.Pitch = FMath::Clamp(CameraRotationOffset.Pitch, -(MaxPitchOffset + MaxPitchOvershoot), MaxPitchOffset + MaxPitchOvershoot);
	CameraRotationOffset.Yaw = FMath::Clamp(CameraRotationOffset.Yaw, -(MaxYawOffset + MaxYawOvershoot), MaxYawOffset + MaxYawOvershoot);

	UE_LOGFMT(LogSplineMovement, Verbose, "Camera rotation offset set: {0}", CameraRotationOffset.ToCompactString());
}

float USplineMovementComponent::GetPlayerSpeedPercentage() const
{
	return CurrentSpeed / MaxSpeed;
}

FRotator USplineMovementComponent::GetPlayerSplineRotation() const
{
	return CurrentPath->Spline->GetTransformAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World).Rotator();
}

