// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectMacros.h"

#define LOG(x, ...) UE_LOG(LogTemp, Warning, TEXT(x), __VA_ARGS__)

/*
 * Enum Voxel Type;
 */
UENUM(BlueprintType)
enum class EVoxelType : uint8 
{
	Empty,
	Dirt,
	Grass,
	Sand,
	Count
};

/**
 * 
 */
namespace MathFunc
{
	template <typename T>
	inline T AbsoluteValue(T a) { return (a < 0) ? -a : a; }
};


