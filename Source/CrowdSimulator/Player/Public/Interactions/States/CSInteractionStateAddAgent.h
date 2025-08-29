#pragma once

#include "CSInteractionState.h"
#include "CSInteractionStateAddAgent.generated.h"

class ACSAgent;
class ACSAgentPreview; 

UCLASS( BlueprintType, Blueprintable )
class CROWDSIMULATOR_API UCSInteractionStateAddAgent : public UCSInteractionState
{
public:

	GENERATED_BODY()

	virtual void Initialize() override;

	virtual void Update ( float dt ) override;

	virtual void OnInteraction() override;

	virtual void OnDestroy() override;

protected:
	

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSubclassOf <ACSAgent> AgentClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSubclassOf<ACSAgentPreview> AgentPreviewClass;
	UPROPERTY()
	ACSAgentPreview * AgentPreview;

};
