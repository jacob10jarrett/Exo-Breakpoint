#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HighScoreSave.generated.h"

/** Simple “one-int” SaveGame asset to persist the best score */
UCLASS()
class EXOBREAKPOINT_API UHighScoreSave : public USaveGame
{
    GENERATED_BODY()

public:
    /** Highest score ever achieved */
    UPROPERTY(BlueprintReadWrite, Category = "Score")
    int32 HighScore = 0;
};
