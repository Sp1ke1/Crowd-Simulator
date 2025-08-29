// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/AI/Public/CSSettings.h"


TSubclassOf<ACSNavigationSystem> UCSSettings::GetNavigationSystemClass() const
{
	return NavigationSystemClass; 
}

TSubclassOf<ACSAgent> UCSSettings::GetAgentClass() const
{
	return AgentClass;
}

TSubclassOf<ACSNavigationPoint> UCSSettings::GetNavigationPointClass() const
{
	return NavigationPointClass;
}

TSubclassOf<ACSAgentSpawner> UCSSettings::GetAgentSpawnerClass() const
{
	return AgentSpawnerClass;
}

float UCSSettings::GetMaxDensityFactor() const
{
	return MaxDensityFactor; 
}

float UCSSettings::GetMaxRandomFactor() const
{
	return MaxRandomFactor; 
}
