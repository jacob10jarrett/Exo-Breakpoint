// WaveManager.cpp

#include "WaveManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();
    StartNextWave();
}

void AWaveManager::StartNextWave()
{
    // compute our fixed quota for this wave
    ToSpawn = BaseEnemiesPerWave + (CurrentWave - 1) * EnemiesIncrementPerWave;
    TotalToSpawn = ToSpawn;
    AliveCount = 0;
    KilledCount = 0;

    // start spawning timer
    GetWorldTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &AWaveManager::SpawnEnemyTick,
        SpawnInterval,
        true);

    // initial UI update
    TickWaveProgress();
}

void AWaveManager::SpawnEnemyTick()
{
    // stop spawning if we've hit the cap, or we have too many on screen
    if (ToSpawn <= 0 || AliveCount >= MaxConcurrentAlive)
    {
        if (ToSpawn <= 0)
        {
            // once cap is reached, clear the spawn timer
            GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        }
        return;
    }

    // choose a random portal (or fallback to the manager's location)
    int32 idx = SpawnPortals.Num() > 0
        ? FMath::RandRange(0, SpawnPortals.Num() - 1)
        : INDEX_NONE;

    FVector loc = SpawnPortals.IsValidIndex(idx)
        ? SpawnPortals[idx]->GetActorLocation()
        : GetActorLocation();
    FRotator rot = SpawnPortals.IsValidIndex(idx)
        ? SpawnPortals[idx]->GetActorRotation()
        : FRotator::ZeroRotator;

    // do the actual spawn
    FActorSpawnParameters params;
    params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (AActor* spawned = GetWorld()->SpawnActor<AActor>(EnemyClass, loc, rot, params))
    {
        ToSpawn--;
        AliveCount++;

        // debug: draw a little sphere so you see where they popped
        DrawDebugSphere(GetWorld(), loc, 75.f, 12, FColor::Red, false, 2.f);

        TickWaveProgress();
    }
}

void AWaveManager::NotifyEnemyKilled()
{
    // job #1: clamp AliveCount down
    AliveCount = FMath::Max(0, AliveCount - 1);
    // job #2: count the kill
    KilledCount++;

    TickWaveProgress();
    CheckWaveEnd();
}

void AWaveManager::AddScore(int32 Amount)
{
    Score += Amount;
    OnScoreChanged.Broadcast(Score);
}

float AWaveManager::GetWaveProgress() const
{
    return (TotalToSpawn > 0)
        ? float(KilledCount) / float(TotalToSpawn)
        : 0.f;
}

void AWaveManager::TickWaveProgress()
{
    OnWaveProgressChanged.Broadcast(GetWaveProgress());
}

void AWaveManager::CheckWaveEnd()
{
    // only advance once we've spawned the full quota AND killed them all
    if (ToSpawn == 0 && AliveCount == 0)
    {
        CurrentWave++;
        StartNextWave();
    }
}
