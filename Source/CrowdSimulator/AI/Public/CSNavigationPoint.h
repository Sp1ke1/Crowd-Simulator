
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSTypes.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "CSNavigationPoint.generated.h"


class UBoxComponent;
class UCSSubsystem;
class ACSNavigationSystem; 


UCLASS()
class CROWDSIMULATOR_API ACSNavigationPoint : public AActor
{
	GENERATED_BODY()

	friend UCSSubsystem;
	friend ACSNavigationSystem; 
	
public:
	// Sets default values for this actor's properties
	ACSNavigationPoint();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	int GetGoalGroupId() const;
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	int32 GetHandle () const;

	UFUNCTION( BlueprintCallable )
	void SetDefaultSprite (); 

	UFUNCTION( BlueprintCallable, BlueprintNativeEvent ) 
	void SetSprite ( ECSNavigationPointSpriteType SpriteType  );

	void SetSprite_Implementation ( ECSNavigationPointSpriteType SpriteType );

	UFUNCTION( BlueprintCallable )
	bool GetIsEvacuationPoint () const; 

	UFUNCTION( BlueprintCallable )
	void SetIsEvacuationPoint ( bool IsEvacuationPoint ); 

	UFUNCTION( BlueprintCallable )
	void SetupPointFromParameters ( const FCSNavigationPointParameters & Parameters ); 
	
	UFUNCTION( BlueprintCallable )
	bool GetIsGoalPoint () const; 

	UFUNCTION( BlueprintCallable )
	void SetIsGoalPoint ( bool bIsGoal ); 
	
	UFUNCTION( BlueprintCallable )
	UBoxComponent * GetCollisionComponent () const; 

	UFUNCTION( BlueprintCallable ) 
	void SetGoalGroupId(int Id);

	UFUNCTION( BlueprintCallable )
	const FCSNavigationPointSpriteData & GetSpritesData () const;

	UFUNCTION( BlueprintCallable )
	FCSNavigationPointParameters GetNavigationPointParameters() const;

protected:
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetHandle ( int32 Handle );

	UPROPERTY( EditDefaultsOnly )
	UBoxComponent * BoxCollision; 
	
	UPROPERTY( EditAnywhere )
	bool IsGoal = false; 

	UPROPERTY( EditAnywhere )
	int32 GoalGroupId; 

	UPROPERTY( EditAnywhere )
	bool bIsEvacuationPoint = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FCSNavigationPointSpriteData SpritesData; 
	
	int32 NavPointHandle = CS_INVALID_HANDLE;
};
