// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
namespace MathFunc
{
	template <typename T>
	inline T AbsoluteValue(T a) { return (a < 0) ? -a : a; }
};
