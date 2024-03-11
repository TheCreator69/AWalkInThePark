


#include "WalkPawn.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "SplineMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "../Interaction/InteractionComponent.h"
#include "../Interaction/InteractiveActor.h"
#include "SanityComponent.h"
#include "../Core/WalkDefines.h"
#include "../Environment/ParkBench.h"
#include "SittingComponent.h"
#include "WalkCameraActor.h"
#include "AudioModulationStatics.h"
#include "SoundControlBusMix.h"

// Sets default values
AWalkPawn::AWalkPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(DefaultSceneRoot);
	CameraComponent->SetRelativeLocation(FVector(12, 0, 64));

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetupAttachment(DefaultSceneRoot);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(DefaultSceneRoot);

	MusicPlayerComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicPlayer"));
	MusicPlayerComponent->SetupAttachment(DefaultSceneRoot);

	LowSanityAmbienceComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LowSanityAmbience"));
	LowSanityAmbienceComponent->SetupAttachment(DefaultSceneRoot);

	IntrusiveThoughtsComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("IntrusiveThoughts"));
	IntrusiveThoughtsComponent->SetupAttachment(DefaultSceneRoot);

	ViewTargetComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("ViewTarget"));
	ViewTargetComponent->SetChildActorClass(AWalkCameraActor::StaticClass());
	ViewTargetComponent->SetupAttachment(MeshComponent, "HeadSocket");

	SplineMovementComponent = CreateDefaultSubobject<USplineMovementComponent>(TEXT("SplineMovementComponent"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	SanityComponent = CreateDefaultSubobject<USanityComponent>(TEXT("SanityComponent"));
	SittingComponent = CreateDefaultSubobject<USittingComponent>(TEXT("SittingComponent"));
}

// Called when the game starts or when spawned
void AWalkPawn::BeginPlay()
{
	Super::BeginPlay();

	MusicPlayerComponent->SetPaused(true);
}

// Called every frame
void AWalkPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AWalkPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PC = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(GlobalMappingContext, 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(ChangeSpeedAction, ETriggerEvent::Triggered, this, &AWalkPawn::ChangeSpeed);
	Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWalkPawn::Look);
	Input->BindAction(ToggleMusicAction, ETriggerEvent::Triggered, this, &AWalkPawn::ToggleMusic);
	Input->BindAction(ChangeSongAction, ETriggerEvent::Triggered, this, &AWalkPawn::ChangeSong);
	Input->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AWalkPawn::Interact);
	Input->BindAction(GetUpAction, ETriggerEvent::Triggered, this, &AWalkPawn::GetUp);
	Input->BindAction(SaveGameAction, ETriggerEvent::Triggered, this, &AWalkPawn::SaveGame);
}

void AWalkPawn::KillPlayer(TEnumAsByte<PlayerDeathReason> Reason)
{
	UE_LOGFMT(LogWalkPlayer, Verbose, "Player is dead with reason: {0}", Reason.GetValue());
}

void AWalkPawn::ChangeSpeed(const FInputActionValue& Value)
{
	float ActionValue = Value.Get<float>();
	bool bSlowDown = ActionValue <= 0.f;
	SplineMovementComponent->AddToMovementSpeed((bSlowDown ? 120.f : 30.f) * ActionValue);

	UE_LOGFMT(LogWalkPlayer, Verbose, "Speed change input triggered with ActionValue: {0}", ActionValue);
}

void AWalkPawn::Look(const FInputActionValue& Value)
{
	FVector2D ActionValue = Value.Get<FVector2D>();
	SplineMovementComponent->AddCameraRotationOffset(FRotator(ActionValue.Y, ActionValue.X, 0.f));

	UE_LOGFMT(LogWalkPlayer, Verbose, "Look input triggered with ActionValue: {0}", ActionValue.ToString());
}

void AWalkPawn::ToggleMusic(const FInputActionValue& Value)
{
	MusicPlayerComponent->SetPaused(!MusicPlayerComponent->bIsPaused);
	OnMusicStateChanged.Broadcast(MusicPlayerComponent->bIsPaused);

	if (MusicPlayerComponent->bIsPaused)
	{
		UAudioModulationStatics::DeactivateBusMix(GetWorld(), MusicOnBusMix);
	}
	else
	{
		UAudioModulationStatics::ActivateBusMix(GetWorld(), MusicOnBusMix);
	}

	UE_LOGFMT(LogWalkPlayer, Verbose, "Toggle music input triggered");
}

void AWalkPawn::ChangeSong(const FInputActionValue& Value)
{
	if (MusicPlayerComponent->bIsPaused) return;

	MusicPlayerComponent->SetSound(bIsPlayingWaterMask ? MusicTrackParkMask : MusicTrackWaterMask);
	bIsPlayingWaterMask = !bIsPlayingWaterMask;

	UE_LOGFMT(LogWalkPlayer, Verbose, "Change song input triggered");
}

void AWalkPawn::Interact(const FInputActionValue& Value)
{
	AActor* InteractionActor = InteractionComponent->TargetedActor;
	if (!InteractionActor) return;

	IInteractiveActor::Execute_Interact(InteractionActor, this);

	UE_LOGFMT(LogWalkPlayer, Verbose, "Interaction input triggered");
}

void AWalkPawn::GetUp(const FInputActionValue& Value)
{
	if (!SittingComponent->CanPlayerGetUp()) return;
	SittingComponent->OnPlayerGetUp();

	UE_LOGFMT(LogWalkPlayer, Display, "Get up input triggered");
}

void AWalkPawn::SaveGame(const FInputActionValue& Value)
{
	UE_LOGFMT(LogWalkPlayer, Display, "Save game input triggered");
}

