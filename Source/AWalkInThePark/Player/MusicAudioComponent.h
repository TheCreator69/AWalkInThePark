

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "MusicAudioComponent.generated.h"

class USoundControlBusMix;

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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Music tracks available to play during gameplay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TArray<TObjectPtr<USoundBase>> MusicTracks;

	// Event dispatched when the music starts/stops playing
	UPROPERTY(BlueprintAssignable, Category = "Music")
	FMusicPlayStateChanged OnMusicStateChanged;

	// Control bus mix for modulating sounds when music is playing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Music")
	TObjectPtr<USoundControlBusMix> MusicOnBusMix;

	/*
	* Hard-coded headphone sound duration to delay stopping music by.
	* Ideally, we'd just listen for an audio stop event, but that doesn't remember the playback time
	* and then we'd have to manually set the playback time when starting the music again.
	* But getting the playback time out of the MetaSound graph is very complicated, so hard-coding it is. Yippee!
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Music")
	float HeadphoneSoundDuration = 0.3f;

private:
	// Timer handle used to delay pausing of music until headphone "blip" is done playing.
	FTimerHandle HeadphoneBlipTimerHandle;

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
