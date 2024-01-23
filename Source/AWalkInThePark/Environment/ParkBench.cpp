


#include "ParkBench.h"
#include "../Player/WalkPawn.h"
#include "Camera/CameraComponent.h"
#include "../Core/WalkDefines.h"
#include "Kismet/GameplayStatics.h"
#include "../Player/SplineMovementComponent.h"

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
	
	bFindCameraComponentWhenViewTarget = true;
}

// Called when the game starts or when spawned
void AParkBench::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player) return;
}

void AParkBench::Interact_Implementation(AActor* Source)
{
	if (!Player || Source != Player) return;

	// Stop the player dead in their tracks for now. Later on, change tracks
	// (splines, but then the word play wouldn't work anymore)
	Player->SplineMovementComponent->StopPlayerMovement();
	Player->ParkBenchSatOn = this;
	bCanPlayerSit = false;

	if (ForcedSitTime <= 0.0)
	{
		Player->SitStatus = Sitting;
	}
	else
	{
		Player->SitStatus = ForcedSitting;
		GetWorld()->GetTimerManager().SetTimer(ForcedSitEndTimer, this, &AParkBench::AllowPlayerToStand, ForcedSitTime);
	}

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this);

	OnPlayerSitDown.Broadcast();

	UE_LOGFMT(LogBench, Display, "Player sat down on Park Bench with forced sitting time of: {0}", ForcedSitTime);
}

void AParkBench::AllowPlayerToStand()
{
	Player->SitStatus = Sitting;

	UE_LOGFMT(LogBench, Display, "Player is allowed to stand up again");
}

void AParkBench::GetUp()
{
	Player->SitStatus = Standing;
	Player->ParkBenchSatOn = nullptr;

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(Player);

	OnPlayerStandUp.Broadcast();

	UE_LOGFMT(LogBench, Display, "Player got up from Park Bench");
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

