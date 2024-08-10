


#include "WalkGameModeBase.h"
#include "../Core/WalkDefines.h"

void AWalkGameModeBase::ReloadLevelAndRespawnPlayer()
{
	switch (CurrentProgress)
	{
	case Start:
		ResetGameProgressToStart();
		break;
	case FirstParkBench:
		ResetGameProgressToFirstBench();
		break;
	case SecondParkBench:
		ResetGameProgressToSecondBench();
		break;
	case ThirdParkBench:
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
	TODO:
	- Deactivate sanity decreasing
	- Deactivate both monsters
	*/
}

void AWalkGameModeBase::ResetGameProgressToFirstBench()
{
	/*
	TODO:
	- Deactivate park monster
	- Sit player down at correct park bench
	*/
}

void AWalkGameModeBase::ResetGameProgressToSecondBench()
{
	/*
	TODO:
	- Sit player down at correct park bench
	*/
}

void AWalkGameModeBase::ResetGameProgressToThirdBench()
{
	/*
	TODO:
	- Rewind and stop shadow monster
	*/
}
