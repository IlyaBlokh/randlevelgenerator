// Copyright Ilya Blokh iblokh.com

#include "LevelStreamer.h"
#include "LevelConnector.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/LevelStreamingDynamic.h"


#define OUT

ALevelStreamer::ALevelStreamer()
{
	PrimaryActorTick.bCanEverTick = false;
	srand(FDateTime::Now().ToUnixTimestamp());

	//debug function to generate levels in random order
	//LevelsToLoad = ShufflePresets(LevelsToLoad);
}

void ALevelStreamer::BeginPlay()
{
	Super::BeginPlay();
	LoadLevels();
	WorldLevel = WorldLevelStructure();
}

//----------------------------------------------------------------------//
//-----------------------MAIN FUNCTIONS---------------------------------//
//----------------------------------------------------------------------//

//TODO: set shift and location based on level size and level connectors
void ALevelStreamer::LoadLevels()
{
	UE_LOG(LogTemp, Warning, TEXT("Sublevel seed:"));

	int32 NamesCount = LevelsToLoad.Num();
	float SpawnShift = 2000.f;
	float InitialShift = 0.f;
	bool bLoaded = true;
	for (int32 i = 0; i < NamesCount; i++) {
		int32 CountToLoad = FMath::RandRange(LevelsToLoad[i].MinCount, LevelsToLoad[i].MaxCount);
		UE_LOG(LogTemp, Warning, TEXT("Level %s count %i"), *LevelsToLoad[i].LevelName, CountToLoad);

		for (int32 j = 0; j < CountToLoad; j++) {
			ULevelStreamingDynamic* Level = ULevelStreamingDynamic::LoadLevelInstance(this, LevelsToLoad[i].LevelName, FVector(InitialShift, 0, 0), FRotator().ZeroRotator, OUT bLoaded);
			Level->OnLevelShown.AddDynamic(this, &ALevelStreamer::OnLevelInstanceAddedToWorld);
			//TODO: hide levels before applying final transformation
			//Level->SetShouldBeVisible(false);
			LoadedLevels.Emplace(Level);
			//InitialShift += SpawnShift;
		}
	}
}

void ALevelStreamer::OnLevelInstanceAddedToWorld()
{
	UE_LOG(LogTemp, Warning, TEXT("Level is loaded"));
	if (++LevelsLoaded == LoadedLevels.Num())
		OnAllLevesLoaded();
}

void ALevelStreamer::OnAllLevesLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("We have %i elements in LoadedLevels"), LoadedLevels.Num());
	//Find all Connectors loaded in the world
	UGameplayStatics::GetAllActorsOfClass(this, ALevelConnector::StaticClass(), OUT LoadedConnectors);

	//setup seed before world level generation
	for (int32 i = 0; i < LoadedLevels.Num(); i++) {
		ULevelStreamingDynamic* Level = LoadedLevels[i];
		//rename level
		ULevel* LoadedLevel = Level->GetLoadedLevel();
		LoadedLevel->Rename(*CreateLevelLogicalName(LoadedLevel->GetFullName(), i));
		FString LevelName = Level->GetLoadedLevel()->GetFName().ToString();
		// find connectors related to current level 
		TArray<ALevelConnector*> LevelConnectors;
		for (AActor* LevelConnectorActor : LoadedConnectors) {
			ALevelConnector* LevelConnector = Cast<ALevelConnector>(LevelConnectorActor);
			if (LoadedLevel == LevelConnector->GetLevel()) {
				LevelConnectors.Add(LevelConnector);
			}
		}
		// init worldlevel structure with following level and it's connectors
		WorldLevel.AddWorldLevelSeed(Level, LevelConnectors);
	}

	WorldLevel.GenerateWorld();
	PlaceWorldLevels();
}

void ALevelStreamer::PlaceWorldLevels()
{
	//TODO: put into BP variable
	FVector FirstLevelLocation = FVector(0, 0, 0);
	PlaceLevel(WorldLevel.GetGeneratedWorld()[0], FirstLevelLocation);
}

void ALevelStreamer::PlaceLevel(FWorldLevel Level, FVector Location)
{
	Level.Level->GetLoadedLevel()->ApplyWorldOffset(Location, false);
	for (ALevelConnector* Connector : Level.Connectors) {
		if (!Connector->WasCoveredInLevelPlacing() && Connector->HasAttachedConnector()) {
			FWorldLevel LevelToPlace = WorldLevel.FindWorldLevel(Connector->GetAttachedConnector()->GetLevel());
			if (!LevelToPlace.Level) {
				UE_LOG(LogTemp, Error, TEXT("Can't find FWorldLevel for Level %s"), *Connector->GetAttachedConnector()->GetLevel()->GetFName().ToString());
				continue;
			}
			//TODO: fix location estimation - use level width / distance from connector to level center *2 ?
			Connector->SetWasCoveredInLevelPlacing(true);
			Connector->GetAttachedConnector()->SetWasCoveredInLevelPlacing(true);
			FVector LocationToPlace = Connector->GetActorLocation() - Connector->GetAttachedConnector()->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Attached connector type: %s"), *Connector->GetConnectorTypeStr());
			UE_LOG(LogTemp, Warning, TEXT("WorldOffset for Level %s is %s"), *Connector->GetAttachedConnector()->GetLevel()->GetFName().ToString(), *LocationToPlace.ToString());
			PlaceLevel(LevelToPlace, LocationToPlace);
		}
	}
}

//----------------------------------------------------------------------//
//---------------------SERVICE FUNCTIONS--------------------------------//
//----------------------------------------------------------------------//

/// <summary>
/// DEBUG FUNCTION.Sort preset level data in random range
/// </summary>
/// <param name="Presets">preset contains level name and possible range of count to be generated at level</param>
/// <returns></returns>

TArray<FPreset> ALevelStreamer::ShufflePresets(TArray<FPreset> Presets)
{
	int32 PresetsCount = Presets.Num();

	for (int32 i = 0; i < PresetsCount; i++)
	{
		int32 index = rand() % PresetsCount;
		FPreset temp = Presets[i];
		Presets[i] = Presets[index];
		Presets[index] = temp;
	}

	return Presets;
}

/// <summary>
/// Generate short logical name for sublevel idential to it's name in Editor
/// </summary>
/// <param name="GetFullName">Pass ULevel*->GetFullName here</param>
/// <returns></returns>
FString ALevelStreamer::CreateLevelLogicalName(FString LevelFullName, int32 Index)
{
	FString StartChar = TEXT(".");
	FString EndChar = TEXT(":");
	int32 FirstIdx = LevelFullName.Find(StartChar, ESearchCase::IgnoreCase, ESearchDir::Type::FromStart, 0);
	int32 SecondIdx = LevelFullName.Find(EndChar, ESearchCase::IgnoreCase, ESearchDir::Type::FromStart, FirstIdx + 1);
	if (FirstIdx == -1 || SecondIdx == -1)
		return LevelFullName;
	int32 NameLength = (SecondIdx - FirstIdx) - 1;
	FString FinalName = LevelFullName.Mid(FirstIdx + 1, NameLength);
	FinalName.AppendInt(Index);
	return FinalName;
}