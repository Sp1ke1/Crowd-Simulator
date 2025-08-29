// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/Core/Public/CSGameInstance.h"

void UCSGameInstance::SetLevelSlotData(const FCSSaveSlotData& SaveSlotData)
{
	LevelSaveSlotData = SaveSlotData; 
}

const FCSSaveSlotData& UCSGameInstance::GetLevelSlotData() const
{
	return LevelSaveSlotData; 
}
