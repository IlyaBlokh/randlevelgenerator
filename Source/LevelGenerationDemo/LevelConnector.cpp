// Fill out your copyright notice in the Description page of Project Settings.
#include "LevelConnector.h"
#include "Components/StaticMeshComponent.h"

ALevelConnector::ALevelConnector()
{
	PrimaryActorTick.bCanEverTick = false;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);
}

ConnectorTypes ALevelConnector::GetConnectorType()
{
	return ConnectorType;
}

void ALevelConnector::SetConnectorType(ConnectorTypes type)
{
	ConnectorType = type;
}

ALevelConnector* ALevelConnector::GetAttachedConnector()
{
	return AttachedConnector;
}

void ALevelConnector::SetAttachedConnector(ALevelConnector* ConnectorToAttach)
{
	bHasAttachedConnector = true;
	AttachedConnector = ConnectorToAttach;
}

bool ALevelConnector::HasAttachedConnector()
{
	return bHasAttachedConnector;
}

void ALevelConnector::SetWasCoveredInLevelPlacing(bool WasCoveredInLevelPlacing)
{
	bWasCoveredInLevelPlacing = WasCoveredInLevelPlacing;
}

bool ALevelConnector::WasCoveredInLevelPlacing()
{
	return bWasCoveredInLevelPlacing;
}

bool ALevelConnector::IsCompatible(ALevelConnector* OtherConnector)
{
	if (ConnectorType == ConnectorTypes::Left && OtherConnector->ConnectorType == ConnectorTypes::Right ||
		ConnectorType == ConnectorTypes::Right && OtherConnector->ConnectorType == ConnectorTypes::Left ||
		ConnectorType == ConnectorTypes::Up && OtherConnector->ConnectorType == ConnectorTypes::Bottom ||
		ConnectorType == ConnectorTypes::Bottom && OtherConnector->ConnectorType == ConnectorTypes::Up) {
		return true;
	}
	return false;
}

FString ALevelConnector::GetConnectorTypeStr()
{
	switch (ConnectorType) {
	case ConnectorTypes::Left: return TEXT("LEFT");
	case ConnectorTypes::Right: return TEXT("RIGHT");
	case ConnectorTypes::Up: return TEXT("UP");
	case ConnectorTypes::Bottom: return TEXT("BOTTOM");
	default: return TEXT("Undefined");
	}
}

void ALevelConnector::BeginPlay()
{
	Super::BeginPlay();
}
