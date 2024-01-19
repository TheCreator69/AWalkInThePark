


#include "WalkPath.h"
#include "Components/SplineComponent.h"

// Sets default values
AWalkPath::AWalkPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called every frame
void AWalkPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

