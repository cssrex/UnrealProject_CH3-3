#include "Item/HealingItem.h"
#include "Character/VSCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AVSCharacter* PlayerCharacter = Cast<AVSCharacter>(Activator))
		{
			PlayerCharacter->AddHealth(HealAmount);
		}

		DestroyItem();
	}
}
