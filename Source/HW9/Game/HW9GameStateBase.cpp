// HW9GameStateBase.cpp


#include "HW9GameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/HW9PlayerController.h"

void AHW9GameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			AHW9PlayerController* HW9PC = Cast<AHW9PlayerController>(PC);
			if (IsValid(HW9PC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				HW9PC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
