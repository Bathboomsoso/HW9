//HWGameStateBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HWGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class CHATX_API AHWGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));
	
};
