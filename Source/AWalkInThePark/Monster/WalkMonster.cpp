


#include "WalkMonster.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/RandomStream.h"
#include "../Player/WalkPawn.h"
#include "../Player/SplineMovementComponent.h"
#include "PlayerFollowComponent.h"
#include "MonsterFootstepAudioComponent.h"
#include "../Core/WalkDefines.h"

// Sets default values
AWalkMonster::AWalkMonster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	DebugCube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugCube"));
	DebugCube->SetupAttachment(DefaultSceneRoot);

	FootstepAudioComponent = CreateDefaultSubobject<UMonsterFootstepAudioComponent>(TEXT("FootstepAudioComponent"));
	FootstepAudioComponent->SetupAttachment(DefaultSceneRoot);

	PlayerFollowComponent = CreateDefaultSubobject<UPlayerFollowComponent>(TEXT("PlayerFollowComponent"));
}

// Called when the game starts or when spawned
void AWalkMonster::BeginPlay()
{
	Super::BeginPlay();

	AWalkPawn* Player = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player) return;

	Player->OnBeginOverlapSafeZone.AddUniqueDynamic(this, &AWalkMonster::DeactivateMonster);
	Player->OnEndOverlapSafeZone.AddUniqueDynamic(this, &AWalkMonster::ReactivateMonster);
}

void AWalkMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(AggressionChangeTimerHandle);
}

void AWalkMonster::ActivateMonster()
{
	bCanBeReactivated = true;

	GetWorld()->GetTimerManager().SetTimer(AggressionChangeTimerHandle, this, &AWalkMonster::ChangeAggressionPeriodically, AggressionChangeRate, true);
	UE_LOGFMT(LogMonster, Display, "Activated monster for the first time");
}

void AWalkMonster::ReactivateMonster()
{
	if (!bCanBeReactivated) return;

	GetWorld()->GetTimerManager().SetTimer(AggressionChangeTimerHandle, this, &AWalkMonster::ChangeAggressionPeriodically, AggressionChangeRate, true);
	UE_LOGFMT(LogMonster, Display, "Reactivated monster");
}

void AWalkMonster::DeactivateMonster()
{
	GetWorld()->GetTimerManager().ClearTimer(AggressionChangeTimerHandle);
	SetAggression(0.0);
	UE_LOGFMT(LogMonster, Display, "Deactivated monster");
}

double AWalkMonster::GetAggression() const
{
	return Aggression;
}

void AWalkMonster::SetAggression(double NewAggression)
{
	Aggression = FMath::Clamp(NewAggression, 0.f, 1.f);
	if (Aggression == 1.f)
	{
		AWalkPawn* PlayerPawn = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (!PlayerPawn) return;

		TEnumAsByte<EPlayerDeathReason> DeathReason = bIsWaterMonster 
			? TEnumAsByte<EPlayerDeathReason>(WaterMonster) 
			: TEnumAsByte<EPlayerDeathReason>(ParkMonster);

		PlayerPawn->KillPlayer(DeathReason);
	}
	UE_LOGFMT(LogMonster, Verbose, "Aggression set: {0}", Aggression);
}

void AWalkMonster::ChangeAggressionPeriodically()
{
	if (IsPlayerStaringDownMonster())
	{
		FAggressionChangeInfo* Info = &AggressionIncreaseInfo;
		double AggressionChange = CalculateAggressionChange(true, Info->BaseChange, Info->OffsetMin, Info->OffsetMax);
		SetAggression(Aggression - AggressionChange);
		UE_LOGFMT(LogMonster, Verbose, "Player is staring down monster: {0}", GetName());
	}
	else
	{
		FAggressionChangeInfo* Info = &AggressionDecreaseInfo;
		double AggressionChange = CalculateAggressionChange(false, Info->BaseChange, Info->OffsetMin, Info->OffsetMax);
		SetAggression(Aggression + AggressionChange);
		UE_LOGFMT(LogMonster, Verbose, "Player is NOT staring down monster: {0}", GetName());
	}
}

double AWalkMonster::CalculateAggressionChange(bool bLowerAggression, float SpeedPercentageMultiplier, double OffsetMin, double OffsetMax)
{
	double AggressionChange = 0.0;

	AWalkPawn* PlayerPawn = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerPawn) return AggressionChange;

	float SpeedPercentage = PlayerPawn->SplineMovementComponent->GetPlayerSpeedPercentage();
	// If a curve asset is preset, use it to scale the aggression change, if not, do it linearly as a backup
	if (AggressionChangeCurve)
	{
		float inTime = bLowerAggression ? 1.f - SpeedPercentage : SpeedPercentage;
		AggressionChange = AggressionChangeCurve->GetFloatValue(inTime);
	}
	else
	{
		AggressionChange = SpeedPercentage;
	}
	AggressionChange *= SpeedPercentageMultiplier;
	AggressionChange += FMath::RandRange(OffsetMin, OffsetMax);

	return AggressionChange;
}

bool AWalkMonster::IsPlayerStaringDownMonster()
{
	AWalkPawn* PlayerPawn = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerPawn) return false;

	// Player has to move below 50% speed to intimidate monsters (otherwise there'd be no reason to slow down)
	if (PlayerPawn->SplineMovementComponent->GetPlayerSpeedPercentage() > 0.5) return false;

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerPawn->GetActorLocation(), GetActorLocation());
	double DotProduct = FVector::DotProduct(UKismetMathLibrary::GetForwardVector(PlayerPawn->GetControlRotation()), UKismetMathLibrary::GetForwardVector(LookAtRotation));

	// If player looks directly at monster, dot product is 1, if player looks directly away it's -1.
	// So it needs to be close to 1 to pass the "player looking at monster" check
	if (DotProduct > 0.7) return true;
	return false;
}

