// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "CSNavigationPath.generated.h"


/**
 * 
 */


UCLASS ( Blueprintable )
class CROWDSIMULATOR_API UCSNavigationPath : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION( BlueprintCallable )
	void SetNavigationPoints ( const TArray<int32> & Path ); 
	
	UFUNCTION( BlueprintCallable, BlueprintPure  )
	const TArray<int32> & GetNavigationPoints () const;

	UFUNCTION( BlueprintCallable ) 
	void SetNavigationPointIndex ( int32 NewIndex ); 

	UFUNCTION( BlueprintCallable, BLueprintPure )
	bool GetIsValid () const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	int32 GetCurrentIndex () const; 
	
	UFUNCTION( BlueprintCallable ) 
	void NextNavigationPoint();

	UFUNCTION( BlueprintCallable, BlueprintPure ) 
	int32 GetCurrentNavigationPoint() const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	int32 GetPreviousNavigationPoint () const; 
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	int32 GetGoalHandle () const;

	UFUNCTION( BlueprintCallable )
	void SetGoalHandle ( int32 Goal ); 
private:
	TArray<int32> NavigationPoints;
	
	int32 GoalHandle = CS_INVALID_HANDLE;
	
	int32 Index = 0; 
};