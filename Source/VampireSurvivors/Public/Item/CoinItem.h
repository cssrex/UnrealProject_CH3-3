#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "CoinItem.generated.h"

UCLASS()
class VAMPIRESURVIVORS_API ACoinItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ACoinItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue;

	virtual void Tick(float DeltaTime) override;
	virtual void ActivateItem(AActor* Activator) override;
};
