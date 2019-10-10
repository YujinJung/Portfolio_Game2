// Fill out your copyright notice in the Description page of Project Settings.


#include "DestroyedVoxel.h"
#include "ProceduralMeshComponent.h"
#include "Materials/Material.h"

const int32 bTriangles[] = { 2, 1, 0, 0, 3, 2 }; // draw triangle order
const FVector2D bUVs[] = { FVector2D(0.000000, 0.000000), FVector2D(0.000000, 1.000000), FVector2D(1.000000, 1.000000), FVector2D(1.000000, 0.000000) };
// top
const FVector bNormals0[] = { FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) };

// bot
const FVector bNormals1[] = { FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) };

// right
const FVector bNormals2[] = { FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0) };

// left
const FVector bNormals3[] = { FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0) };

// front
const FVector bNormals4[] = { FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) };

// back
const FVector bNormals5[] = { FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0) };

// Sets default values
ADestroyedVoxel::ADestroyedVoxel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	FString name = "Voxel_Destroyed";
	VoxelMeshComponent = NewObject<UProceduralMeshComponent>(this, *name);
	
	VoxelMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
	VoxelMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	//VoxelMeshComponent->RegisterComponent();
	//RootComponent = VoxelMeshComponent;

	voxelSize = 20;
	voxelHalfSize = voxelSize / 2;
	originalVoxelHalfSize = 50.f;
	RunningTime = 0.f;
	sinScale = 1.f;
	
	VoxelInfo.VoxelType = EVoxelType::Empty;
	VoxelInfo.Num = 0;

	AddVoxelMaterial(TEXT("M_Dirt"));
	AddVoxelMaterial(TEXT("M_Grass"));
	AddVoxelMaterial(TEXT("M_Sand"));
	AddVoxelMaterial(TEXT("M_Stone"));
	AddVoxelMaterial(TEXT("M_Log"));
	AddVoxelMaterial(TEXT("M_Leaves"));
	AddVoxelMaterial(TEXT("M_Leaves_Far"));
}


void ADestroyedVoxel::BeginPlay()
{
	Super::BeginPlay();
}

/* 
 * Called every frame
 * @ floating voxel
 * @ move downward
 */
void ADestroyedVoxel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector DestroyedVoxelLocation = GetActorLocation();
	FRotator DestroyedVoxelRotation = GetActorRotation();
	float RotationSpeed = 0.2f;

	/* floating voxel */
	if (!isDown)
	{
		float deltaHeight = sin(DeltaTime + RunningTime) - sin(RunningTime);

		DestroyedVoxelLocation.Z += deltaHeight * 10.f;
		SetActorLocation(DestroyedVoxelLocation);
		SetActorRotation(DestroyedVoxelRotation + FRotator(0.f, RotationSpeed, 0.f));

		RunningTime += DeltaTime;

		CheckGravity();
	}
	else /* move downward */
	{
		float deltaHeight = sinScale * (sin(DeltaTime + DownRunningTime) - sin(DownRunningTime));

		DestroyedVoxelLocation.Z += deltaHeight * 20.f;
		SetActorLocation(DestroyedVoxelLocation);

		DownRunningTime += DeltaTime;

		if (DestroyedVoxelLocation.Z <= (BaseLocation.Z + originalVoxelHalfSize))
		{
			isDown = false;
		}
	}
}


void ADestroyedVoxel::AddVoxelMaterial(FString MaterialName)
{
	FString MaterialPath = TEXT("/Game/MinecraftContents/Materials/Voxels/");
	MaterialPath.Append(MaterialName);
	UMaterial* VoxelMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, *MaterialPath));

	if (VoxelMaterial)
	{
		VoxelMaterials.Add(VoxelMaterial);
	}
}


/*
 * Check Gravity and Move downward Setting
 */
void ADestroyedVoxel::CheckGravity()
{
	FHitResult Hit;
	FVector Direction(0.0f, 0.0f, -1.0f);
	FVector	StartTrace = BaseLocation;
	FVector EndTrace = StartTrace + Direction * 1000.f;

	FCollisionQueryParams TraceParams(FName(TEXT("TraceParams")), true, this);
	TraceParams.bReturnPhysicalMaterial = true;

	// Draw LineTrace
	/*const FName TraceTag("MyTraceTag");
	auto world = GetWorld();
	world->DebugDrawTraceTag = TraceTag;
	TraceParams.TraceTag = TraceTag;*/

	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECollisionChannel::ECC_PhysicsBody, TraceParams);

	if (Hit.GetActor() == NULL)
	{
		return;
	}

	float DistanceBaseHit = BaseLocation.Z - Hit.Location.Z;
	DownScale = DistanceBaseHit / originalVoxelHalfSize;
	if (DistanceBaseHit < originalVoxelHalfSize)
	{
		DownScale = 0.f;
	}
	else
	{
		isDown = true;
		if (DistanceBaseHit > originalVoxelHalfSize)
			BaseLocation.Z -= DistanceBaseHit;

		sinScale = 1.5f * DownScale;
		DownRunningTime = PI / 2.f;
	}
}

