#pragma once

#include "CoreMinimal.h"
#include "DebuffInfo.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EDebuffType : uint8
{
    Slowing,
    ReverseControl,
    Blind
};

USTRUCT(BlueprintType)
struct FDebuffInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EDebuffType Type = EDebuffType::Slowing;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.1"))
    float Duration = 5.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float RemainingTime = 0.0f;
};
