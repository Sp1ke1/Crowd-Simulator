// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSNavigationPathPreview.generated.h"

UCLASS()
class CROWDSIMULATOR_API ACSNavigationPathPreview : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACSNavigationPathPreview();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(); 
	ULineBatchComponent * LineBatchComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION( BlueprintCallable )
	void SetPathPoints ( const FVector & P1, const FVector & P2, const FColor & Color );

	UFUNCTION( BlueprintCallable )
	void Clear();

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int Thickness = 5;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float Lifetime = 0.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	uint8 DepthPriority = 0; 
	
	
	UFUNCTION( BlueprintCallable )
	ULineBatchComponent * GetLineBatchComponent() const; 
};
