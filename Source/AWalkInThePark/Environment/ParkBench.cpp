


#include "ParkBench.h"
#include "../Player/WalkPawn.h"
#include "Camera/CameraComponent.h"
#include "../Core/WalkDefines.h"
#include "Kismet/GameplayStatics.h"
#include "../Player/SittingComponent.h"

// Sets default values
AParkBench::AParkBench()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	BenchComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bench"));
	BenchComponent->SetupAttachment(DefaultSceneRoot);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewTarget"));
	CameraComponent->SetupAttachment(DefaultSceneRoot);
}

// Called when the game starts or when spawned
void AParkBench::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AParkBench::AllowPlayerToGetUp()
{
	Player->SittingComponent->AllowPlayerToGetUp();

	UE_LOGFMT(LogBench, Display, "Played allowed to get back up again");
}

void AParkBench::Interact_Implementation(AActor* Source)
{
	if (!Player || Source != Player) return;

	Player->SittingComponent->OnPlayerSitDown(this);
	bCanPlayerSit = false;

	Player->SetActorLocation(CameraComponent->GetComponentLocation() - FVector(0.0, 0.0, 64.0));
	Player->Controller->SetControlRotation(CameraComponent->GetComponentRotation());

	UE_LOGFMT(LogBench, Display, "Player interacted with park bench");
}

FText AParkBench::GetInteractionPrompt_Implementation()
{
	if (bCanPlayerSit) return FText::FromString(TEXT("Sit Down & Rest"));
	else return FText::FromString(TEXT("You don't feel like resting..."));
}

bool AParkBench::IsInteractive_Implementation()
{
	return bCanPlayerSit;
}

