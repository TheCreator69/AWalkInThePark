


#include "WalkSafeZone.h"
#include "../Core/WalkDefines.h"
#include "../Player/WalkPawn.h"

void AWalkSafeZone::PlayerBeginOverlapInternal(AWalkPawn* Player)
{
	Player->OnBeginOverlapSafeZone.Broadcast();

	UE_LOGFMT(LogGameplayVolume, Display, "Player has started overlapping safe zone: {0}", GetName());
}

void AWalkSafeZone::PlayerEndOverlapInternal(AWalkPawn* Player)
{
	Player->OnEndOverlapSafeZone.Broadcast();

	UE_LOGFMT(LogGameplayVolume, Display, "Player has stopped overlapping safe zone: {0}", GetName());
}