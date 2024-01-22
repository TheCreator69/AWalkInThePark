

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerFollowComponent.generated.h"

class AWalkPawn;

// Component used to "attach" monsters to the player to follow them around without inheriting their rotation
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AWALKINTHEPARK_API UPlayerFollowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerFollowComponent();

	// The offset from the player's position that the monster should retain. Assumes the player is looking towards positive X at start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning")
	FVector OffsetToPlayer;

private:
	// Reference to the active player pawn
	TObjectPtr<AWalkPawn> PlayerPawn;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void SnapToPlayerWithOffset();
};
