//HWPlayerController.cpp


#include "Player/HWPlayerController.h"

#include "ChatX/ChatX.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/HWChatInput.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/HWGameModeBase.h"
#include "HWPlayerState.h"
#include "Net/UnrealNetwork.h"

AHWPlayerController::AHWPlayerController()
{
	bReplicates = true;
}

void AHWPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsLocalController() == false)
	{
		return;
	}
	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UHWChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void AHWPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		AHWPlayerState*HWPS = GetPlayerState<AHWPlayerState>();
		if (IsValid(HWPS) == true)
		{
			FString CombinedMessageString =HWPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
			
			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void AHWPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	ChatXFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void AHWPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void AHWPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		AHWGameModeBase*HWGM = Cast<AHWGameModeBase>(GM);
		if (IsValid(HWGM) == true)
		{
		HWGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

void AHWPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}
