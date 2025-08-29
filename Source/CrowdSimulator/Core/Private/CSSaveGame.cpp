// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/Core/Public/CSSaveGame.h"

void UCSSaveGame::SetLevelData(const FCSSaveGameContainer& LevelData)
{
	SaveGameContainer = LevelData;
}

const FCSSaveGameContainer& UCSSaveGame::GetLevelData() const
{
	return SaveGameContainer; 
}
