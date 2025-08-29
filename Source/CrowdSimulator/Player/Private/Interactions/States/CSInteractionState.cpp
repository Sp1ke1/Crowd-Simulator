// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/Player/Public/Interactions/States/CSInteractionState.h"
#include "CrowdSimulator/AI/Public/CSAgent.h"
#include "CrowdSimulator/AI/Public/CSNavigationPoint.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "Kismet/GameplayStatics.h"

void UCSInteractionState::Initialize()
{
}

void UCSInteractionState::OnDestroy()
{
}

void UCSInteractionState::Update(float dt)
{
	const APlayerController * PlayerController = UGameplayStatics::GetPlayerController( this, 0 );
	for ( const auto & TraceChannel : TraceChannels )
	{
		PlayerController -> GetHitResultUnderCursor( TraceChannel, false, HitResultUnderCursor );
		if ( GetIsCSObject( HitResultUnderCursor.GetActor() ) ) 
			break; 
	}
	
}

void UCSInteractionState::OnInteraction()
{
}

const FHitResult& UCSInteractionState::GetLastHitResultUnderCursor() const
{
	return HitResultUnderCursor; 
}

bool UCSInteractionState::GetIsCSObject(AActor* Actor)
{
	return Cast<ACSAgent> ( Actor ) || Cast <ACSNavigationPoint> ( Actor ); 
}

void UCSInteractionState::TryApplyHighlight(AActor* CSObject, bool Highlight)
{
	const auto CSHighlight = Highlight ? ECSNavigationPointSpriteType::DestroySprite : ECSNavigationPointSpriteType::DefaultSprite; 
	ACSAgent * Agent = Cast<ACSAgent> ( CSObject );
	ACSNavigationPoint * NavPoint = Cast<ACSNavigationPoint> ( CSObject );
	if ( Agent )
	{
		Agent -> SetHighlight( CSHighlight );
		return; 
	}
	if ( NavPoint )
	{
		NavPoint -> SetSprite( CSHighlight );
		return; 
	}
}