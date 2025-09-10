#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HWPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CHATX_API AHWPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	AHWPlayerState();
	
	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;

	FString GetPlayerInfoString();
};