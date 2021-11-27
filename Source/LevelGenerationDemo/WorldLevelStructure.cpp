// Copyright Ilya Blokh iblokh.com

#include "WorldLevelStructure.h"
#include "LevelConnector.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/Level.h"

WorldLevelStructure::WorldLevelStructure()
{
}

WorldLevelStructure::~WorldLevelStructure()
{
}

void WorldLevelStructure::AddWorldLevelSeed(ULevelStreamingDynamic* Level, TArray<ALevelConnector*> Connectors)
{
	SeedWorldLevels.Add(FWorldLevel(Level, Connectors));
}

void WorldLevelStructure::GenerateWorld()
{
	DEBUG_LogSeedLevels();
	UE_LOG(LogTemp, Warning, TEXT("=====GENERATING WORLD====="));
	//empiric value, could be changed
	AttemptFailureLimit = SeedWorldLevels.Num();//TODO: * count
	while (!bWorldGenerationComplete)
	{
		UE_LOG(LogTemp, Warning, TEXT("----GENERATION CYCLE----"));
		FWorldLevel CandidateLevel = GetRandomSeedLevel();
		UE_LOG(LogTemp, Warning, TEXT("Level candidate: %s"), *CandidateLevel.Level->GetLoadedLevel()->GetFName().ToString());
		bool bAttemptSucceed = AttemptToAddLevelToWorld(CandidateLevel);
		// if succeed: reduce count-to-load. if count == 0 then remove level from seed
		if (bAttemptSucceed) {
			UE_LOG(LogTemp, Warning, TEXT("Level add SUCCESS"));
			RemoveLevelInstanceFromSeed(CandidateLevel);
			DEBUG_LogSeedLevels();
			if (SeedWorldLevels.Num() == 0)
				bWorldGenerationComplete = true;
		}
		// if failed: increase count of failures. If count > limit then exit
		else {
			AttemptFailureCount++;
			UE_LOG(LogTemp, Warning, TEXT("Level add FAILURE; failure count is %s"), AttemptFailureCount);
			if (AttemptFailureCount > AttemptFailureLimit)
				bWorldGenerationComplete = true;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("=====GENERATING COMPLETE====="));
	DEBUG_LogWorldLevels();
}

TArray<FWorldLevel> WorldLevelStructure::GetGeneratedWorld()
{
	return WorldLevels;
}

FWorldLevel WorldLevelStructure::FindWorldLevel(ULevel* Level)
{
	for (FWorldLevel& WorldLevel : WorldLevels) {
		if (WorldLevel.Level->GetLoadedLevel() == Level)
			return WorldLevel;
	}
	return FWorldLevel();
}

/// <summary>
/// Tries to add Level into World if approtiate Connectors exits
/// </summary>
/// <param name="LevelCandidate">Level to be added</param>
/// <returns>true if succeed, false if failed to add </returns>
bool WorldLevelStructure::AttemptToAddLevelToWorld(FWorldLevel CandidateLevel)
{
	//TODO: set location for first level
	if (WorldLevels.Num() == 0) {
		WorldLevels.Add(CandidateLevel);
		return true;
	}
	for (int32 i = 0; i < WorldLevels.Num(); i++) {
		FWorldLevel& CurrentLevel = WorldLevels[i];
		//TODO: randomize / prioritize the order of connector selection
		for (ALevelConnector* CurrentConnector : CurrentLevel.Connectors) {
			for (ALevelConnector* CandidateConnector : CandidateLevel.Connectors) {
				if (CurrentConnector->HasAttachedConnector()) {
					UE_LOG(LogTemp, Warning, TEXT("CurrentConnector is already atteched to %s"), *CurrentConnector->GetAttachedConnector()->GetLevel()->GetFName().ToString());
					continue;
				}
				if (CandidateConnector->IsCompatible(CurrentConnector))
				{
					CandidateConnector->SetAttachedConnector(CurrentConnector);
					CurrentConnector->SetAttachedConnector(CandidateConnector);
					WorldLevels.Add(CandidateLevel);
					//TODO: set location for candidate level
					return true;
				}
			}
		}
	}
	return false;
}

FWorldLevel WorldLevelStructure::GetRandomSeedLevel()
{
	int32 Index = FMath::RandRange(0, SeedWorldLevels.Num() - 1);
	return SeedWorldLevels[Index];
}

void WorldLevelStructure::RemoveLevelInstanceFromSeed(FWorldLevel Level)
{
	//TODO: fix bug - it removes only if Level is first level in seed array
	for (int32 i = 0; i < SeedWorldLevels.Num(); i++) {
		FWorldLevel& SeedLevel = SeedWorldLevels[i];
		if (SeedLevel.Level == Level.Level) {
			SeedWorldLevels.RemoveAt(i);
			return;
		}
	}
}

//DEBUG FUNCTIONS
void WorldLevelStructure::DEBUG_LogSeedLevels()
{
	UE_LOG(LogTemp, Warning, TEXT("=====PRINT SEED LEVELS====="));
	for (FWorldLevel& SeedLevel : SeedWorldLevels) {
		FString LevelName = SeedLevel.Level->GetLoadedLevel()->GetFName().ToString();
		UE_LOG(LogTemp, Warning, TEXT("Level %s Location: %s"), *LevelName, *SeedLevel.Level->LevelTransform.GetLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("Level %s has %i connectors:"), *LevelName, SeedLevel.Connectors.Num());
		for (ALevelConnector* LevelConnector : SeedLevel.Connectors) {
			FVector Location = LevelConnector->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("    LevelConnector has location %s"), *Location.ToString());
		}
	}
}

void WorldLevelStructure::DEBUG_LogWorldLevels()
{
	UE_LOG(LogTemp, Warning, TEXT("=====PRINT WORLD LEVELS====="));
	for (FWorldLevel& Level : WorldLevels) {
		FString LevelName = Level.Level->GetLoadedLevel()->GetFName().ToString();
		UE_LOG(LogTemp, Warning, TEXT("Level %s"), *LevelName);
		for (ALevelConnector* LevelConnector : Level.Connectors) {
			FString SType = LevelConnector->GetConnectorTypeStr();
			UE_LOG(LogTemp, Warning, TEXT("    LevelConnector type %s"), *SType);
			ALevelConnector* AttachedConnector = LevelConnector->GetAttachedConnector();
			if (AttachedConnector) {
				UE_LOG(LogTemp, Warning, TEXT("    is attached to connector from %s"), *AttachedConnector->GetLevel()->GetFName().ToString());
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("    not attached"));
		}
	}
}
