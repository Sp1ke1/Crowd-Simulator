// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSNavigationPointPreview.h"
#include "CSAgentSpawnerPreview.generated.h"

UCLASS()
class CROWDSIMULATOR_API ACSAgentSpawnerPreview : public ACSNavigationPointPreview
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACSAgentSpawnerPreview();

	UBoxComponent * GetBoxComponent() const; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsAvailable = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	UBoxComponent * BoxComponent;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
