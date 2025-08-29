#pragma once

#define ECC_CS_NavigationPoint ECC_GameTraceChannel1
#define ECC_CS_AgentSpawner ECC_GameTraceChannel2
#define ECC_CS_Agent ECC_GameTraceChannel3 
	
DECLARE_LOG_CATEGORY_EXTERN( LogCrowdSimulator , Log, All);

#define CS_INVALID_HANDLE -1

bool IsValidCSHandle ( int32 Handle );
