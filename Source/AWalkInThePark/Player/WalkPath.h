

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WalkPath.generated.h"

class USplineComponent;

// Path the player can walk along. Merely a class used to create and alter a spline more easily
UCLASS()
class AWALKINTHEPARK_API AWalkPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWalkPath();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	TObjectPtr<USplineComponent> Spline;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
