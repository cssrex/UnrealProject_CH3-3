#include "Item/DebuffItem.h"
#include "Character/VSCharacter.h"

void ADebuffItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AVSCharacter* PlayerCharacter = Cast<AVSCharacter>(Activator))
		{
			PlayerCharacter->AddDebuff(DebuffInfo);
		}
	}

	DestroyItem();
}
