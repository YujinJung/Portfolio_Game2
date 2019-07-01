// Fill out your copyright notice in the Description page of Project Settings.


#include "LandBlock.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Classes/Materials/Material.h"
#include "ConstructorHelpers.h"

ALandBlock::ALandBlock()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LandBlockMesh(TEXT("/Engine/BasicShapes/Cube"));
	if (LandBlockMesh.Succeeded())
	{
		static ConstructorHelpers::FObjectFinder<UMaterial> LandBlockMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
		UMaterial* mat = CreateDefaultSubobject<UMaterial>(TEXT("LandBlockMaterial"));
		mat = LandBlockMat.Object;

		LandBlockMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LandBlockComponent"));
		LandBlockMeshComponent->SetStaticMesh(LandBlockMesh.Object);
		LandBlockMeshComponent->SetMaterial(0, mat);
		RootComponent = LandBlockMeshComponent;
		SetActorEnableCollision(true);
	}
}
