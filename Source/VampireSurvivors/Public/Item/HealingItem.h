#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "HealingItem.generated.h"

UCLASS()
class VAMPIRESURVIVORS_API AHealingItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AHealingItem();

	virtual void ActivateItem(AActor* Activator) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 HealAmount;
};
