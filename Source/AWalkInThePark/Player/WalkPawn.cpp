


#include "WalkPawn.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "SplineMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "../Interaction/InteractionComponent.h"
#include "../Interaction/InteractiveActor.h"
#include "SanityComponent.h"

// Sets default values
AWalkPawn::AWalkPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(DefaultSceneRoot);

	MusicPlayerComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicPlayer"));
	MusicPlayerComponent->SetupAttachment(DefaultSceneRoot);

	LowSanityAmbienceComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LowSanityAmbience"));
	LowSanityAmbienceComponent->SetupAttachment(DefaultSceneRoot);

	SplineMovementComponent = CreateDefaultSubobject<USplineMovementComponent>(TEXT("SplineMovementComponent"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	SanityComponent = CreateDefaultSubobject<USanityComponent>(TEXT("SanityComponent"));
}

// Called when the game starts or when spawned
void AWalkPawn::BeginPlay()
{
	Super::BeginPlay();

	MusicPlayerComponent->SetPaused(true);
	SanityComponent->OnSanityReachedZero.AddDynamic(this, &AWalkPawn::KillPlayer);
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
	Subsystem->AddMappingContext(MappingContext, 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(ChangeSpeedAction, ETriggerEvent::Triggered, this, &AWalkPawn::ChangeSpeed);
	Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWalkPawn::Look);
	Input->BindAction(ToggleMusicAction, ETriggerEvent::Triggered, this, &AWalkPawn::ToggleMusic);
	Input->BindAction(ChangeSongAction, ETriggerEvent::Triggered, this, &AWalkPawn::ChangeSong);
	Input->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AWalkPawn::Interact);
	Input->BindAction(TogglePauseAction, ETriggerEvent::Triggered, this, &AWalkPawn::TogglePause);
}

void AWalkPawn::KillPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("Player is dead!"))
}

void AWalkPawn::ChangeSpeed(const FInputActionValue& Value)
{
	float ActionValue = Value.Get<float>();
	bool bSlowDown = ActionValue < 0.f;
	SplineMovementComponent->AddToMovementSpeed((bSlowDown ? 120.f : 30.f) * ActionValue);
}

void AWalkPawn::Look(const FInputActionValue& Value)
{
	FVector2D ActionValue = Value.Get<FVector2D>();
	SplineMovementComponent->AddCameraRotationOffset(FRotator(ActionValue.Y, ActionValue.X, 0.f));
}

void AWalkPawn::ToggleMusic(const FInputActionValue& Value)
{
	MusicPlayerComponent->SetPaused(!MusicPlayerComponent->bIsPaused);
	OnMusicStateChanged.Broadcast(MusicPlayerComponent->bIsPaused);
}

void AWalkPawn::ChangeSong(const FInputActionValue& Value)
{
	if (MusicPlayerComponent->bIsPaused) return;

	MusicPlayerComponent->SetSound(bIsPlayingWaterMask ? MusicTrackParkMask : MusicTrackWaterMask);
	bIsPlayingWaterMask = !bIsPlayingWaterMask;
}

void AWalkPawn::Interact(const FInputActionValue& Value)
{
	AActor* InteractionActor = InteractionComponent->TargetedActor;
	if (!InteractionActor) return;

	IInteractiveActor::Execute_Interact(InteractionActor, this);
}

void AWalkPawn::TogglePause(const FInputActionValue& Value)
{
	UGameplayStatics::SetGamePaused(GetWorld(), !UGameplayStatics::IsGamePaused(GetWorld()));
}

