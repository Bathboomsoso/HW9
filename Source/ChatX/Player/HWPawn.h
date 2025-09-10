//HWPawn.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HWPawn.generated.h"

UCLASS()
class CHATX_API AHWPawn : public APawn
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

};
