// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSInteractionState.h"
#include "CSInteractionStateAddNavigationPoint.generated.h"


class ACSNavigationPoint;
class ACSNavigationPointPreview;
/**
 * 
 */
UCLASS()
class CROWDSIMULATOR_API UCSInteractionStateAddNavigationPoint : public UCSInteractionState
{
	GENERATED_BODY()
public: 
	virtual void Update(float dt) override;

	virtual void OnInteraction() override;

	virtual void OnDestroy() override;

protected:

	UPROPERTY( BlueprintReadWrite, EditAnywhere )
	TSubclassOf <ACSNavigationPoint> NavigationPointClass;

	UPROPERTY( BlueprintReadWrite, EditAnywhere )
	TSubclassOf <ACSNavigationPointPreview> NavigationPointPreviewClass;

	UPROPERTY()
	ACSNavigationPointPreview * NavigationPointPreview;

	UPROPERTY( EditDefaultsOnly )
	FVector TraceExtent = { 50.f, 50.f, 50.f };

	UPROPERTY()
	TArray<int32> PotentialNeighbours;
};