void ADestroyedVoxel::SetNum(int32 num)
{
	VoxelInfo.Num = num;
}

void ADestroyedVoxel::GenerateVoxel(const FVector& VoxelLocation, EVoxelType e)
{
	if (e == EVoxelType::Empty)
	{
		return;
	}

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	VoxelInfo.VoxelType = e;

	int32 triangleVerticeNum = 0;

	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			Triangles.Add(bTriangles[j] + triangleVerticeNum);
		}
		triangleVerticeNum += 4;

		switch (i)
		{
		case 0:
		{
			Vertices.Add(FVector(-voxelHalfSize,voxelHalfSize,voxelHalfSize));
			Vertices.Add(FVector(-voxelHalfSize, -voxelHalfSize,voxelHalfSize));
			Vertices.Add(FVector(voxelHalfSize, -voxelHalfSize,voxelHalfSize));
			Vertices.Add(FVector(voxelHalfSize,voxelHalfSize,voxelHalfSize));

			Normals.Append(bNormals0, ARRAY_COUNT(bNormals0));
			break;
		}
		case 1:
		{
			Vertices.Add(FVector(voxelHalfSize, -voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(-voxelHalfSize, -voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(-voxelHalfSize,voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(voxelHalfSize,voxelHalfSize, -voxelHalfSize));

			Normals.Append(bNormals1, ARRAY_COUNT(bNormals1));
			break;
		}
		case 2:
		{
			Vertices.Add(FVector(voxelHalfSize,voxelHalfSize,voxelHalfSize));
			Vertices.Add(FVector(voxelHalfSize,voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(-voxelHalfSize,voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(-voxelHalfSize,voxelHalfSize,voxelHalfSize));

			Normals.Append(bNormals2, ARRAY_COUNT(bNormals2));
			break;
		}
		case 3:
		{
			Vertices.Add(FVector(-voxelHalfSize, -voxelHalfSize,voxelHalfSize));
			Vertices.Add(FVector(-voxelHalfSize, -voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(voxelHalfSize, -voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(voxelHalfSize, -voxelHalfSize,voxelHalfSize));

			Normals.Append(bNormals3, ARRAY_COUNT(bNormals3));
			break;
		}
		case 4:
		{
			Vertices.Add(FVector(voxelHalfSize, -voxelHalfSize,voxelHalfSize));
			Vertices.Add(FVector(voxelHalfSize, -voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(voxelHalfSize,voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(voxelHalfSize,voxelHalfSize,voxelHalfSize));

			Normals.Append(bNormals4, ARRAY_COUNT(bNormals4));
			break;
		}
		case 5:
		{
			Vertices.Add(FVector(-voxelHalfSize,voxelHalfSize,voxelHalfSize));
			Vertices.Add(FVector(-voxelHalfSize,voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(-voxelHalfSize, -voxelHalfSize, -voxelHalfSize));
			Vertices.Add(FVector(-voxelHalfSize, -voxelHalfSize,voxelHalfSize));

			Normals.Append(bNormals5, ARRAY_COUNT(bNormals5));
			break;
		}
		default:
			break;
		}

		UV.Append(bUVs, ARRAY_COUNT(bUVs));

		// red = 1 -> Grass
		FColor color(FColor(0, 0, 255, i));

		for (int j = 0; j < 4; ++j)
		{
			VertexColors.Add(color);
		}
	}


	VoxelMeshComponent->ClearAllMeshSections();

	VoxelMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UV, VertexColors, Tangents, true);

	VoxelMeshComponent->SetMaterial(0, VoxelMaterials[static_cast<int32>(e) - 1]);

	BaseLocation = VoxelLocation;
	SetActorLocation(BaseLocation);
	
	CheckGravity();
}

bool ADestroyedVoxel::CheckLifeTime()
{
	int32 LifeTime = 30.f;
	if (RunningTime > LifeTime)
	{
		this->Destroy();
		return true;
	}
	return false;
}
