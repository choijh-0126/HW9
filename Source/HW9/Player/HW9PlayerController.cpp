#include "Player/HW9PlayerController.h"

#include "UI/HW9ChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HW9.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "HW9PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Game/HW9GameModeBase.h"

AHW9PlayerController::AHW9PlayerController()
{
	bReplicates = true;
}

void AHW9PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void AHW9PlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		AHW9PlayerState* HW9PS = GetPlayerState<AHW9PlayerState>();
		if (IsValid(HW9PS) == true)
		{
			FString CombinedMessageString = HW9PS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void AHW9PlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	HW9FunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void AHW9PlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void AHW9PlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		AHW9GameModeBase* HW9GM = Cast<AHW9GameModeBase>(GM);
		if (IsValid(HW9GM) == true)
		{
			HW9GM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

void AHW9PlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}