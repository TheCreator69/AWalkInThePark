

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractiveActorStartTargetSignature, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractiveActorEndTargetSignature, AActor*, PreviousTarget);

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

	// Event that fires when a new interactive actor becomes the target
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractiveActorStartTargetSignature OnInteractiveActorStartTarget;

	// Event that fires when the interactive actor stops being the target
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractiveActorEndTargetSignature OnInteractiveActorEndTarget;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Trace for the first targeted actor and see if it implements the interactive actor interface
	void TraceForInteractiveActor();

	// Get start and end location of the interaction line trace
	void GetStartAndEndOfTrace(FVector& OutStart, FVector& OutEnd) const;

	// Fire start or end target events if targeted actor has changed from previous frame (used in interaction UI)
	void FireTargetEventsIfNecessary(AActor* PreviousTarget);
};
