// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSAgentPreview.generated.h"

class UCapsuleComponent;

UCLASS()
class CROWDSIMULATOR_API ACSAgentPreview : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACSAgentPreview();

	UCapsuleComponent * GetCapsuleComponent () const; 

	UFUNCTION( BlueprintCallable )
	void SetIsAvailable ( bool Available );

	UFUNCTION( BlueprintCallable )
	bool GetIsAvailable () const; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsAvailable = false; 

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	USkeletalMeshComponent * SkeletalMesh;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	UCapsuleComponent * CapsuleComponent; 

	UPROPERTY( BlueprintReadOnly, EditAnywhere )
	UMaterialInterface * AvailableMaterial;

	UPROPERTY( BlueprintReadOnly, EditAnywhere )
	UMaterialInterface * UnavailableMaterial ; 

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
