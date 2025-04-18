// WaveManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveProgressChanged, float, Progress);

UCLASS(Blueprintable)
class EXOBREAKPOINT_API AWaveManager : public AActor
{
    GENERATED_BODY()

public:
    AWaveManager();

protected:
    virtual void BeginPlay() override;

public:
    /** Starts (or restarts) the next wave */
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartNextWave();

    /** Spawns one enemy (fired by the internal timer) */
    UFUNCTION()
    void SpawnEnemyTick();

    /** Call this *once* from your BP_AI_CharSoldier when it actually dies */
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void NotifyEnemyKilled();

    /** Add to the player's score */
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    /** Total kills fraction: [0..1] */
    UFUNCTION(BlueprintCallable, Category = "Wave")
    float GetWaveProgress() const;

    /** Which wave you’re on (1,2,3…) */
    UFUNCTION(BlueprintCallable, Category = "Wave")
    int32 GetCurrentWave() const { return CurrentWave; }

    /** Fired when the score changes */
    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnScoreChanged OnScoreChanged;

    /** Fired when the wave?progress fraction changes */
    UPROPERTY(BlueprintAssignable, Category = "Wave")
    FOnWaveProgressChanged OnWaveProgressChanged;

private:
    /** The enemy Blueprint to spawn */
    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    TSubclassOf<AActor> EnemyClass;

    /** Drop your four (or more) portals here in the level */
    UPROPERTY(EditAnywhere, Category = "Wave Settings|Spawn")
    TArray<AActor*> SpawnPortals;

    /** Seconds between spawns */
    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    float SpawnInterval = 1.5f;

    /** Base kills needed in wave 1 */
    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    int32 BaseEnemiesPerWave = 10;

    /** + kills each subsequent wave */
    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    int32 EnemiesIncrementPerWave = 5;

    /** Never have more than this many alive at once */
    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    int32 MaxConcurrentAlive = 5;

    // ** Runtime state **
    int32 CurrentWave = 1;
    int32 ToSpawn = 0;  // how many left to spawn
    int32 AliveCount = 0;  // how many currently living
    int32 KilledCount = 0;  // how many have been killed this wave
    int32 TotalToSpawn = 0;  // cached for UI fraction
    int32 Score = 0;  // player score

    FTimerHandle SpawnTimerHandle;

    /** Broadcasts OnWaveProgressChanged */
    void TickWaveProgress();

    /** Checks for end?of?wave and advances if done */
    void CheckWaveEnd();
};
