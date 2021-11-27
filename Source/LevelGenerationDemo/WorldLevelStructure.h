// Copyright Ilya Blokh iblokh.com

#pragma once

#include "CoreMinimal.h"

class ULevelStreamingDynamic;
class ALevelConnector;
class ULevel;

struct FWorldLevel {
	ULevelStreamingDynamic* Level;
	TArray<ALevelConnector*> Connectors;
	FWorldLevel(ULevelStreamingDynamic* InLevel, TArray<ALevelConnector*> InConnectors) {
		Level = InLevel;
		Connectors = InConnectors;
	};

	FWorldLevel() {};
};


class LEVELGENERATIONDEMO_API WorldLevelStructure
{
public:
	WorldLevelStructure();
	~WorldLevelStructure();

	void AddWorldLevelSeed(ULevelStreamingDynamic* Level, TArray<ALevelConnector*> Connectors);
	void GenerateWorld();
	TArray<FWorldLevel> GetGeneratedWorld();
	FWorldLevel FindWorldLevel(ULevel* Level);

private:
	//VARIABLES
	TArray<FWorldLevel> SeedWorldLevels;
	TArray<FWorldLevel> WorldLevels;
	int32 AttemptFailureLimit;
	int32 AttemptFailureCount = 0;
	bool bWorldGenerationComplete = false;

	//FUNCTIONS
	bool AttemptToAddLevelToWorld(FWorldLevel CandidateLevel);
	FWorldLevel GetRandomSeedLevel();
	void RemoveLevelInstanceFromSeed(FWorldLevel Level);

	void DEBUG_LogSeedLevels();
	void DEBUG_LogWorldLevels();
};
