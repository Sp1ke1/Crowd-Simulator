// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSInteractionState.h"
#include "CSInteractionStateRemove.generated.h"

class ACSNavigationPoint;
/**
 * 
 */
UCLASS()
class CROWDSIMULATOR_API UCSInteractionStateRemove : public UCSInteractionState
{
	GENERATED_BODY()
public: 
	virtual void Update(float dt) override;

	virtual void OnInteraction() override;

protected:
	

	UPROPERTY()
	AActor * SelectedCSObject;
};
