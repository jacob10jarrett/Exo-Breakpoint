#include "WaveManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"     
#include "HighScoreSave.h"              

const FString AWaveManager::SaveSlotName = TEXT("DefaultHS");            

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();

    LoadHighScore();               
    StartNextWave();
}

/* ---------- Wave lifecycle ---------- */

void AWaveManager::StartNextWave()
{
    ToSpawn = BaseEnemiesPerWave + (CurrentWave - 1) * EnemiesIncrementPerWave;
    TotalToSpawn = ToSpawn;
    AliveCount = 0;
    KilledCount = 0;

    GetWorldTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &AWaveManager::SpawnEnemyTick,
        SpawnInterval,
        true);

    TickWaveProgress();
}

void AWaveManager::SpawnEnemyTick()
{
    if (ToSpawn <= 0 || AliveCount >= MaxConcurrentAlive)
    {
        if (ToSpawn <= 0)
        {
            GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        }
        return;
    }

    int32 idx = SpawnPortals.Num() > 0
        ? FMath::RandRange(0, SpawnPortals.Num() - 1)
        : INDEX_NONE;

    FVector  loc = SpawnPortals.IsValidIndex(idx)
        ? SpawnPortals[idx]->GetActorLocation()
        : GetActorLocation();

    FRotator rot = SpawnPortals.IsValidIndex(idx)
        ? SpawnPortals[idx]->GetActorRotation()
        : FRotator::ZeroRotator;

    FActorSpawnParameters params;
    params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (AActor* spawned = GetWorld()->SpawnActor<AActor>(EnemyClass, loc, rot, params))
    {
        ToSpawn--;
        AliveCount++;
        TickWaveProgress();
    }
}

void AWaveManager::NotifyEnemyKilled()
{
    AliveCount = FMath::Max(0, AliveCount - 1);
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
    if (ToSpawn == 0 && AliveCount == 0)
    {
        CurrentWave++;
        StartNextWave();
    }
}

/* ---------- High-score logic ---------- */

void AWaveManager::LoadHighScore()                                          
{
    CachedHighScore = 0;

    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
    {
        if (UHighScoreSave* Save =
            Cast<UHighScoreSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex)))
        {
            CachedHighScore = Save->HighScore;
        }
    }
}

bool AWaveManager::TryCommitHighScore(int32 CurrentScore)                   
{
    if (CachedHighScore == INDEX_NONE)   // first call safety
    {
        LoadHighScore();
    }

    if (CurrentScore > CachedHighScore)
    {
        CachedHighScore = CurrentScore;

        UHighScoreSave* Save =
            Cast<UHighScoreSave>(UGameplayStatics::CreateSaveGameObject(UHighScoreSave::StaticClass()));
        Save->HighScore = CachedHighScore;

        UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, UserIndex);
        return true;           
    }
    return false;               
}
