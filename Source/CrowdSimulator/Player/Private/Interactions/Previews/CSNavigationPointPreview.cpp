// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/Player/Public/Interactions/Previews/CSNavigationPointPreview.h"
#include "Components/BillboardComponent.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
// Sets default values
ACSNavigationPointPreview::ACSNavigationPointPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpriteTexture = CreateDefaultSubobject<UBillboardComponent> ("SpriteTexture");
	RootComponent = CreateDefaultSubobject<USceneComponent> ("RootComponent");
	SetRootComponent( RootComponent );
	SpriteTexture -> SetupAttachment( RootComponent ); 
}

// Called when the game starts or when spawned
void ACSNavigationPointPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSNavigationPointPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSNavigationPointPreview::SetIsAvailable(bool Available)
{
	IsAvailable = Available;
}

void ACSNavigationPointPreview::SetHighlight ( ECSNavigationPointSpriteType HighlightType ) const
{
	switch ( HighlightType )
	{
		case ECSNavigationPointSpriteType::DefaultSprite:
		{
			if ( !IsValid ( DefaultTexture ) )
			{
				UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "NavigationPoint => Invalid default texture" ) ); 
			}
			if ( GetIsGoal()  )
			{
				SpriteTexture -> SetSprite( GoalTexture ); 
			}
			else
			{
				SpriteTexture -> SetSprite( DefaultTexture ); 
			}
			return;
		}
		case ECSNavigationPointSpriteType::DestroySprite:
		{
			if ( !IsValid ( DestroyTexture ) )
			{
				UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "NavigationPoint => Invalid destroy texture" ) ); 
			}
			SpriteTexture -> SetSprite( DestroyTexture );
			return;
		}
		case ECSNavigationPointSpriteType::SelectSprite:
		{
			if ( !IsValid ( SelectTexture ) )
			{
				UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "NavigationPoint => Invalid select texture" ) ); 
			}
			SpriteTexture -> SetSprite( SelectTexture );
			return; 
		}
		
	default:
		{
			
		};
	}
}

bool ACSNavigationPointPreview::GetIsAvailable() const
{
	return IsAvailable; 
}

