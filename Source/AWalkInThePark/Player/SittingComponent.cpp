


#include "SittingComponent.h"
#include "WalkPawn.h"
#include "../Core/WalkDefines.h"
#include "../Environment/ParkBench.h"
#include "Kismet/GameplayStatics.h"
#include "SplineMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
USittingComponent::USittingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void USittingComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AWalkPawn>(GetOwner());
	if (!Player) return;

	AddMappingContext(WalkingMappingContext);
}

void USittingComponent::OnPlayerSitDown(AParkBench* TargetBench)
{
	Player->SplineMovementComponent->StopMovement();
	// To prevent component from immediately resetting player's location and control rotation.
	// Ugly hack, but that way I can use the camera rotation code without having to rewrite it for when the player is sitting.
	Player->SplineMovementComponent->bSitMode = true;
	Player->SplineMovementComponent->SitModeBaseOffset = TargetBench->CameraComponent->GetComponentRotation();

	CurrentBench = TargetBench;
	SitStatus = ForcedSitting;

	RemoveMappingContext(WalkingMappingContext);
	AddMappingContext(SittingMappingContext);

	UE_LOGFMT(LogSitting, Display, "Player sat down");
}

void USittingComponent::AllowPlayerToGetUp()
{
	if (SitStatus != ForcedSitting) return;
	SitStatus = Sitting;

	UE_LOGFMT(LogSitting, Display, "Player allowed to stand up");
}

void USittingComponent::OnPlayerGetUp()
{
	if (!CurrentBench) return;

	AWalkPath* PotentialNextPath = CurrentBench->NextPath;
	if (PotentialNextPath && PotentialNextPath != Player->SplineMovementComponent->CurrentPath)
	{
		Player->SplineMovementComponent->ChangePath(PotentialNextPath);
	}
	// Ugly hack part #2
	Player->SplineMovementComponent->bSitMode = false;

	CurrentBench->GetUp();
	CurrentBench = nullptr;

	SitStatus = Standing;

	RemoveMappingContext(SittingMappingContext);
	AddMappingContext(WalkingMappingContext);

	UE_LOGFMT(LogSitting, Display, "Player got up");
}

bool USittingComponent::IsPlayerSitting()
{
	return SitStatus != Standing;
}

bool USittingComponent::CanPlayerGetUp() const
{
	return SitStatus != ForcedSitting;
}

void USittingComponent::AddMappingContext(UInputMappingContext* MappingContext) const
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem();
	Subsystem->AddMappingContext(MappingContext, 1);
}

void USittingComponent::RemoveMappingContext(UInputMappingContext* MappingContext) const
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem();
	Subsystem->RemoveMappingContext(MappingContext);
}

UEnhancedInputLocalPlayerSubsystem* USittingComponent::GetEnhancedInputSubsystem() const
{
	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	return Subsystem;
}

