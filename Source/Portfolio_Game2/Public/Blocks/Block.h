// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Block.generated.h"

/**
 * 
 */

UENUM()
enum BlockSurface {
	Side,
	Top,
	Bot
};

UCLASS()
class PORTFOLIO_GAME2_API ABlock : public AActor
{
	GENERATED_BODY()
	
private:
	// Need Material Path
	ABlock();

protected:
public:
	ABlock(const TCHAR *MaterialPath);
	ABlock(const TCHAR *MaterialPathSideBot, const TCHAR *MaterialPathTop);
	ABlock(const TCHAR *MaterialPathSide, const TCHAR *MaterialPathTop, const TCHAR *MaterialPathBot);

	virtual void Tick(float DeltaTime) override;
	void SetMaterial(BlockSurface InBlockSurface, const TCHAR* MaterialPath);

public:
	UStaticMeshComponent* BlockStaticMeshComponet;
};
