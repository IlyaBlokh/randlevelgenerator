// Copyright Ilya Blokh (c)
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelConnector.generated.h"

UENUM(BlueprintType)
enum class ConnectorTypes : uint8
{
	Left	UMETA(DisplayName = "Left"),
	Right	UMETA(DisplayName = "Right"),
	Up	UMETA(DisplayName = "Up"),
	Bottom	UMETA(DisplayName = "Bottom"),
};

UCLASS()
class LEVELGENERATIONDEMO_API ALevelConnector : public AActor
{
	GENERATED_BODY()

public:
	ALevelConnector();

	ConnectorTypes GetConnectorType();
	void SetConnectorType(ConnectorTypes type);
	ALevelConnector* GetAttachedConnector();
	void SetAttachedConnector(ALevelConnector* ConnectorToAttach);
	bool HasAttachedConnector();
	void SetWasCoveredInLevelPlacing(bool WasCoveredInLevelPlacing);
	bool WasCoveredInLevelPlacing();
	bool IsCompatible(ALevelConnector* OtherConnector);
	FString GetConnectorTypeStr();

protected:
	virtual void BeginPlay() override;

private:
	ALevelConnector* AttachedConnector;
	bool bHasAttachedConnector = false;
	bool bWasCoveredInLevelPlacing = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection", meta = (AllowPrivateAccess = "true"))
		ConnectorTypes ConnectorType;
};
