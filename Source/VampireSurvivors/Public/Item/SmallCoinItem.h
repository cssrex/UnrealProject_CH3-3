#pragma once

#include "CoreMinimal.h"
#include "Item/CoinItem.h"
#include "SmallCoinItem.generated.h"

UCLASS()
class VAMPIRESURVIVORS_API ASmallCoinItem : public ACoinItem
{
	GENERATED_BODY()
	
public:
	ASmallCoinItem();

	virtual void ActivateItem(AActor* Activator) override;
};
