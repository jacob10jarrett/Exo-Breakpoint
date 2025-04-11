#include "WaveManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentWave = 1;
    Score = 0;
    SpawnInterval = 10.0f;
    DisplayWaveProgress = 0.0f;
    TotalEnemiesInWave = 0;
    EnemiesRemaining = 0;
    WaveStartTime = 0.0f;
    WaveDuration = 0.0f;
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
    TotalEnemiesInWave = CurrentWave * 5;
    EnemiesRemaining = TotalEnemiesInWave;
    WaveStartTime = GetWorld()->GetTimeSeconds();
    WaveDuration = SpawnInterval * TotalEnemiesInWave;
    DisplayWaveProgress = 0.0f;

    GetWorldTimerManager().SetTimer(SpawnTimer, this, &AWaveManager::SpawnEnemy, SpawnInterval, true);
    GetWorldTimerManager().SetTimer(WaveProgressTimer, this, &AWaveManager::UpdateWaveProgress, 1.0f, true);
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
        }
    }
}

void AWaveManager::UpdateWaveProgress()
{
    float Elapsed = GetWorld()->GetTimeSeconds() - WaveStartTime;
    DisplayWaveProgress = FMath::Clamp(Elapsed / WaveDuration, 0.0f, 1.0f);

    if (DisplayWaveProgress >= 1.0f)
    {
        GetWorldTimerManager().ClearTimer(WaveProgressTimer);
        CurrentWave++;
        StartNextWave();
    }
}

void AWaveManager::AddScore(int32 Amount)
{
    Score += Amount;
    OnScoreChanged.Broadcast(Score);
}

float AWaveManager::GetWaveProgress() const
{
    return (TotalEnemiesInWave > 0) ? 1.0f - ((float)EnemiesRemaining / (float)TotalEnemiesInWave) : 1.0f;
}
