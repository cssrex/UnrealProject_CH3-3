#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "MineItem.generated.h"

UCLASS()
class VAMPIRESURVIVORS_API AMineItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AMineItem();

	virtual void ActivateItem(AActor* Activator) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* ExplosionArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;

private:
	void Explode();
	FTimerHandle ExplosionTimerHandle;
	bool bHasExploded;
};
