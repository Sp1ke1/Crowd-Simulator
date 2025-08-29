// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "GameFramework/Character.h"
#include "CSAgent.generated.h"


class UCSSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FOnStartedFollowingPath, int32, NavPointHandle1, int32, NavPointHandle2 );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FOnPathSectorFinished, int32, NavPointHandle1, int32, NavPointHandle2 );


UCLASS()
class CROWDSIMULATOR_API ACSAgent : public ACharacter
{
	GENERATED_BODY()
	friend UCSSubsystem;
public:

	// Sets default values for this character's properties
	ACSAgent();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION( BlueprintCallable )
	void SetHighlight ( ECSNavigationPointSpriteType HighlightType );

	UFUNCTION( BlueprintCallable )
	int GetGroupId () const;

	UFUNCTION( BlueprintCallable )
	bool GetIsRunningLogic () const; 

	UFUNCTION( BlueprintCallable )
	void StartLogic ();

	UFUNCTION( BlueprintCallable )
	void StopLogic ();

	UFUNCTION( BlueprintCallable )
	void PauseLogic ();
	
	UFUNCTION( BlueprintCallable )
	int32 GetHandle () const;

	UFUNCTION( BlueprintCallable )
	void SetSpeed ( float Speed );

	UFUNCTION( BlueprintCallable, BlueprintPure )
	float GetSpeed () const;

	UFUNCTION( BlueprintCallable )
	const FCSAgentSpeedParameters & GetSpeedParameters () const;
	
	UFUNCTION( BlueprintCallable )
	void SetSpeedParameters( const FCSAgentSpeedParameters & AgentSpeedParameters);

	UFUNCTION( BlueprintCallable )
	void SetAge(TEnumAsByte<ECSAgentAge> NewAge);

	UFUNCTION( BlueprintCallable, BlueprintPure )
	TEnumAsByte<ECSAgentAge> GetAge () const; 

	UFUNCTION( BlueprintCallable )
	void SetupFromParameters ( const FCSAgentParameters & AgentParameters );


	UFUNCTION( BlueprintCallable, BlueprintPure )
	FCSAgentParameters GetAgentParameters () const; 
	
	UFUNCTION( BlueprintCallable, BlueprintPure  )
	ECSAgentMovementType GetMovementType () const;

	void UpdateMovementSpeed();

	UFUNCTION( BlueprintCallable )
	void SetMovementType ( ECSAgentMovementType NewMovementType ); 
	
	UPROPERTY( BlueprintAssignable, BlueprintCallable )
	FOnStartedFollowingPath OnStartedFollowingPath; 
	
	UPROPERTY( BlueprintAssignable, BlueprintCallable )
	FOnPathSectorFinished OnFinishedFollowingPath; 

	
protected:

	UFUNCTION( BlueprintCallable )
	void SetGroupId ( int Id ); 

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; 

	void SetHandle ( int32 NewHandle ); 

	UPROPERTY( BlueprintReadWrite, EditAnywhere, Category = "Agent Settings" )
	UMaterialInterface * HighlightMaterial; 

	// Array to store default materials of the skeletal mesh. Used to restore state after SetHighlight ( false ) 
	UPROPERTY()
	TArray <UMaterialInterface*> DefaultMaterials;
	
	int32 GroupId;

	TEnumAsByte<ECSAgentAge> Age; 

	int32 Handle = CS_INVALID_HANDLE;

	FCSAgentSpeedParameters SpeedParameters;

	ECSAgentMovementType MovementType = ECSAgentMovementType::Walk;

	
};
