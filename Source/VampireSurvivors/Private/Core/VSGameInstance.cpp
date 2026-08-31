#include "Core/VSGameInstance.h"

UVSGameInstance::UVSGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void UVSGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
}

int32 UVSGameInstance::GetCurrentLevelIndex() const
{
	return CurrentLevelIndex;
}

void UVSGameInstance::SetCurrentLevelIndex(int32 NewLevelIndex)
{
	CurrentLevelIndex = NewLevelIndex;
}

int32 UVSGameInstance::GetTotalScore() const
{
	return TotalScore;
}

void UVSGameInstance::SetTotalScore(int32 NewTotalScore)
{
	TotalScore = NewTotalScore;
}
