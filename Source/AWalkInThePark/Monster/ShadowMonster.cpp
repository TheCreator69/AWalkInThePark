


#include "ShadowMonster.h"
#include "../Player/SplineMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AShadowMonster::AShadowMonster()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	LightDisabler = CreateDefaultSubobject<USphereComponent>(TEXT("LightDisabler"));
	LightDisabler->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewTarget"));
	CameraComponent->SetupAttachment(RootComponent);

	ShadowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShadowMesh"));
	ShadowMesh->SetupAttachment(RootComponent);

	SplineMovementComponent = CreateDefaultSubobject<USplineMovementComponent>(TEXT("SplineMovementComponent"));
}

// Called when the game starts or when spawned
void AShadowMonster::BeginPlay()
{
	Super::BeginPlay();
}

void AShadowMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AccelerationPerTick > 0.f)
	{
		SplineMovementComponent->AddToMovementSpeed(AccelerationPerTick);

		TimeAccelerated += DeltaTime;
		if (TimeAccelerated >= TimeNeededToAccelerate)
		{
			AccelerationPerTick = 0.f;
		}
	}
}

// Called to bind functionality to input
void AShadowMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AShadowMonster::AccelerateToSpeed(float TargetSpeed, float Duration)
{
	AccelerationPerTick = TargetSpeed / Duration;
	TimeAccelerated = 0.f;
	TimeNeededToAccelerate = Duration;
}

