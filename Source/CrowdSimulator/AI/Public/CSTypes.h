// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSTypes.generated.h"


UENUM()
enum ECSAgentHighlightType
{
	AgentDestroy,
	AgentNoHighlight
};

UENUM()
enum ECSAgentMovementType
{
	Walk,
	Run,
};

UENUM ( BlueprintType )
enum ECSAgentAge : int
{
	Kid,
	Adult,
	Old
};

USTRUCT ( BlueprintType )
struct FCSAgentSpeedParameters
{
	GENERATED_BODY()
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	float WalkSpeed = 150.f;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	float RunSpeed = 150.f;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	TMap <TEnumAsByte<ECSAgentAge>, float> WalkSpeedMap = { { Kid, 100.f },
											 { Adult, 150.f },
											 { Old, 80.f }
											 };

	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	TMap <TEnumAsByte<ECSAgentAge>, float> RunSpeedMap = { { Kid, 100.f },
											 { Adult, 150.f },
											 { Old, 80.f }
	};
};

USTRUCT ( BlueprintType )
struct FCSAgentParameters
{
	GENERATED_BODY()

	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	TEnumAsByte<ECSAgentAge> Age = Adult;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	int GroupId = 0;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	FCSAgentSpeedParameters AgentSpeedParameters;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	bool HasSpawnTransform = false; 
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	FTransform SpawnTransform = FTransform::Identity; 
	
};

USTRUCT ( BlueprintType )
struct FCSNavigationPointParameters
{
	GENERATED_BODY();
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	int32 GoalGroupId = 0;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	bool bIsGoal = false;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	bool bIsEvacuation = false;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	bool HasSpawnTransform = false;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	FTransform SpawnTransform = FTransform::Identity;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	int32 Handle = -1;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	TArray<int32> Paths; 
};


USTRUCT ( BlueprintType )
struct FAgentSpawnerAgeDistribution
{
	GENERATED_BODY()
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	TMap <TEnumAsByte<ECSAgentAge>, float> AgeProbability = { { Kid, 0.25f }, { Adult, 0.75f }, { Old, 1.f } };
	ECSAgentAge GetAgentAge () const
	{
		const float P = FMath::RandRange( 0.f, 1.0f );
		if ( P <= AgeProbability [ Kid ] )
			return Kid;
		else if ( P <= AgeProbability [ Adult ] )
			return Adult;
		return Old; 
	}
};


USTRUCT ( BlueprintType )
struct FCSAgentSpawnerParameters
{
	
	GENERATED_BODY()
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	FCSNavigationPointParameters NavigationPointParameters; 
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	int32 AgentsGroupId = 0; 
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	float SpawnRate = 2.0f;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	FAgentSpawnerAgeDistribution AgeDistribution;
	UPROPERTY ( BlueprintReadWrite, EditAnywhere, SaveGame)
	FVector2D SpawnerSize = { 200.f, 200.f };
};

UENUM( BlueprintType )
enum class ECSNavigationSystemDrawMode : uint8
{
	Default,
	HeatMap
};

USTRUCT( BlueprintType )
struct FCSNavigationSystemDrawSettings
{
	GENERATED_BODY()
	UPROPERTY( BlueprintReadWrite, EditAnywhere )
	FColor DefaultColor = FColor::Green;

	UPROPERTY( BlueprintReadWrite, EditAnywhere )
	FColor SelectColor = FColor::Yellow;

	UPROPERTY( BlueprintReadWrite, EditAnywhere )
	FColor DestroyColor = FColor::Red;

	UPROPERTY( BlueprintReadWrite, EditAnywhere )
	FColor EvacuationColor = FColor::Purple;

	UPROPERTY( BlueprintReadOnly )
	ECSNavigationSystemDrawMode DrawMode = ECSNavigationSystemDrawMode::Default; 
};


UENUM ( BlueprintType )
enum class ECSNavigationPointSpriteType : uint8
{
	DefaultSprite,
	DestroySprite,
	SelectSprite,
	GoalSprite,
	EvacuationSprite,
	EvacuationGoalSprite
};


USTRUCT ( BlueprintType )
struct FCSNavigationPointSpriteData
{
	GENERATED_BODY()
	UPROPERTY( BlueprintReadWrite, EditAnywhere)
	TMap <ECSNavigationPointSpriteType, UTexture2D*> Sprites;  
};
