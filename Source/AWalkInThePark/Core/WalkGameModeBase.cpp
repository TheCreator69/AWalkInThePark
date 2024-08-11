


#include "WalkGameModeBase.h"
#include "../Core/WalkDefines.h"
#include "../Environment/ParkBench.h"
#include "../Player/WalkPawn.h"
#include "../Player/SanityComponent.h"
#include "../Player/SittingComponent.h"
#include "../Player/SplineMovementComponent.h"
#include "../Monster/WalkMonster.h"
#include "../Monster/ShadowMonster.h"
#include "Kismet/GameplayStatics.h"

void AWalkGameModeBase::ReloadLevelAndRespawnPlayer()
{
	switch (CurrentProgress)
	{
	case EGameProgress::Start:
		ResetGameProgressToStart();
		break;
	case EGameProgress::FirstParkBench:
		ResetGameProgressToFirstBench();
		break;
	case EGameProgress::SecondParkBench:
		ResetGameProgressToSecondBench();
		break;
	case EGameProgress::ThirdParkBench:
		ResetGameProgressToThirdBench();
		break;
	}
}

void AWalkGameModeBase::SetGameProgress(TEnumAsByte<EGameProgress> Progress)
{
	CurrentProgress = Progress;

	UE_LOGFMT(LogWalkGameMode, Display, "Game progress was changed: {0}", Progress.GetValue());
}

void AWalkGameModeBase::ResetGameProgressToStart()
{
	/*
	- Stop sanity from being able to decrease
	- Place player at start of first path
	- Stop both monsters from reactivating
	*/

	AWalkPawn* PlayerPawn = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerPawn) return;

	PlayerPawn->SanityComponent->ProhibitDecreasingSanityAndReset();
	PlayerPawn->SplineMovementComponent->ResetPathProgress();

	if(WaterMonster) WaterMonster->DisableReactivation();
	if(ParkMonster) ParkMonster->DisableReactivation();
}

void AWalkGameModeBase::ResetGameProgressToFirstBench()
{
	/*
	- Stop park monster from reactivating
	- Sit player down at correct park bench
	*/

	if (ParkMonster) ParkMonster->DisableReactivation();

	AWalkPawn* PlayerPawn = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerPawn) return;

	PlayerPawn->SittingComponent->SitDownForRespawn(FirstParkBench);
}

void AWalkGameModeBase::ResetGameProgressToSecondBench()
{
	/*
	- Sit player down at correct park bench
	*/
	AWalkPawn* PlayerPawn = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerPawn) return;

	PlayerPawn->SittingComponent->SitDownForRespawn(SecondParkBench);
}

void AWalkGameModeBase::ResetGameProgressToThirdBench()
{
	/*
	- Reset shadow monster's location
	- Sit player down at correct park bench
	*/

	ShadowMonster->ResetShadowOnRespawn();

	AWalkPawn* PlayerPawn = Cast<AWalkPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerPawn) return;

	PlayerPawn->SittingComponent->SitDownForRespawn(ThirdParkBench);
}
