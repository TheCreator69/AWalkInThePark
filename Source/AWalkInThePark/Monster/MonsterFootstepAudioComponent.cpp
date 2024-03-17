


#include "MonsterFootstepAudioComponent.h"
#include "WalkMonster.h"
#include "../Core/WalkDefines.h"

// Sets default values for this component's properties
UMonsterFootstepAudioComponent::UMonsterFootstepAudioComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UMonsterFootstepAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AWalkMonster>(GetOwner());
	if (!Owner) return;

	PreviousAggression = Owner->GetAggression();
}

void UMonsterFootstepAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(FootstepTimerHandle);
}

void UMonsterFootstepAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Owner) return;

	double NewAggression = Owner->GetAggression();
	ExecuteAggressionThresholdFunctions(PreviousAggression, NewAggression);

	PreviousAggression = NewAggression;
}

void UMonsterFootstepAudioComponent::ExecuteAggressionThresholdFunctions(double& OldAggression, double& NewAggression)
{
	// Should start making sound?
	TEnumAsByte<EThresholdCrossing> SoundThreshold = AggressionCrossedThreshold(OldAggression, NewAggression, SoundThresholds.StartFootstepSounds);
	if (SoundThreshold == TEnumAsByte<EThresholdCrossing>(Upwards))
	{
		StartMakingSound();
	}

	// Should stop making sound?
	TEnumAsByte<EThresholdCrossing> NoSoundThreshold = AggressionCrossedThreshold(OldAggression, NewAggression, SoundThresholds.StopFootstepSounds);
	if (NoSoundThreshold == TEnumAsByte<EThresholdCrossing>(Downwards))
	{
		StopMakingSound();
	}
}

void UMonsterFootstepAudioComponent::StartMakingSound()
{
	PlayRightSoundAtProperVolume();
	OnAudioFinished.AddUniqueDynamic(this, &UMonsterFootstepAudioComponent::ScheduleNextSound);

	UE_LOGFMT(LogMonsterSound, Log, "Start making sound");
}

void UMonsterFootstepAudioComponent::ScheduleNextSound()
{
	float AggressionAlpha = FMath::Clamp((Owner->GetAggression() - SoundThresholds.StopFootstepSounds) / (1.0 - SoundThresholds.StopFootstepSounds), 0.0, 1.0);

	float TimerDelay = FMath::Lerp<double, double>(IntervalMax, IntervalMin, AggressionAlpha) + FMath::RandRange(-RandomIntervalOffset, RandomIntervalOffset);

	GetWorld()->GetTimerManager().SetTimer(FootstepTimerHandle, this, &UMonsterFootstepAudioComponent::PlayRightSoundAtProperVolume, TimerDelay);

	UE_LOGFMT(LogMonsterSound, Display, "Next sound scheduled with delay of: {0} seconds", TimerDelay);
}

void UMonsterFootstepAudioComponent::PlayRightSoundAtProperVolume()
{
	bool bPlayRetreatingFootsteps = Owner->IsPlayerStaringDownMonster();
	if (bPlayRetreatingFootsteps)
	{
		SetSound(RetreatingFootsteps);
	}
	else
	{
		SetSound(ApproachingFootsteps);
	}

	double VolumeAlpha = (Owner->GetAggression() - 0.25) * 4.0;
	float Multiplier = FMath::Lerp<double, double>(0.25, 1.25, VolumeAlpha);
	SetVolumeMultiplier(Multiplier);

	Play();

	FString FootstepType = (bPlayRetreatingFootsteps ? TEXT("retreating") : TEXT("approaching"));
	UE_LOGFMT(LogMonsterSound, Verbose, "Playing {0} sound with volume multiplier: {1}", FootstepType, Multiplier);
}

void UMonsterFootstepAudioComponent::StopMakingSound()
{
	GetWorld()->GetTimerManager().ClearTimer(FootstepTimerHandle);
	OnAudioFinished.RemoveDynamic(this, &UMonsterFootstepAudioComponent::ScheduleNextSound);

	UE_LOGFMT(LogMonsterSound, Log, "Stop making sound");
}

TEnumAsByte<EThresholdCrossing> UMonsterFootstepAudioComponent::AggressionCrossedThreshold(double& OldAggression, double& NewAggression, double Threshold)
{
	if (NewAggression == OldAggression)
	{
		return TEnumAsByte<EThresholdCrossing>(NoCross);
	}
	else if (OldAggression < NewAggression && OldAggression <= Threshold && NewAggression >= Threshold)
	{
		return TEnumAsByte<EThresholdCrossing>(Upwards);
	}
	else if (OldAggression > NewAggression && OldAggression >= Threshold && NewAggression <= Threshold)
	{
		return TEnumAsByte<EThresholdCrossing>(Downwards);
	}
	return TEnumAsByte<EThresholdCrossing>(NoCross);
}

