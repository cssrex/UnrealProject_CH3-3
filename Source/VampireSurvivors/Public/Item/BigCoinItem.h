#pragma once

#include "CoreMinimal.h"
#include "Item/CoinItem.h"
#include "BigCoinItem.generated.h"

UCLASS()
class VAMPIRESURVIVORS_API ABigCoinItem : public ACoinItem
{
	GENERATED_BODY()
	
public:
	ABigCoinItem();

	virtual void ActivateItem(AActor* Activator) override;
};
