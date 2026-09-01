#include "Item/CoinItem.h"
#include "Engine/World.h"
#include "Core/VSGameState.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldRotation(FRotator(0, 180.0f * DeltaTime, 0));
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (AVSGameState* GameState = World->GetGameState<AVSGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}
	}

	DestroyItem();
}
