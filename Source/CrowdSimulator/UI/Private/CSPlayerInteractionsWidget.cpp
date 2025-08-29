// Fill out your copyright notice in the Description page of Project Settings.

#include "CrowdSimulator/UI/Public/CSPlayerInteractionsWidget.h"

FCSAgentParameters UCSPlayerInteractionsWidget::GetAgentParameters_Implementation() const
{
	return {};
}

FCSNavigationPointParameters UCSPlayerInteractionsWidget::GetNavigationPointParameters_Implementation() const
{
	return {};
}

FCSAgentSpawnerParameters UCSPlayerInteractionsWidget::GetAgentSpawnerParameters_Implementation() const
{
	return {};
}

float UCSPlayerInteractionsWidget::GetNavigationPointsSnappingRadius_Implementation() const
{
	return 0.f; 
}

float UCSPlayerInteractionsWidget::GetAgentSpawnerSpanningRadius_Implementation()
{
	return 0.f; 
}
