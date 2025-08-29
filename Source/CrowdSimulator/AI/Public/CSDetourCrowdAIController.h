// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CSDetourCrowdAIController.generated.h" 


UCLASS()
class CROWDSIMULATOR_API ACSDetourCrowdAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACSDetourCrowdAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	UBehaviorTree * BTAsset;
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UFUNCTION( BlueprintCallable, BlueprintNativeEvent )
    void StartAgentLogic ();
    
    void StartAgentLogic_Implementation(); 
    
    UFUNCTION( BlueprintCallable )
    void PauseAgentLogic ();
    
    UFUNCTION( BlueprintCallable, BlueprintNativeEvent  )
    void StopAgentLogic ();
    
    void StopAgentLogic_Implementation();

	UFUNCTION( BlueprintCallable )
	bool GetIsRunningAgentLogic () const; 
};
