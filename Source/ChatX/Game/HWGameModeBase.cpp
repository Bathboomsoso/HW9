#include "Game/HWGameModeBase.h"
#include "HWGameStateBase.h"
#include "EngineUtils.h"
#include "Player/HWPlayerController.h"
#include "Player/HWPlayerState.h"

void AHWGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	AHWPlayerController*HWPlayerController = Cast<AHWPlayerController>(NewPlayer);
	if (IsValid(HWPlayerController) == true)
	{
		AllPlayerControllers.Add(HWPlayerController);

		AHWPlayerState*HWPS =HWPlayerController->GetPlayerState<AHWPlayerState>();
		if (IsValid(HWPS) == true)
		{
		HWPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		HWPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		AHWGameStateBase*HWGameStateBase =  GetGameState<AHWGameStateBase>();
		if (IsValid(HWGameStateBase) == true)
		{
		HWGameStateBase->MulticastRPCBroadcastLoginMessage(HWPS->PlayerNameString);
		}
	}
}

FString AHWGameModeBase::GenerateSecretNumber()
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

bool AHWGameModeBase::IsGuessNumberString(const FString& InNumberString)
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

FString AHWGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
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

void AHWGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Warning, TEXT("SecretNumberString = %s"), *SecretNumberString);
}

void AHWGameModeBase::PrintChatMessageString(AHWPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	FString ChatMessageString = InChatMessageString;
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		IncreaseGuessCount(InChattingPlayerController);
		
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		
		for (TActorIterator<AHWPlayerController> It(GetWorld()); It; ++It)
		{
			AHWPlayerController*HWPlayerController = *It;
			if (IsValid(HWPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
			HWPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<AHWPlayerController> It(GetWorld()); It; ++It)
		{
			AHWPlayerController*HWPlayerController = *It;
			if (IsValid(HWPlayerController) == true)
			{
			HWPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void AHWGameModeBase::IncreaseGuessCount(AHWPlayerController* InChattingPlayerController)
{
	{
		AHWPlayerState*HWPS = InChattingPlayerController->GetPlayerState<AHWPlayerState>();
		if (IsValid(HWPS) == true)
		{
		HWPS->CurrentGuessCount++;
		}
	}
}

void AHWGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto&HWPlayerController : AllPlayerControllers)
	{
		AHWPlayerState*HWPS =HWPlayerController->GetPlayerState<AHWPlayerState>();
		if (IsValid(HWPS) == true)
		{
		HWPS->CurrentGuessCount = 0;
		}
	}
}

void AHWGameModeBase::JudgeGame(AHWPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		AHWPlayerState*HWPS = InChattingPlayerController->GetPlayerState<AHWPlayerState>();
		for (const auto&HWPlayerController : AllPlayerControllers)
		{
			if (IsValid(HWPS) == true)
			{
				FString CombinedMessageString =HWPS->PlayerNameString + TEXT(" has won the game.");
			HWPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto&HWPlayerController : AllPlayerControllers)
		{
			AHWPlayerState*HWPS =HWPlayerController->GetPlayerState<AHWPlayerState>();
			if (IsValid(HWPS) == true)
			{
				if (HWPS->CurrentGuessCount <HWPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto&HWPlayerController : AllPlayerControllers)
			{
			HWPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}
