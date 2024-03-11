


#include "MusicAudioComponent.h"
#include "AudioModulationStatics.h"
#include "SoundControlBusMix.h"
#include "../Core/WalkDefines.h"
#include "Audio/SoundParameterControllerInterface.h"

// Sets default values for this component's properties
UMusicAudioComponent::UMusicAudioComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UMusicAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	OnAudioFinished.AddDynamic(this, &UMusicAudioComponent::FinishStopMusic);
}

void UMusicAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(HeadphoneBlipTimerHandle);
}

void UMusicAudioComponent::ToggleMusic()
{
	if (bIsPaused)
	{
		StartMusic();
	}
	else
	{
		InitializeStopMusic();
	}
}

void UMusicAudioComponent::ChangeMusicTrack(int NewIndex)
{
	if (!MusicTracks.IsValidIndex(NewIndex)) return;

	SetSound(MusicTracks[NewIndex]);
}

void UMusicAudioComponent::StartMusic()
{
	SetPaused(false);
	OnMusicStateChanged.Broadcast(bIsPaused);
	UAudioModulationStatics::ActivateBusMix(GetWorld(), MusicOnBusMix);
	ISoundParameterControllerInterface::SetTriggerParameter(FName("Resume"));

	UE_LOGFMT(LogMusic, Display, "Music started");
}

void UMusicAudioComponent::InitializeStopMusic()
{
	GetWorld()->GetTimerManager().SetTimer(HeadphoneBlipTimerHandle, this, &UMusicAudioComponent::FinishStopMusic, HeadphoneSoundDuration);
	ISoundParameterControllerInterface::SetTriggerParameter(FName("Stop"));

	UE_LOGFMT(LogMusic, Display, "Music stop requested");
}

void UMusicAudioComponent::FinishStopMusic()
{
	SetPaused(true);
	OnMusicStateChanged.Broadcast(bIsPaused);
	UAudioModulationStatics::DeactivateBusMix(GetWorld(), MusicOnBusMix);

	UE_LOGFMT(LogMusic, Display, "Music stopped");
}
