#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Item/DebuffInfo.h"
#include "VSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class VAMPIRESURVIVORS_API AVSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Health") // Get 함수에서는 주로 BlueprintPure을 사용
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Debuff")
	void AddDebuff(const FDebuffInfo& DebuffInfo);

	const TArray<FDebuffInfo>& GetActiveDebuffs() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debuff")
	TArray<FDebuffInfo> ActiveDebuffs;

public:
	AVSCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(
		float DamageAmount,						// 대미지를 얼마나 입었는지
		struct FDamageEvent const& DamageEvent, // 대미지를 받은 유형 이벤트 (스킬 공격 등의 추가 정보를 받을 때)
		AController* EventInstigator,			// 대미지를 누가 입혔는지 (여기서는 nullptr. 지뢰를 심은 사람이 있다면 그 정보가 들어옴)
		AActor* DamageCauser					// 대미지를 누가 어떤 액터가 입혔는지 (여기서는 지뢰 액터가 여기에 들어옴)
	) override;


	void OnDeath();
	void UpdateOverHeadHP();

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

private:
	void RefreshDebuffEffects();
	void UpdateMovementSpeed();

	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;

	float MoveSpeedMultiplier = 1.0f;
	bool bReverseControl = false;
	bool bIsSprinting = false;
	int32 BlindCount = 0;
};
