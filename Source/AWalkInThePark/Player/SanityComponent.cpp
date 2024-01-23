


#include "SanityComponent.h"
#include "WalkPawn.h"
#include "Components/AudioComponent.h"
#include "SplineMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/WalkDefines.h"

// Sets default values for this component's properties
USanityComponent::USanityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void USanityComponent::BeginPlay()
{
	Super::BeginPlay();

	AWalkPawn* Owner = Cast<AWalkPawn>(GetOwner());
	if (!Owner) return;

	Owner->OnMusicStateChanged.AddUniqueDynamic(this, &USanityComponent::SetDecreaseSanity);
	Owner->OnBeginOverlapSafeZone.AddUniqueDynamic(this, &USanityComponent::ProhibitDecreasingSanityAndReset);
	Owner->OnEndOverlapSafeZone.AddUniqueDynamic(this, &USanityComponent::AllowDecreasingSanity);
}

void USanityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(IntrusiveThoughtTimerHandle);
}

void USanityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UpdateSanity(DeltaTime);
	UpdateSanityEffects();
}

void USanityComponent::SetSanity(float NewSanity)
{
	Sanity = FMath::Clamp(NewSanity, 0.f, 1.f);
	if (Sanity == 0.f)
	{
		AWalkPawn* Owner = Cast<AWalkPawn>(GetOwner());
		if (!Owner) return;

		Owner->KillPlayer(TEnumAsByte<PlayerDeathReason>(Insanity));
	}
	UE_LOGFMT(LogSanity, Verbose, "Sanity changed: {0}", Sanity);
}

void USanityComponent::SetDecreaseSanity(bool bNewDecreaseSanity)
{
	bDecreaseSanity = bNewDecreaseSanity;
	UE_LOGFMT(LogSanity, Log, "Sanity decrease status changed: {0}", bNewDecreaseSanity);
}

void USanityComponent::AllowDecreasingSanity()
{
	bCanDecreaseSanity = true;
	UE_LOGFMT(LogSanity, Display, "Sanity allowed to decrease");
}

void USanityComponent::ProhibitDecreasingSanityAndReset()
{
	bCanDecreaseSanity = false;
	SetSanity(1.f);
	UE_LOGFMT(LogSanity, Display, "Sanity prohibited from decreasing and reset");
}

void USanityComponent::UpdateSanity(float DeltaTime)
{
	if (!bCanDecreaseSanity) return;
	if (bDecreaseSanity)
	{
		SetSanity(Sanity - DeltaTime * SanityDecreaseRate);
	}
	else
	{
		SetSanity(Sanity + DeltaTime * SanityIncreaseRate);
	}
}

void USanityComponent::UpdateSanityEffects()
{
	AWalkPawn* Owner = Cast<AWalkPawn>(GetOwner());
	if (!Owner) return;

	Owner->LowSanityAmbienceComponent->SetVolumeMultiplier(InvertedSanityAlpha(AmbienceThreshold));
	
	// Very subtly tilt the camera. Barely noticable unless observed in an environment with clean lines, but supposidly this works.
	// Called the "Dutch Angle", a type of shot often used to depict madness and disorientation. Very interesting.
	// Btw, "Dutch" has nothing to do with the "Dutch", but rather derives from "Deutsch", which means "German". Yet another German invention!
	float CameraRollOffset = InvertedSanityAlpha(CameraTiltThreshold) * 3.f;
	FRotator CurrentOffset = Owner->SplineMovementComponent->GetCameraRotationOffset();
	Owner->SplineMovementComponent->SetCameraRotationOffset(FRotator(CurrentOffset.Pitch, CurrentOffset.Yaw, CameraRollOffset));

	// Magic values for post process effects. Makes the screen almost completely grayscale at 0% sanity
	float SanityAlpha = InvertedSanityAlpha(0.2f);
	Owner->CameraComponent->PostProcessSettings.bOverride_ColorSaturation = true;
	Owner->CameraComponent->PostProcessSettings.ColorSaturation = PostProcessLerp(1, 0.3, SanityAlpha);
	Owner->CameraComponent->PostProcessSettings.bOverride_ColorContrast = true;
	Owner->CameraComponent->PostProcessSettings.ColorContrast = PostProcessLerp(1, 1.5, SanityAlpha);
	Owner->CameraComponent->PostProcessSettings.bOverride_ColorGammaMidtones = true;
	Owner->CameraComponent->PostProcessSettings.ColorGammaMidtones = PostProcessLerp(1, 0.8, SanityAlpha);

	if (Sanity > 0.2)
	{
		GetWorld()->GetTimerManager().ClearTimer(IntrusiveThoughtTimerHandle);
	}
	else
	{
		// Don't schedule the intrusive thought timer if it's already counting down
		if (GetWorld()->GetTimerManager().IsTimerActive(IntrusiveThoughtTimerHandle)) return;
		GetWorld()->GetTimerManager().SetTimer(IntrusiveThoughtTimerHandle, this, &USanityComponent::PlayIntrusiveThought, 3.f, true, 1.f);
	}
}

float USanityComponent::InvertedSanityAlpha(float Threshold) const
{
	return FMath::Clamp((Threshold - Sanity) * (1.f / Threshold), 0.f, 1.f);
}

FVector4 USanityComponent::PostProcessLerp(double A, double B, float Alpha) const
{
	return FMath::Lerp<FVector4, float>(FVector4(A, A, A, 1), FVector4(B, B, B, 1), Alpha);
}

void USanityComponent::PlayIntrusiveThought()
{
	AWalkPawn* Owner = Cast<AWalkPawn>(GetOwner());
	float VolumeMultiplier = (0.5f - Sanity) * 1.2f + 0.4f;
	UGameplayStatics::PlaySound2D(GetWorld(), Owner->IntrusiveThoughtSoundEffect, VolumeMultiplier);

	UE_LOGFMT(LogSanity, Verbose, "Intrusive thought audio played with volume multiplier: {0}", VolumeMultiplier);
}

