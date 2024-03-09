


#include "WalkCameraActor.h"
#include "Camera/CameraComponent.h"

// Sets default values
AWalkCameraActor::AWalkCameraActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewTarget"));
	CameraComponent->SetupAttachment(DefaultSceneRoot);
}

// Called when the game starts or when spawned
void AWalkCameraActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWalkCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
