// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/Player/Public/Interactions/Previews/CSAgentSpawnerPreview.h"
#include "Components/BoxComponent.h"

// Sets default values
ACSAgentSpawnerPreview::ACSAgentSpawnerPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent> ( "BoxComponent" ); 
	BoxComponent -> SetHiddenInGame( false );
	BoxComponent -> SetupAttachment( RootComponent ); 
}

UBoxComponent* ACSAgentSpawnerPreview::GetBoxComponent() const
{
	return BoxComponent;
}

// Called when the game starts or when spawned
void ACSAgentSpawnerPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSAgentSpawnerPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}