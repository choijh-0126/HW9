// HW9Pawn.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HW9Pawn.generated.h"

UCLASS()
class HW9_API AHW9Pawn : public APawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

};
