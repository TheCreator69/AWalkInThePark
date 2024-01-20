

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractiveActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractiveActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can be interacted with
 */
class AWALKINTHEPARK_API IInteractiveActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Called when this actor is being interacted with
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* Source);

	// The on-screen prompt to show when the player hovers over this actor
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	FText GetInteractionPrompt();
};
