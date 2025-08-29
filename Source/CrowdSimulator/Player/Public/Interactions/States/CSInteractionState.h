// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "UObject/Object.h"
#include "CSInteractionState.generated.h"

/**
 * 
 */
UCLASS( BlueprintType, Blueprintable )
class CROWDSIMULATOR_API UCSInteractionState : public UObject
{
	GENERATED_BODY()
public: 
	virtual void Initialize ();

	virtual void OnDestroy (); 

	virtual void Update ( float dt );

	virtual void OnInteraction ();

	static bool GetIsCSObject ( AActor * Actor );

	static void TryApplyHighlight ( AActor * CSObject, bool Highlight ); 
	

	UFUNCTION( BlueprintCallable )
	const FHitResult & GetLastHitResultUnderCursor () const;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TArray<TEnumAsByte<ECollisionChannel>> TraceChannels = { ECC_CS_Agent, ECC_CS_AgentSpawner, ECC_CS_NavigationPoint }; 
	
	FHitResult HitResultUnderCursor; 
};
