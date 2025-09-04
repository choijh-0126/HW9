// HW9Pawn.cpp


#include "HW9Pawn.h"
#include "HW9.h"


void AHW9Pawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = HW9FunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("HW9Pawn::BeginPlay() %s [%s]"), *HW9FunctionLibrary::GetNetModeString(this), *NetRoleString);
	HW9FunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void AHW9Pawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = HW9FunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("HW9Pawn::PossessedBy() %s [%s]"), *HW9FunctionLibrary::GetNetModeString(this), *NetRoleString);
	HW9FunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}
