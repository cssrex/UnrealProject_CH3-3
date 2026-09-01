#include "Character/VSCharacter.h"
#include "Character/VSPlayerController.h"
#include "Core/VSGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

AVSCharacter::AVSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	MaxHealth = 100.0f;
	Health = MaxHealth;
}

float AVSCharacter::GetHealth() const
{
	return Health;
}

void AVSCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);

	UpdateOverHeadHP();
}

void AVSCharacter::AddDebuff(const FDebuffInfo& DebuffInfo)
{
	if (DebuffInfo.Duration <= 0.0f) return;

	FDebuffInfo NewDebuff = DebuffInfo;
	NewDebuff.RemainingTime = NewDebuff.Duration;

	ActiveDebuffs.Add(NewDebuff);
	RefreshDebuffEffects();
}

const TArray<FDebuffInfo>& AVSCharacter::GetActiveDebuffs() const
{
	return ActiveDebuffs;
}

void AVSCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverHeadHP();
}

void AVSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AVSPlayerController* PlayerController = Cast<AVSPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AVSCharacter::Move);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &AVSCharacter::StartJump);
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &AVSCharacter::StopJump);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AVSCharacter::Look);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &AVSCharacter::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &AVSCharacter::StopSprint);
			}
		}
	}
}

void AVSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool bDebuffRemoved = false;

	for (int32 i = ActiveDebuffs.Num() - 1; i >= 0; --i)
	{
		ActiveDebuffs[i].RemainingTime -= DeltaTime;

		if (ActiveDebuffs[i].RemainingTime <= 0.0f)
		{
			ActiveDebuffs.RemoveAt(i);
			bDebuffRemoved = true;
		}
	}

	if (bDebuffRemoved)
	{
		RefreshDebuffEffects();
	}
}

float AVSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverHeadHP();

	if (Health <= 0.0f)
	{
		OnDeath();
	}
	return ActualDamage; // 들어온 DamageAmount 값에다가 방어구, 저항 등을 고려해 계산해서 캐릭터가 실제로 입은 대미지
}

void AVSCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();
	const float InputMultiplier = bReverseControl ? -1.0f : 1.0f;

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		const FVector ForwardDirection = FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f).Vector();
		AddMovementInput(ForwardDirection, MoveInput.X * InputMultiplier);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		const FVector RightDirection = FRotator(0.0f, Controller->GetControlRotation().Yaw + 90.0f, 0.0f).Vector();
		AddMovementInput(RightDirection, MoveInput.Y * InputMultiplier);
	}
}

void AVSCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void AVSCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void AVSCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y * 0.5f);
}

void AVSCharacter::StartSprint(const FInputActionValue& value)
{
	bIsSprinting = true;
	UpdateMovementSpeed();
}

void AVSCharacter::StopSprint(const FInputActionValue& value)
{
	bIsSprinting = false;
	UpdateMovementSpeed();
}

void AVSCharacter::RefreshDebuffEffects()
{
	MoveSpeedMultiplier = 1.0f;
	bReverseControl = false;
	BlindCount = 0;

	for (const FDebuffInfo& Debuff : ActiveDebuffs)
	{
		switch (Debuff.Type)
		{
		case EDebuffType::Slowing:
			MoveSpeedMultiplier *= 0.5f;
			break;
		case EDebuffType::ReverseControl:
			bReverseControl = true;
			break;
		case EDebuffType::Blind:
			BlindCount++;
			break;
		}
	}

	UpdateMovementSpeed();

	if (AVSPlayerController* PlayerController = Cast<AVSPlayerController>(GetController()))
	{
		PlayerController->UpdateBlindEffect(BlindCount);
	}
}

void AVSCharacter::UpdateMovementSpeed()
{
	const float BaseSpeed = bIsSprinting ? SprintSpeed : NormalSpeed;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * MoveSpeedMultiplier;
}

void AVSCharacter::OnDeath()
{
	AVSGameState* VSGameState = GetWorld() ? GetWorld()->GetGameState<AVSGameState>() : nullptr;
	if (VSGameState)
	{
		VSGameState->OnGameOver(false);
	}
}

void AVSCharacter::UpdateOverHeadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("HPText"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}

	if (UProgressBar* HPText = Cast<UProgressBar>(OverheadWidgetInstance->GetWidgetFromName(TEXT("HPProgressBar"))))
	{
		HPText->SetPercent(Health/MaxHealth);
	}
}

