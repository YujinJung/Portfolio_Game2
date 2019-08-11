// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlot.h"
#include "GameFramework/PlayerState.h"

UQuickSlot::UQuickSlot()
{
	MaxVoxelItemNum = 64;
	QuickSlotVoxelItemArray.Init(FVoxelItemInfo(), 9);
	CurrentVoxelItem = QuickSlotVoxelItemArray[0];
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
	if ((CurrentVoxelItem.VoxelType == EVoxelType::Empty) || (CurrentVoxelItem.VoxelType == EVoxelType::Count))
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

	CurrentVoxelItem = VoxelInfo;
}

void UQuickSlot::SetCurrentVoxelItemWithIndex(int32 index)
{
	if ((0 <= index) && (index <= 9))
	{
		CurrentVoxelItem = QuickSlotVoxelItemArray[index];
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
