

#pragma once

#include "CoreMinimal.h"
// To use more convenient UE_LOGFMT macro
#include "Logging/StructuredLog.h"

// Header file for global utility functions and defines (such as log categories)

DECLARE_LOG_CATEGORY_EXTERN(LogWalkPlayer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogWalkGameMode, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSplineMovement, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSanity, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMonster, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMonsterSound, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGameplayVolume, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogBench, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSitting, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMusic, Log, All);