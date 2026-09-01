#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosionZone.generated.h"

class USphereComponent;

UCLASS()
class VAMPIRESURVIVORS_API AExplosionZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AExplosionZone();

protected:
	virtual void BeginPlay() override;

	void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생한 자기 자신 (Collision 같은거)
		AActor* OtherActor,						// 부딪힌 상대방 액터
		UPrimitiveComponent* OtherComp,			// 부딪힌 액터의 정확히 어떤 컴포넌트에 충돌했는지
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생한 자기 자신 (Collision 같은거)
		AActor* OtherActor,						// 부딪힌 상대방 액터
		UPrimitiveComponent* OtherComp,			// 부딪힌 액터의 정확히 어떤 컴포넌트에 충돌했는지
		int32 OtherBodyIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* ExplosionArea;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UStaticMeshComponent* StaticMesh;

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
	FTimerHandle ExplosionTimerHandle;

	void Explode();

};
