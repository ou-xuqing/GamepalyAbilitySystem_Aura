// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AuraEnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAuraEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 敌人高光接口
 */
class AURA_API IAuraEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void HighlightActor()=0;
	virtual void UnHighlightActor()=0;
};
