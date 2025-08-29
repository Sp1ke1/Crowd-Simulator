#include "CrowdSimulator/Core/Public/CSCore.h"

DEFINE_LOG_CATEGORY(LogCrowdSimulator);

bool IsValidCSHandle ( int32 Handle )
{
	return Handle != CS_INVALID_HANDLE; 
}