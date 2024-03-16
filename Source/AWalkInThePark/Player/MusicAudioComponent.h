

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "MusicAudioComponent.generated.h"

class USoundControlBusMix;
struct FMetaSoundOutput;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMusicPlayStateChanged, bool, bIsPaused);

/**
 * Component used to play headphone music.
 */
UCLASS()
class AWALKINTHEPARK_API UMusicAudioComponent : public UAudioComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UMusicAudioComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Music tracks available to play during gameplay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TArray<TObjectPtr<USoundBase>> MusicTracks;

	// Durations of music tracks. A hack because Unreal doesn't report durations accurately
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TArray<double> MusicTrackDurations;

	// Event dispatched when the music starts/stops playing
	UPROPERTY(BlueprintAssignable, Category = "Music")
	FMusicPlayStateChanged OnMusicStateChanged;

	// Control bus mix for modulating sounds when music is playing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TObjectPtr<USoundControlBusMix> MusicOnBusMix;

	// Trigger parameter to execute to stop the music appropriately
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	FName StopTriggerParam = FName("Stop");

	// Float parameter to play music where it was paused at
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	FName PlaybackTimeParam = FName("PlaybackTime");

private:
	// Saved playback time to continue looping music at the correct position
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Music", meta = (AllowPrivateAccess = "true"))
	double PlaybackTime = 0.0;
	
	// Time at which the music started playing
	double PlatformStartTime;

	// Index of the music track that's currently playing
	int MusicIndex = 0;

public:
	// Toggle the music on and off
	UFUNCTION(BlueprintCallable, Category = "Music")
	void ToggleMusic();

	/*
	* Change music track to the one specified in MusicTracks and index NewIndex.
	* Won't change music if NewIndex is out of range.
	*/
	UFUNCTION(BlueprintCallable, Category = "Music")
	void ChangeMusicTrack(int NewIndex);

private:
	// Start playing music when requested
	void StartMusic();

	// Fire MetaSound trigger to play the headphone "blip"
	void InitializeStopMusic();

	// Stop the music once the headphone "blip" has finished playing
	UFUNCTION()
	void FinishStopMusic();
};
