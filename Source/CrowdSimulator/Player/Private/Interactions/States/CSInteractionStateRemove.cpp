// Fill out your copyright notice in the Description page of Project Settings.

#include "CrowdSimulator/Player/Public/Interactions/States/CSInteractionStateRemove.h"
#include "CrowdSimulator/AI/Public/CSAgent.h"

void UCSInteractionStateRemove::Update(float dt)
{
	Super::Update(dt);
	AActor * NewSelectedActor =  Cast <AActor> ( GetLastHitResultUnderCursor() . GetActor() );
	
	// If new selected agent is different from old on => stop highlighting old one 
	if ( IsValid ( SelectedCSObject ) && NewSelectedActor != SelectedCSObject )
	{
		TryApplyHighlight( SelectedCSObject, false ); 
	}

	// Highlight new selected agent 
	SelectedCSObject = NewSelectedActor; ;
	if ( SelectedCSObject )
	{
		TryApplyHighlight( SelectedCSObject, true );  
	}
}

void UCSInteractionStateRemove::OnInteraction()
{
	Super::OnInteraction();
	if ( IsValid ( SelectedCSObject ) )
	{
		SelectedCSObject -> Destroy();
		SelectedCSObject = nullptr; 
	}
}


