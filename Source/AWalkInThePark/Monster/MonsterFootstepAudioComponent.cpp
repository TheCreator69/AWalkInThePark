


#include "MonsterFootstepAudioComponent.h"
#include "WalkMonster.h"

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
	// Switch high and medium aggression sounds if needed
	TEnumAsByte<ThresholdCrossing> HighAggressionThreshold = AggressionCrossedThreshold(OldAggression, NewAggression, SoundThresholds.ToggleHighAggressionSound);
	if (HighAggressionThreshold == TEnumAsByte<ThresholdCrossing>(Upwards))
	{
		SetSound(HighAggressionFootsteps);
		UE_LOG(LogTemp, Warning, TEXT("High aggression sound set!"))
	}
	else if (HighAggressionThreshold == TEnumAsByte<ThresholdCrossing>(Downwards))
	{
		SetSound(MediumAggressionFootsteps);
		UE_LOG(LogTemp, Warning, TEXT("Medium aggression sound set"))
	}

	// Should start making sound?
	TEnumAsByte<ThresholdCrossing> MediumAggressionThreshold = AggressionCrossedThreshold(OldAggression, NewAggression, SoundThresholds.StartMediumAggressionSound);
	if (MediumAggressionThreshold == TEnumAsByte<ThresholdCrossing>(Upwards))
	{
		StartMakingSound();
		UE_LOG(LogTemp, Warning, TEXT("Start making sound!"))
	}

	// Should stop making sound?
	TEnumAsByte<ThresholdCrossing> NoSoundThreshold = AggressionCrossedThreshold(OldAggression, NewAggression, SoundThresholds.StopMediumAggressionSound);
	if (NoSoundThreshold == TEnumAsByte<ThresholdCrossing>(Downwards))
	{
		StopMakingSound();
		UE_LOG(LogTemp, Warning, TEXT("Stop making sound!"))
	}
}

void UMonsterFootstepAudioComponent::StartMakingSound()
{
	OnAudioFinished.AddDynamic(this, &UMonsterFootstepAudioComponent::ScheduleNextSound);
	ScheduleNextSound();
}

void UMonsterFootstepAudioComponent::ScheduleNextSound()
{
	GetWorld()->GetTimerManager().SetTimer(FootstepTimerHandle, this, &UMonsterFootstepAudioComponent::PlaySoundAtProperVolume, FMath::RandRange(1.0, 2.0));
	UE_LOG(LogTemp, Warning, TEXT("Next sound scheduled..."))
}

void UMonsterFootstepAudioComponent::PlaySoundAtProperVolume()
{
	double VolumeAlpha = (Owner->GetAggression() - 0.25) * 4.0;
	float Multiplier = FMath::Lerp<double, double>(0.25, 1.25, VolumeAlpha);
	SetVolumeMultiplier(Multiplier);
	Play();
	UE_LOG(LogTemp, Warning, TEXT("Playing sound!"))
}

void UMonsterFootstepAudioComponent::StopMakingSound()
{
	GetWorld()->GetTimerManager().ClearTimer(FootstepTimerHandle);
	OnAudioFinished.RemoveDynamic(this, &UMonsterFootstepAudioComponent::ScheduleNextSound);
}

TEnumAsByte<ThresholdCrossing> UMonsterFootstepAudioComponent::AggressionCrossedThreshold(double& OldAggression, double& NewAggression, double Threshold)
{
	if (NewAggression == OldAggression)
	{
		return TEnumAsByte<ThresholdCrossing>(NoCross);
	}
	else if (OldAggression < NewAggression && OldAggression <= Threshold && NewAggression >= Threshold)
	{
		return TEnumAsByte<ThresholdCrossing>(Upwards);
	}
	else if (OldAggression > NewAggression && OldAggression >= Threshold && NewAggression <= Threshold)
	{
		return TEnumAsByte<ThresholdCrossing>(Downwards);
	}
	return TEnumAsByte<ThresholdCrossing>(NoCross);
}

