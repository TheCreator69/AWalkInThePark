


#include "WalkEventVolume.h"
#include "Components/BoxComponent.h"
#include "../Player/WalkPawn.h"
#include "../Core/WalkDefines.h"

// Sets default values
AWalkEventVolume::AWalkEventVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	VolumeComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	VolumeComponent->SetupAttachment(DefaultSceneRoot);
}

// Called when the game starts or when spawned
void AWalkEventVolume::BeginPlay()
{
	Super::BeginPlay();

	VolumeComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AWalkEventVolume::BeginOverlapCheckIfPlayer);
	VolumeComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AWalkEventVolume::EndOverlapCheckIfPlayer);
}

void AWalkEventVolume::BeginOverlapCheckIfPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWalkPawn* Player = Cast<AWalkPawn>(OtherActor);
	if (Player)
	{
		PlayerBeginOverlapInternal(Player);
		PlayerBeginOverlap(Player);
	}
}

void AWalkEventVolume::EndOverlapCheckIfPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWalkPawn* Player = Cast<AWalkPawn>(OtherActor);
	if (Player)
	{
		PlayerEndOverlapInternal(Player);
		PlayerEndOverlap(Player);
	}
}

void AWalkEventVolume::PlayerBeginOverlapInternal(AWalkPawn* Player)
{
	UE_LOGFMT(LogGameplayVolume, Log, "Player has triggered base class C++ START overlap method for: {0}", GetName());
}

void AWalkEventVolume::PlayerEndOverlapInternal(AWalkPawn* Player)
{
	UE_LOGFMT(LogGameplayVolume, Log, "Player has triggered base class C++ END overlap method for: {0}", GetName());
}

