// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "LandBlock.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_GAME2_API ALandBlock : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	ALandBlock();
	const float BlockSize = 100.f;
	UStaticMeshComponent* LandBlockMeshComponent;


};
