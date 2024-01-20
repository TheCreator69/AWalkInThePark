

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/InteractiveActor.h"
#include "ParkBench.generated.h"

// The park benches the player uses to rest and save their progress on. Also serve as instigators for new monster introductions
UCLASS()
class AWALKINTHEPARK_API AParkBench : public AActor, public IInteractiveActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParkBench();

	// Root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	// Bench static mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BenchComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// C++ implementation of Interact() from IInteractiveActor
	void Interact_Implementation(AActor* Source);

	// C++ implementation of GetInteractionPrompt() from IInteractiveActor
	FText GetInteractionPrompt_Interaction();

};
