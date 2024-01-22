

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerFollowComponent.generated.h"

// Component used to "attach" monsters to the player to follow them around without inheriting their rotation
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AWALKINTHEPARK_API UPlayerFollowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerFollowComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
