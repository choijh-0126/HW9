#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/HW9PlayerController.h"
#include "HW9GameModeBase.generated.h"


/**
 *
 */
UCLASS()
class HW9_API AHW9GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(AHW9PlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(AHW9PlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(AHW9PlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<AHW9PlayerController>> AllPlayerControllers;
};
