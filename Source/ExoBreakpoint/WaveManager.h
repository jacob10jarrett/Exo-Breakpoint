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

    /* ---------- Gameplay API ---------- */
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartNextWave();

    UFUNCTION()
    void SpawnEnemyTick();

    UFUNCTION(BlueprintCallable, Category = "Wave")
    void NotifyEnemyKilled();

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    UFUNCTION(BlueprintPure, Category = "Score")                              
        int32 GetScore() const { return Score; }                                

    UFUNCTION(BlueprintPure, Category = "Wave")
    float GetWaveProgress() const;

    UFUNCTION(BlueprintPure, Category = "Wave")
    int32 GetCurrentWave() const { return CurrentWave; }

    /* ----- High-score helpers (Blueprint or C++ callers) ----- */
    UFUNCTION(BlueprintCallable, Category = "Score")                         
        bool TryCommitHighScore(int32 CurrentScore);                            

    /* ---------- Broadcasts ---------- */
    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnScoreChanged OnScoreChanged;

    UPROPERTY(BlueprintAssignable, Category = "Wave")
    FOnWaveProgressChanged OnWaveProgressChanged;

protected:
    virtual void BeginPlay() override;

private:
    /* ---------- Configurable data ---------- */
    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    TSubclassOf<AActor> EnemyClass;

    UPROPERTY(EditAnywhere, Category = "Wave Settings|Spawn")
    TArray<AActor*> SpawnPortals;

    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    float SpawnInterval = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    int32 BaseEnemiesPerWave = 10;

    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    int32 EnemiesIncrementPerWave = 5;

    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    int32 MaxConcurrentAlive = 5;

    /* ---------- Runtime state ---------- */
    int32 CurrentWave = 1;
    int32 ToSpawn = 0;
    int32 AliveCount = 0;
    int32 KilledCount = 0;
    int32 TotalToSpawn = 0;
    int32 Score = 0;

    FTimerHandle SpawnTimerHandle;

    /* ---------- Internal helpers ---------- */
    void TickWaveProgress();
    void CheckWaveEnd();

    /* ---------- High-score internals ---------- */
    static const FString SaveSlotName;                                       
    static const uint32  UserIndex = 0;                                      

    int32 CachedHighScore = INDEX_NONE;  // “not loaded yet” sentinel       
    void  LoadHighScore();                                                   
};
