// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include "ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Classes/Materials/Material.h"
#include "Containers/UnrealString.h"

ABlock::ABlock()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BlockStaticMeshFind(TEXT("/Game/Shapes/Block"));

	if (BlockStaticMeshFind.Succeeded())
	{
		//BlockStaticMesh = BlockStaticMeshFind.Object;
		BlockStaticMeshComponet = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMeshComponent"));
		BlockStaticMeshComponet->SetStaticMesh(BlockStaticMeshFind.Object);
		RootComponent = BlockStaticMeshComponet;
	}
	PrimaryActorTick.bCanEverTick = true;
}

ABlock::ABlock(const TCHAR *MaterialPath)
	: ABlock()
{
	if (BlockStaticMeshComponet->GetStaticMesh() != NULL)
	{
		UMaterial* BlockMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, MaterialPath));
		if (BlockMaterial)
		{
			BlockStaticMeshComponet->SetMaterial(BlockSurface::Side, BlockMaterial);
			BlockStaticMeshComponet->SetMaterial(BlockSurface::Top, BlockMaterial);
			BlockStaticMeshComponet->SetMaterial(BlockSurface::Bot, BlockMaterial);
		}
		SetActorEnableCollision(true);
	}
}

ABlock::ABlock(const TCHAR* MaterialPathSide, const TCHAR* MaterialPathTop, const TCHAR* MaterialPathBot)
	: ABlock()
{
	if (BlockStaticMeshComponet->GetStaticMesh() != NULL)
	{
		SetMaterial(BlockSurface::Side, MaterialPathSide);
		SetMaterial(BlockSurface::Top, MaterialPathTop);
		SetMaterial(BlockSurface::Bot, MaterialPathBot);

		SetActorEnableCollision(true);
	}
}

ABlock::ABlock(const TCHAR* MaterialPathSideBot, const TCHAR* MaterialPathTop)
	: ABlock(MaterialPathSideBot, MaterialPathTop, MaterialPathSideBot)
{
}

void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*
 * Set Material
 * @param ElementIndex - 0(Side), 1(Top), 2(Bottom)
 * @param MaterialPath - Material Path
 */
void ABlock::SetMaterial(BlockSurface InBlockSurface, const TCHAR* MaterialPath)
{
	UMaterial* BlockMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, MaterialPath));
	if (BlockMaterial)
	{
		BlockStaticMeshComponet->SetMaterial(InBlockSurface, BlockMaterial);
	}
}
