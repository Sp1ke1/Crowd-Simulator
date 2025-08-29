// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/Player/Public/Interactions/Previews/CSAgentPreview.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "Components/CapsuleComponent.h"
// Sets default values
ACSAgentPreview::ACSAgentPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent> ( "CapsuleComponent" );
	SetRootComponent( CapsuleComponent );
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>( "AgentMesh" );
	SkeletalMesh -> SetupAttachment( CapsuleComponent ); 
	CapsuleComponent -> InitCapsuleSize( 34.0f, 88.0f ); 
}

UCapsuleComponent* ACSAgentPreview::GetCapsuleComponent() const
{
	return CapsuleComponent;
}

// Called when the game starts or when spawned
void ACSAgentPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACSAgentPreview::SetIsAvailable(bool Available)
{
		IsAvailable = Available; 
		UMaterialInterface * Material = Available ? AvailableMaterial : UnavailableMaterial;
		if ( !IsValid ( Material ) )
		{
			UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "Invalid material in agent preview" ) );
			return; 
		}

		const int MaterialNum = SkeletalMesh -> GetNumMaterials();
		for ( int i = 0; i < MaterialNum; i ++ )
		{
			SkeletalMesh -> SetMaterial( i, Material );
		}
}

bool ACSAgentPreview::GetIsAvailable() const
{
	return IsAvailable; 
}

// Called every frame
void ACSAgentPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

