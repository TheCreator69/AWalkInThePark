#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WalkCameraActor.generated.h"

class UCameraComponent;

// Hack actor used for smoothly switching view targets. Only useful when attached to another actor with a child actor component.
UCLASS()
class AWALKINTHEPARK_API AWalkCameraActor : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AWalkCameraActor();

	// Default scene root
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	// Camera component used as view target
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
