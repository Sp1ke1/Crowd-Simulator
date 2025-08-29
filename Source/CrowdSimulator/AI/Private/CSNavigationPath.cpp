// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/AI/Public/CSNavigationPath.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"


void UCSNavigationPath::SetNavigationPoints(const TArray<int32>& Path)
{
	NavigationPoints = Path; 
}

const TArray<int32>& UCSNavigationPath::GetNavigationPoints() const
{
	return NavigationPoints; 
}

void UCSNavigationPath::SetNavigationPointIndex(int32 NewIndex)
{
	if ( NewIndex < 0 || NewIndex >= GetNavigationPoints().Num())
		return;
	Index = NewIndex; 
}

bool UCSNavigationPath::GetIsValid() const
{
	const UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	if ( !IsValid ( CSSubsystem ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT("UCSNavigationPath::GetIsValidPath() => CSSubsystem is invalid.") );
		return false;
	}
	for (const auto Point : GetNavigationPoints() )
	{
		if ( !CSSubsystem -> GetIsNavigationPointRegistered( Point ) )
			return false; 
	}
	if ( !CSSubsystem -> GetIsNavigationPointRegistered( GetGoalHandle() ) )
		return false;
	return true;
}

int32 UCSNavigationPath::GetCurrentIndex() const
{
	return Index; 
}

void UCSNavigationPath::NextNavigationPoint()
{
	SetNavigationPointIndex( GetCurrentIndex() + 1 ); 
}

int32 UCSNavigationPath::GetCurrentNavigationPoint() const
{
	return GetNavigationPoints()[GetCurrentIndex()]; 
}

int32 UCSNavigationPath::GetPreviousNavigationPoint() const
{
	if ( !GetNavigationPoints().IsValidIndex( GetCurrentIndex() - 1 ) )
		return CS_INVALID_HANDLE;
	return GetNavigationPoints()[ GetCurrentIndex() - 1 ]; 
}

int32 UCSNavigationPath::GetGoalHandle() const
{
	return GoalHandle; 
}

void UCSNavigationPath::SetGoalHandle(int32 Goal)
{
	GoalHandle = Goal; 
}
