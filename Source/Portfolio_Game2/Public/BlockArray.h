// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandBlock.h"
#include "Containers/Array.h"
#include "BlockArray.generated.h"

USTRUCT(Atomic)
struct FBlockArray
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FVector2D BlockIndex;
	UPROPERTY(VisibleAnywhere)
	ALandBlock* PlacedBlock;
	UPROPERTY(VisibleAnywhere)
	int32 flags = 0;

public:
	FBlockArray()
		: BlockIndex(FVector2D::ZeroVector), PlacedBlock()
	{
	}
	FBlockArray(const FVector2D& v)
		: BlockIndex(v), PlacedBlock()
	{
	}
	FBlockArray(const FBlockArray& v)
		:BlockIndex(v.BlockIndex), PlacedBlock(v.PlacedBlock)
	{
	}
	
	FBlockArray& operator=(FBlockArray&& o)
	{
		BlockIndex = o.BlockIndex;
		PlacedBlock = o.PlacedBlock;
		o.PlacedBlock = nullptr;
		o.BlockIndex = FVector2D::ZeroVector;

		return *this;
	}
	FBlockArray& operator=(const FBlockArray& o)
	{
		BlockIndex = o.BlockIndex;
		PlacedBlock = o.PlacedBlock;
		return *this;
	}

	bool operator==(const FBlockArray& o) const
	{
		return ((o.BlockIndex.X == BlockIndex.X) && (o.BlockIndex.Y == BlockIndex.Y));
	}

	~FBlockArray()
	{
		PlacedBlock = nullptr;
		BlockIndex = FVector2D::ZeroVector;
	}
};

namespace ArrayFunc
{
	template<typename T>
	int32 Find(const TArray<T>& a, const T& b)
	{
		int32 index = 0;
		for (auto& e : a)
		{
			if (e == b)
				return index;
			++index;
		}
		return INDEX_NONE;
	}
}

