


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
	SetAutoActivate(false);
}

void UMusicAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	OnAudioFinished.AddDynamic(this, &UMusicAudioComponent::FinishStopMusic);
}

void UMusicAudioComponent::ToggleMusic()
{
	if (IsPlaying())
	{
		InitializeStopMusic();
	}
	else
	{
		StartMusic();
	}
}

void UMusicAudioComponent::ChangeMusicTrack(int NewIndex)
{
	if (!MusicTracks.IsValidIndex(NewIndex)) return;

	SetSound(MusicTracks[NewIndex]);
	MusicIndex = NewIndex;
	PlaybackTime = 0.0;
}

void UMusicAudioComponent::StartMusic()
{
	Play();
	OnMusicStateChanged.Broadcast(false);
	UAudioModulationStatics::ActivateBusMix(GetWorld(), MusicOnBusMix);
	ISoundParameterControllerInterface::SetFloatParameter(PlaybackTimeParam, PlaybackTime);

	PlatformStartTime = FPlatformTime::Seconds();

	UE_LOGFMT(LogMusic, Display, "Music started with playback time: {0}", PlaybackTime);
}

void UMusicAudioComponent::InitializeStopMusic()
{
	PlaybackTime += FPlatformTime::Seconds() - PlatformStartTime;
	PlaybackTime = FMath::Fmod(PlaybackTime, MusicTrackDurations[MusicIndex]);
	
	ISoundParameterControllerInterface::SetTriggerParameter(StopTriggerParam);

	UE_LOGFMT(LogMusic, Display, "Music stop requested. Playback time saved: {0}", PlaybackTime);
}

void UMusicAudioComponent::FinishStopMusic()
{
	Stop();
	OnMusicStateChanged.Broadcast(true);
	UAudioModulationStatics::DeactivateBusMix(GetWorld(), MusicOnBusMix);

	UE_LOGFMT(LogMusic, Display, "Music stopped");
}
