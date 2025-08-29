// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "CSNavigationPointPreview.generated.h"

class UBoxComponent;
class UBillboardComponent; 

UCLASS()
class CROWDSIMULATOR_API ACSNavigationPointPreview : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACSNavigationPointPreview();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	UTexture2D * SelectTexture = nullptr;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite)
	UTexture2D * DefaultTexture = nullptr;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite)
	UTexture2D * GoalTexture = nullptr;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite)
	UTexture2D * DestroyTexture = nullptr;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	UBillboardComponent * SpriteTexture;

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION( BlueprintCallable )
	void SetIsAvailable ( bool Available );
	
	void SetHighlight(ECSNavigationPointSpriteType HighlightType) const;

	UFUNCTION( BlueprintCallable )
	bool GetIsAvailable () const;

	UFUNCTION(BlueprintCallable)
	bool GetIsGoal() const
	{
		return IsGoal; 
	}

	UFUNCTION(BlueprintCallable)
	void SetIsGoal( bool isGoal )
	{
		IsGoal = isGoal;
	}
	
	bool IsAvailable = true;

	bool IsGoal = false; 

};
