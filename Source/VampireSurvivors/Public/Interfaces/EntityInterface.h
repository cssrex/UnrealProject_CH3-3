#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EntityInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEntityInterface : public UInterface
{
	GENERATED_BODY()
};

class VAMPIRESURVIVORS_API IEntityInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp, // 오버랩이 발생한 자기 자신 (Collision 같은거)
		AActor* OtherActor, // 부딪힌 상대방 액터
		UPrimitiveComponent* OtherComp, // 부딪힌 액터의 정확히 어떤 컴포넌트에 충돌했는지
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) = 0;
	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp, // 오버랩이 발생한 자기 자신 (Collision 같은거)
		AActor* OtherActor, // 부딪힌 상대방 액터
		UPrimitiveComponent* OtherComp, // 부딪힌 액터의 정확히 어떤 컴포넌트에 충돌했는지
		int32 OtherBodyIndex) = 0;
	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0;
};
