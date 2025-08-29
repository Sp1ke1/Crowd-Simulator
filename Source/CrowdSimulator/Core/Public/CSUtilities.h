// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "CSUtilities.generated.h"
class ACSPlayerPawn;
/**
 * 
 */
UCLASS()
class CROWDSIMULATOR_API UCSUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public: 
	static void PrintString ( const FString & String, float DisplayTime = 3.0f, const FColor & Color = FColor::Yellow );

	UFUNCTION( BlueprintCallable ) 
	static ACSPlayerPawn * GetPlayerPawn ( UObject * WorldContextObject ) ;
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	static const FCSAgentSpeedParameters & GetAgentAgeSpeedParameters ( TEnumAsByte<ECSAgentAge> AgentAge ); 

	UFUNCTION( BlueprintCallable, BlueprintPure )
	static const FString & AgentAgeToString ( TEnumAsByte<ECSAgentAge> AgentAge );

	UFUNCTION( BlueprintCallable, BlueprintPure )
	static bool IsInvalidCSHandle ( int32 Handle ); 
	
	static const TMap <ECSAgentAge, FCSAgentSpeedParameters> AgentSpeedMap;

	static const TMap <ECSAgentAge, FString> AgeToStringMap; 
	
};

