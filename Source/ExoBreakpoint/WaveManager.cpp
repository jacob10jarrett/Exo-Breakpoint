#include "WaveManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentWave = 1;
    EnemiesRemaining = 5;
    SpawnInterval = 2.0f;
    Score = 0;
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();

    StartNextWave();
}

void AWaveManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWaveManager::StartNextWave()
{
    EnemiesRemaining = CurrentWave * 5;

    GetWorldTimerManager().SetTimer(SpawnTimer, this, &AWaveManager::SpawnEnemy, SpawnInterval, true);
}

void AWaveManager::SpawnEnemy()
{
    if (EnemiesRemaining > 0 && EnemyClass)
    {
        GetWorld()->SpawnActor<AActor>(EnemyClass, SpawnLocation, FRotator::ZeroRotator);
        EnemiesRemaining--;

        if (EnemiesRemaining == 0)
        {
            GetWorldTimerManager().ClearTimer(SpawnTimer);
            CurrentWave++;
            StartNextWave();
        }
    }
}

void AWaveManager::AddScore(int32 Amount)
{
    Score += Amount;
    UE_LOG(LogTemp, Log, TEXT("New Score: %d"), Score);

    OnScoreChanged.Broadcast(Score);
}
