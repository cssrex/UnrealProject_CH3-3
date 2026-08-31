#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VSGameInstance.generated.h"

UCLASS()
class VAMPIRESURVIVORS_API UVSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UVSGameInstance();

public:
	UFUNCTION(BlueprintCallable, Category = "GameData")
	void AddToScore(int32 Amount);
	int32 GetCurrentLevelIndex() const;
	void SetCurrentLevelIndex(int32 NewLevelIndex);
	int32 GetTotalScore() const;
	void SetTotalScore(int32 NewTotalScore);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 TotalScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentLevelIndex;
};
