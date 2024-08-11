


#include "ParkBench.h"
#include "../Player/WalkPawn.h"
#include "Camera/CameraComponent.h"
#include "../Core/WalkDefines.h"
#include "../Core/WalkGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "../Player/SittingComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
AParkBench::AParkBench()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	BenchComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bench"));
	BenchComponent->SetupAttachment(DefaultSceneRoot);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewTargetIndicator"));
	CameraComponent->SetupAttachment(BenchComponent);

	SitDownStartRotationComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("SitDownStartRotation"));
	SitDownStartRotationComponent->SetupAttachment(BenchComponent);
	SitDownStartRotationComponent->SetRelativeRotation(SitDownStartRotation);
}

// Called when the game starts or when spawned
void AParkBench::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	AWalkGameModeBase* GameMode = Cast<AWalkGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	switch (ParkBenchID)
	{
	case 1:
		GameMode->FirstParkBench = this;
		break;
	case 2:
		GameMode->SecondParkBench = this;
		break;
	case 3:
		GameMode->ThirdParkBench = this;
		break;
	default:
		UE_LOGFMT(LogBench, Warning, "Invalid park bench ID (valid range: 1-3)");
	}
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

	Player->SetActorLocation(GetActorLocation() + SitDownStart);

	UE_LOGFMT(LogBench, Display, "Player interacted with park bench");
}

FText AParkBench::GetInteractionPrompt_Implementation()
{
	if (bCanPlayerSit) return NSLOCTEXT("", "SitDownAction", "Sit Down & Rest");
	else return NSLOCTEXT("", "SitDownActionDenied", "You don't feel like resting right now");
}

bool AParkBench::IsInteractive_Implementation()
{
	return bCanPlayerSit;
}

void AParkBench::ReactivateParkBench()
{
	bCanPlayerSit = true;

	UE_LOGFMT(LogBench, Display, "Park bench #{0} reactivated", ParkBenchID);
}

