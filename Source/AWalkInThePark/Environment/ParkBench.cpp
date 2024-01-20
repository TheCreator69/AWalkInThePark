


#include "ParkBench.h"

// Sets default values
AParkBench::AParkBench()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	BenchComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bench"));
	BenchComponent->SetupAttachment(DefaultSceneRoot);
}

// Called when the game starts or when spawned
void AParkBench::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AParkBench::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AParkBench::Interact_Implementation(AActor* Source)
{
	UE_LOG(LogTemp, Warning, TEXT("Interaction from C++"))
}

FText AParkBench::GetInteractionPrompt_Interaction()
{
	return FText::FromString(TEXT("Interaction Prompt"));
}

