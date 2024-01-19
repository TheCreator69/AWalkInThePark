


#include "WalkPath.h"
#include "Components/SplineComponent.h"

// Sets default values
AWalkPath::AWalkPath()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("PathComponent"));
	Spline->SetupAttachment(DefaultSceneRoot);
}

// Called when the game starts or when spawned
void AWalkPath::BeginPlay()
{
	Super::BeginPlay();
	
}

