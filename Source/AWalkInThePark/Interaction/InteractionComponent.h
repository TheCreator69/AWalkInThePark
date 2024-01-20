

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

// This component traces the world for interactive components and handles messaging between interaction source and target
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AWALKINTHEPARK_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

	// The interactive actor that's currently being targeted. null if no interactive actor is being targeted
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<AActor> TargetedActor;

	// The relative location interaction tracing should start from. Unused if owner has a camera component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FVector TraceStart = FVector(0.f);

	// The length of the trace that searches for interactive actors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float TraceDistance = 500.f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void TraceForInteractiveActor();
	void GetStartAndEndOfTrace(FVector& OutStart, FVector& OutEnd) const;
};
