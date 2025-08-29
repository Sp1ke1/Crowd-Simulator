// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/Core/Public/CSUtilities.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "CrowdSimulator/Player/Public/CSPlayerPawn.h"
#include "Kismet/GameplayStatics.h"

const TMap <ECSAgentAge, FCSAgentSpeedParameters> UCSUtilities::AgentSpeedMap =
{
	{ Kid, { 100.f, 150.f } },
	{ Adult, { 150.f, 250.f } },
	{ Old, { 80.f, 130.f } }
}; 

const TMap <ECSAgentAge, FString> UCSUtilities::AgeToStringMap =
{
  	{ Kid, "Kid"},
	{ Adult, "Adult" },
	{ Old, "Old" }
};

void UCSUtilities::PrintString(const FString& String, float DisplayTime, const FColor& Color)
{
	if ( GEngine )
	{
		GEngine -> AddOnScreenDebugMessage( -1, DisplayTime, Color, String ); 
	}
}

ACSPlayerPawn* UCSUtilities::GetPlayerPawn( UObject * WorldContextObject )
{
	return Cast <ACSPlayerPawn> ( UGameplayStatics::GetPlayerPawn( WorldContextObject, 0 ) );  
}

const FCSAgentSpeedParameters & UCSUtilities::GetAgentAgeSpeedParameters( TEnumAsByte<ECSAgentAge> AgentAge )
{
	return AgentSpeedMap [ AgentAge ]; 
}

const FString& UCSUtilities::AgentAgeToString(TEnumAsByte<ECSAgentAge> AgentAge)
{
	return AgeToStringMap [ AgentAge ]; 
}

bool UCSUtilities::IsInvalidCSHandle(int32 Handle)
{
	return Handle == CS_INVALID_HANDLE; 
}
