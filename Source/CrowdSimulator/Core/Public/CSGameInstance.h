// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CSGameInstance.generated.h"


USTRUCT ( BlueprintType )
struct FCSSaveSlotData
{

	GENERATED_BODY()
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FString SlotName = "Playground";
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int32 SlotIndex = 0;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FString LevelName = "CS_Playground"; 
};
/**
 * 
 */
UCLASS()
class CROWDSIMULATOR_API UCSGameInstance : public UGameInstance
{
	GENERATED_BODY()
public: 
	
	UFUNCTION( BlueprintCallable ) 
	void SetLevelSlotData ( const FCSSaveSlotData & SaveSlotData);

	UFUNCTION( BlueprintCallable, BlueprintPure ) 
	const FCSSaveSlotData & GetLevelSlotData () const;
private:
	FCSSaveSlotData LevelSaveSlotData; 
};
