// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlot.h"
#include "GameFramework/PlayerState.h"

UQuickSlot::UQuickSlot()
{
	MaxVoxelItemNum = 64;
	InventoryVoxelItemArray.Init(FVoxelItemInfo(), 27);
	QuickSlotVoxelItemArray.Init(FVoxelItemInfo(), 9);
}

/*
 * Looting Voxel
 * @ret false : no destroy / true : destory
 */
bool UQuickSlot::LootingVoxel(const FVoxelItemInfo& LootingVoxelItem)
{
	int QuickSlotIndex = GetQuickSlotEmptyIndex(LootingVoxelItem);

	if (QuickSlotIndex != 128)
	{
		auto& cQuickSlotVoxelItem = QuickSlotVoxelItemArray[QuickSlotIndex];
		cQuickSlotVoxelItem.VoxelType = LootingVoxelItem.VoxelType;
		cQuickSlotVoxelItem.Num += LootingVoxelItem.Num;

		return true;
	}

	return false;
}

bool UQuickSlot::isValidCurrentVoxelItem() const
{
	auto& cVoxelItem = QuickSlotVoxelItemArray[CurrentVoxelItemIndex];

	if ((cVoxelItem.VoxelType == EVoxelType::Empty) || (cVoxelItem.VoxelType == EVoxelType::Count))
	{
		return false;
	}

	return true;
}

/*
 * return 128  : quickslot is not empty
 * return < 128 : quickslot empty index
 */
uint8 UQuickSlot::GetQuickSlotEmptyIndex(const FVoxelItemInfo& LootingVoxel)
{
	int EmptyIndex = 128;

	for (int i = 0; i < QuickSlotVoxelItemArray.Num(); ++i)
	{
		// Current QuickSlotVoxel
		auto& cQuickSlotVoxel = QuickSlotVoxelItemArray[i];
		uint8 SumVoxelNum = cQuickSlotVoxel.Num + LootingVoxel.Num;

		// Same Voxel Item && Sum of Quickslot and Looting Voxel Num is less than maximum.
		if ((cQuickSlotVoxel.VoxelType == LootingVoxel.VoxelType)
			&& (SumVoxelNum >= 0)
			&& (SumVoxelNum <= MaxVoxelItemNum))
		{
			return i;
		}

		// Find Empty Index
		if ((EmptyIndex == 128)
			&& (cQuickSlotVoxel.VoxelType == EVoxelType::Empty)
			&& (cQuickSlotVoxel.Num == 0))
		{
			EmptyIndex = i;
		}
	}

	return EmptyIndex;
}

void UQuickSlot::SetCurrentVoxelItem(const FVoxelItemInfo& VoxelInfo)
{
	if (VoxelInfo.Num < 0)
	{
		return;
	}

	QuickSlotVoxelItemArray[CurrentVoxelItemIndex] = VoxelInfo;
}

void UQuickSlot::SetCurrentVoxelItemByIndex(int32 index)
{
	if ((0 <= index) && (index <= 9))
	{
		CurrentVoxelItemIndex = index;
	}
}

void UQuickSlot::SetQuickSlotVoxelItemArray(EVoxelType inVoxelType, int32 num, int32 index)
{
	if ((0 <= index) && (index <= 9) && (num >= 0))
	{
		QuickSlotVoxelItemArray[index].VoxelType = inVoxelType;
		QuickSlotVoxelItemArray[index].Num = num;
	}
}

void UQuickSlot::SetItemArray(EInventoryType InvenType, int32 index, FVoxelItemInfo ItemInfo)
{
	if (InvenType == EInventoryType::QuickSlot)
	{
		QuickSlotVoxelItemArray[index] = ItemInfo;
	}
	else if (InvenType == EInventoryType::Inventory)
	{
		InventoryVoxelItemArray[index] = ItemInfo;
	}
	else
	{
		LOG("NOT ARRAY");
	}

	return;
}

TArray<FVoxelItemInfo> UQuickSlot::GetItemArray(EInventoryType InvenType)
{
	if (InvenType == EInventoryType::QuickSlot)
	{
		return QuickSlotVoxelItemArray;
	}
	else if (InvenType == EInventoryType::Inventory)
	{
		return InventoryVoxelItemArray;
	}
	else
	{
		LOG("NOT ARRAY");
		return TArray<FVoxelItemInfo>();
	}
}

FVoxelItemInfo UQuickSlot::GetItemArrayByIndex(EInventoryType InvenType, int32 index)
{
	if (InvenType == EInventoryType::QuickSlot)
	{
		return QuickSlotVoxelItemArray[index];
	}
	else if (InvenType == EInventoryType::Inventory)
	{
		return InventoryVoxelItemArray[index];
	}
	else
	{
		LOG("NOT ARRAY");
		return FVoxelItemInfo();
	}
}

void UQuickSlot::CurrentVoxelMinusOne()
{
	auto& cQuickSlotItemInfo= QuickSlotVoxelItemArray[CurrentVoxelItemIndex];

	if (cQuickSlotItemInfo.Num > 0)
	{
		QuickSlotVoxelItemArray[CurrentVoxelItemIndex].Num--;
	}

	if (cQuickSlotItemInfo.Num <= 0)
	{
		cQuickSlotItemInfo.VoxelType = EVoxelType::Empty;
		cQuickSlotItemInfo.Num = 0;
	}
}