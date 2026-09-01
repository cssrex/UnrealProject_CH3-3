#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeTrap.generated.h"

class USphereComponent;

UCLASS()
class VAMPIRESURVIVORS_API ASpikeTrap : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike|Movement", meta = (ClampMin = "0.0"))
	float TravelDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike|Movement", meta = (ClampMin = "0.01"))
	float RiseDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike|Movement", meta = (ClampMin = "0.01"))
	float RaisedWaitTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike|Movement", meta = (ClampMin = "0.01"))
	float LowerDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike|Movement", meta = (ClampMin = "0.01"))
	float HiddenWaitTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike|Value")
	int32 SpikeDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ActiveSound;

public:	
	ASpikeTrap();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생한 자기 자신 (Collision 같은거)
		AActor* OtherActor,						// 부딪힌 상대방 액터
		UPrimitiveComponent* OtherComp,			// 부딪힌 액터의 정확히 어떤 컴포넌트에 충돌했는지
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생한 자기 자신 (Collision 같은거)
		AActor* OtherActor,						// 부딪힌 상대방 액터
		UPrimitiveComponent* OtherComp,			// 부딪힌 액터의 정확히 어떤 컴포넌트에 충돌했는지
		int32 OtherBodyIndex);

	UFUNCTION()
	void AttackPlayer();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UStaticMeshComponent* SpikeStaticMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UStaticMeshComponent* FloorStaticMesh;

private:
	FTimerHandle RaiseTimerHandle;
	FTimerHandle HiddenTimerHandle;
	FTimerHandle AttackTimerHandle;

	UPROPERTY()
	AActor* OverlappedActor = nullptr;

	void StartRising();
	void StartLowering();

	FVector RaisedLocation = FVector::ZeroVector;
	FVector HiddenLocation = FVector::ZeroVector;

	float MoveElapsedTime = 0.0f;
	bool bIsRising = false;
};
