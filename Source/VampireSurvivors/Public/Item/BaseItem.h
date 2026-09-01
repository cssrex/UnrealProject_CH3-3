#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EntityInterface.h"
#include "BaseItem.generated.h"

class USphereComponent;

UCLASS()
class VAMPIRESURVIVORS_API ABaseItem : public AActor, public IEntityInterface
{
	GENERATED_BODY()
	
public:
	ABaseItem();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생한 자기 자신 (Collision 같은거)
		AActor* OtherActor,						// 부딪힌 상대방 액터
		UPrimitiveComponent* OtherComp,			// 부딪힌 액터의 정확히 어떤 컴포넌트에 충돌했는지
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생한 자기 자신 (Collision 같은거)
		AActor* OtherActor,						// 부딪힌 상대방 액터
		UPrimitiveComponent* OtherComp,			// 부딪힌 액터의 정확히 어떤 컴포넌트에 충돌했는지
		int32 OtherBodyIndex) override;
	virtual void ActivateItem(AActor* Activator) override;
	virtual FName GetItemType() const override;

	virtual void DestroyItem();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* PickupParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating")
	bool bEnableFloating = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating")
	float FloatAmplitude = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating", meta = (ClampMin = "0.0"))
	float FloatFrequency = 0.5f;

private:
	FVector InitialMeshLocation = FVector::ZeroVector;
	float FloatElapsedTime = 0.0f;
};
