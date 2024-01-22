


#include "WalkMonster.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/RandomStream.h"
#include "../Player/WalkPawn.h"
#include "../Player/SplineMovementComponent.h"

// Sets default values
AWalkMonster::AWalkMonster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	DebugCube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugCube"));
	DebugCube->SetupAttachment(DefaultSceneRoot);
}

// Called when the game starts or when spawned
void AWalkMonster::BeginPlay()
{
	Super::BeginPlay();

	// Temporary - only used to test monster aggression
	ActivateMonster();
}

void AWalkMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(AggressionChangeTimerHandle);
}

void AWalkMonster::ActivateMonster()
{
	GetWorld()->GetTimerManager().SetTimer(AggressionChangeTimerHandle, this, &AWalkMonster::ChangeAggressionPeriodically, AggressionChangeRate, true);
}

void AWalkMonster::DeactivateMonster()
{
	GetWorld()->GetTimerManager().ClearTimer(AggressionChangeTimerHandle);
	Aggression = 0.f;
}

void AWalkMonster::SetAggression(double NewAggression)
{
	Aggression = FMath::Clamp(NewAggression, 0.f, 1.f);
	if (Aggression == 1.f)
	{
		AWalkPawn* PlayerPawn = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (!PlayerPawn) return;

		PlayerPawn->KillPlayer();
	}
}

void AWalkMonster::ChangeAggressionPeriodically()
{
	if (IsPlayerStaringDownMonster())
	{
		FAggressionChangeInfo* Info = &AggressionIncreaseInfo;
		double AggressionChange = CalculateAggressionChange(true, Info->BaseChange, Info->OffsetMin, Info->OffsetMax);
		SetAggression(Aggression - AggressionChange);
	}
	else
	{
		FAggressionChangeInfo* Info = &AggressionDecreaseInfo;
		double AggressionChange = CalculateAggressionChange(false, Info->BaseChange, Info->OffsetMin, Info->OffsetMax);
		SetAggression(Aggression + AggressionChange);
	}
	UE_LOG(LogTemp, Warning, TEXT("Aggression: %f"), Aggression)
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

	// Add a random offset in a range to make monster more unpredictable
	FRandomStream Random = FRandomStream(1337);
	AggressionChange += Random.FRandRange(OffsetMin, OffsetMax);

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
