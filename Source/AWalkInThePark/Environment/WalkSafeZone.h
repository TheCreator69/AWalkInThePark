

#pragma once

#include "CoreMinimal.h"
#include "WalkEventVolume.h"
#include "WalkSafeZone.generated.h"

// Zone used to keep the player safe around park benches. 
UCLASS()
class AWALKINTHEPARK_API AWalkSafeZone : public AWalkEventVolume
{
	GENERATED_BODY()

protected:
	virtual void PlayerBeginOverlapInternal(AWalkPawn* Player) override;

	virtual void PlayerEndOverlapInternal(AWalkPawn* Player) override;
	
};
