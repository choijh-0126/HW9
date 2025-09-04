// HW9GameModeBase.cpp


#include "Game/HW9GameModeBase.h"

#include "HW9GameStateBase.h"

#include "Player/HW9PlayerController.h"
#include "Player/HW9PlayerState.h"
#include "EngineUtils.h"

void AHW9GameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	AHW9PlayerController* HW9PlayerController = Cast<AHW9PlayerController>(NewPlayer);
	if (IsValid(HW9PlayerController) == true)
	{
		HW9PlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(HW9PlayerController);

		AHW9PlayerState* HW9PS = HW9PlayerController->GetPlayerState<AHW9PlayerState>();
		if (IsValid(HW9PS) == true)
		{
			HW9PS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		AHW9GameStateBase* HW9GameStateBase = GetGameState<AHW9GameStateBase>();
		if (IsValid(HW9GameStateBase) == true)
		{
			HW9GameStateBase->MulticastRPCBroadcastLoginMessage(HW9PS->PlayerNameString);
		}
	}
}

FString AHW9GameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool AHW9GameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString AHW9GameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void AHW9GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
}

void AHW9GameModeBase::PrintChatMessageString(AHW9PlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);

		for (TActorIterator<AHW9PlayerController> It(GetWorld()); It; ++It)
		{
			AHW9PlayerController* HW9PlayerController = *It;
			if (IsValid(HW9PlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				HW9PlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<AHW9PlayerController> It(GetWorld()); It; ++It)
		{
			AHW9PlayerController* HW9PlayerController = *It;
			if (IsValid(HW9PlayerController) == true)
			{
				HW9PlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void AHW9GameModeBase::IncreaseGuessCount(AHW9PlayerController* InChattingPlayerController)
{
	AHW9PlayerState* HW9PS = InChattingPlayerController->GetPlayerState<AHW9PlayerState>();
	if (IsValid(HW9PS) == true)
	{
		HW9PS->CurrentGuessCount++;
	}
}

void AHW9GameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& HW9PlayerController : AllPlayerControllers)
	{
		AHW9PlayerState* HW9PS = HW9PlayerController->GetPlayerState<AHW9PlayerState>();
		if (IsValid(HW9PS) == true)
		{
			HW9PS->CurrentGuessCount = 0;
		}
	}
}

void AHW9GameModeBase::JudgeGame(AHW9PlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		AHW9PlayerState* HW9PS = InChattingPlayerController->GetPlayerState<AHW9PlayerState>();
		for (const auto& HW9PlayerController : AllPlayerControllers)
		{
			if (IsValid(HW9PS) == true)
			{
				FString CombinedMessageString = HW9PS->PlayerNameString + TEXT(" has won the game.");
				HW9PlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& HW9PlayerController : AllPlayerControllers)
		{
			AHW9PlayerState* HW9PS = HW9PlayerController->GetPlayerState<AHW9PlayerState>();
			if (IsValid(HW9PS) == true)
			{
				if (HW9PS->CurrentGuessCount < HW9PS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& HW9PlayerController : AllPlayerControllers)
			{
				HW9PlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}

