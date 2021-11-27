// Copyright Ilya Blokh iblokh.com
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldLevelStructure.h"
#include "LevelStreamer.generated.h"

class ULevelStreamingDynamic;
class ALevelConnector;

//Preset resperents seed data for level streaming
USTRUCT(BlueprintType)
struct FPreset {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere)
		FString LevelName;
	UPROPERTY(EditAnywhere)
		int32 MinCount;
	UPROPERTY(EditAnywhere)
		int32 MaxCount;
};

UCLASS()
class LEVELGENERATIONDEMO_API ALevelStreamer : public AActor
{
	GENERATED_BODY()

public:
	ALevelStreamer();

protected:
	virtual void BeginPlay() override;

private:
	//VARIABLES
	UPROPERTY(EditAnywhere, Category = "Streaming")
		TArray<FPreset> LevelsToLoad;

	int32 LevelsLoaded = 0;
	TArray<AActor*> LoadedConnectors;
	TArray<ULevelStreamingDynamic*> LoadedLevels;
	WorldLevelStructure WorldLevel;

	//FUNCTIONS
	TArray<FPreset> ShufflePresets(TArray<FPreset> Presets);
	void LoadLevels();
	void OnAllLevesLoaded();
	void PlaceWorldLevels();
	void PlaceLevel(FWorldLevel Level, FVector Location);

	UFUNCTION()
		void OnLevelInstanceAddedToWorld();
	FString CreateLevelLogicalName(FString LevelFullName, int32 Index);
};