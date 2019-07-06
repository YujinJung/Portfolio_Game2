// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"
#include "LandBlock.generated.h"

/**
 * 
 */
enum LandCubeState {
	Dirt,
	Grass
};

UCLASS()
class PORTFOLIO_GAME2_API ALandBlock : public ABlock
{
	GENERATED_BODY()

public:

private:
	FTimerHandle GrassTimerHandle;
	float GrassTime;

public:
	ALandBlock();

	LandCubeState State;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void GrowTimer();
	void GrowGrass();
	FORCEINLINE void SetGrassTime(float InGrassTime) { GrassTime = InGrassTime; };
	void SetState(LandCubeState InState) { UE_LOG(LogTemp, Warning, TEXT("set state"));  State = InState; }
};
