// Copyright Ziger


#include "AbilitySystem/Data/LootTiers.h"

TArray<FLootItem> ULootTiers::GetLootItems()
{
	TArray<FLootItem> ReturnItems;
	for (FLootItem& Item : LootItems)
	{
		for (int32 i = 0; i < Item.MaxSpawnNums; ++i)
		{
			if (FMath::FRandRange(0.f,100.f) < Item.SpawnChance)
			{
				FLootItem NewItem;
				NewItem.ItemClass = Item.ItemClass;
				NewItem.bLevelOverride = Item.bLevelOverride;
				ReturnItems.Add(NewItem);
			}
		}
	}

	return ReturnItems;
}
