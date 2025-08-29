// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/SpectatorPawn.h"
#include "CSPlayerPawn.generated.h"


class UCSInteractionState;
class UCSPlayerUIComponent;

UCLASS()
class CROWDSIMULATOR_API ACSPlayerPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACSPlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Input" )
	UInputMappingContext * MappingContext; 
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Input" )
	UInputAction * LookAction; 

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Input" )
	UInputAction * InteractAction;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Input" )
	UInputAction * MoveAction;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Input" )
	UInputAction * MenuAction;
	
	UPROPERTY()
	UCSInteractionState * InteractionState = nullptr;

	UPROPERTY( EditDefaultsOnly )
	UCSPlayerUIComponent * UIComponent = nullptr; 

	UFUNCTION()
	void OnInteractionModeChanged ( TSubclassOf <UCSInteractionState> InteractionStateClass  ); 
	
	void Interact ( const FInputActionValue & Value ); 

	void Move ( const FInputActionValue & Value );

	void Look ( const FInputActionValue & Value );

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION( BlueprintCallable ) 
	UCSPlayerUIComponent * GetUIComponent () const; 
};
