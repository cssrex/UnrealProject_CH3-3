
#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Item/DebuffInfo.h"
#include "DebuffItem.generated.h"

UCLASS()
class VAMPIRESURVIVORS_API ADebuffItem : public ABaseItem
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateItem(AActor* Activator) override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debuff")
	FDebuffInfo DebuffInfo;
};
