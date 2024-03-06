


#include "SittingComponent.h"
#include "WalkPawn.h"
#include "../Core/WalkDefines.h"
#include "../Environment/ParkBench.h"
#include "Kismet/GameplayStatics.h"
#include "SplineMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

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

	RemoveMappingContext(WalkingMappingContext);

	PlayAnimMontage(SittingDownMontage);

	UAnimInstance* AnimInstance = Player->MeshComponent->GetAnimInstance();
	AnimInstance->OnMontageEnded.AddDynamic(this, &USittingComponent::FinishSitDown);

	Player->MontageCameraComponent->Activate();
	Player->CameraComponent->Deactivate();

	UE_LOGFMT(LogSitting, Display, "Player started sitting down");
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

	RemoveMappingContext(SittingMappingContext);

	PlayAnimMontage(StandingUpMontage);

	UAnimInstance* AnimInstance = Player->MeshComponent->GetAnimInstance();
	AnimInstance->OnMontageEnded.AddDynamic(this, &USittingComponent::FinishGetUp);

	Player->MontageCameraComponent->Activate();
	Player->CameraComponent->Deactivate();

	UE_LOGFMT(LogSitting, Display, "Player got up");
}

bool USittingComponent::IsPlayerSitting() const
{
	return SitStatus != Standing;
}

bool USittingComponent::CanPlayerGetUp() const
{
	return SitStatus != ForcedSitting;
}

void USittingComponent::FinishSitDown(UAnimMontage* Montage, bool bInterrupted)
{
	SitStatus = ForcedSitting;

	CurrentBench->K2_OnPlayerSitDown();

	AddMappingContext(SittingMappingContext);

	UAnimInstance* AnimInstance = Player->MeshComponent->GetAnimInstance();
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &USittingComponent::FinishSitDown);

	Player->CameraComponent->Activate();
	Player->MontageCameraComponent->Deactivate();

	UE_LOGFMT(LogSitting, Display, "Player finished sitting down");
}

void USittingComponent::FinishGetUp(UAnimMontage* Montage, bool bInterrupted)
{
	SitStatus = Standing;

	CurrentBench->K2_OnPlayerGetUp();
	CurrentBench = nullptr;

	AddMappingContext(WalkingMappingContext);

	UAnimInstance* AnimInstance = Player->MeshComponent->GetAnimInstance();
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &USittingComponent::FinishGetUp);

	Player->CameraComponent->Activate();
	Player->MontageCameraComponent->Deactivate();

	UE_LOGFMT(LogSitting, Display, "Player finished getting up");
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

void USittingComponent::PlayAnimMontage(UAnimMontage* Montage) const
{
	if (!Montage) return;

	UAnimInstance* AnimInstance = Player->MeshComponent->GetAnimInstance();
	
	float Duration = AnimInstance->Montage_Play(Montage, 1.0f, EMontagePlayReturnType::Duration);
		
	UE_LOGFMT(LogSitting, Display, "Animation montage started playing with duration: {0}", Duration);
}

