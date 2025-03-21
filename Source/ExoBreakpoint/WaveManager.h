#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);

UCLASS(Blueprintable)
class EXOBREAKPOINT_API AWaveManager : public AActor
{
    GENERATED_BODY()
    
public:    
    AWaveManager();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartNextWave();
    
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void SpawnEnemy();

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 GetScore() const { return Score; }

    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnScoreChanged OnScoreChanged;

private:
    int32 CurrentWave;

    int32 EnemiesRemaining;

    int32 Score;

    float SpawnInterval;

    FTimerHandle SpawnTimer;

    UPROPERTY(EditAnywhere, Category="Wave Settings")
    TSubclassOf<AActor> EnemyClass;

    UPROPERTY(EditAnywhere, Category="Wave Settings")
    FVector SpawnLocation;
};
