// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyCharacter.h"
#include "VoxelChunk.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "DestroyedVoxel.h"
#include "QuickSlot.h"
#include "Portfolio_Game2_Common.h"

void AMyPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedCharacter = Cast<AMyCharacter>(InPawn);
		if (!ensure(PossessedCharacter))
		{
			return;
		}

		SetupInputComponent();
	}
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Voxel Size
	VoxelSize = 100.f;
	VoxelRangeInChunk = 16;
	VoxelRangeInChunkX2 = VoxelRangeInChunk * 2;
	ChunkSize = VoxelRangeInChunk * VoxelSize;

	ChunkXYRangeInWorld = 10;
	ChunkZRangeInWorld = 3;
	MaxChunkRadius = ChunkSize * ChunkXYRangeInWorld;

	DestroyVoxelChunkIndex = -1;
	PlayerStandChunkIndex = 0;

	LootingRadius = 200.f;

	// Create QuickSlot Object
	QuickSlotUI = NewObject<UQuickSlot>();

	// Initialize Map
	InitChunkMap();
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckPlayerStandChunk();
	LootingVoxel();
}

/* 
 * Initialize Chunk Map
 */
void AMyPlayerController::InitChunkMap()
{
	FVector PlayerLocation = GetPawn()->GetActorLocation();
	PlayerLocation /= ChunkSize;

	const int x = floor(PlayerLocation.X);
	const int y = floor(PlayerLocation.Y);
	const int z = floor(PlayerLocation.Z);

	TArray<bool> bHasVoxel;
	bHasVoxel.Init(false, VoxelRangeInChunk * VoxelRangeInChunk);
	for (int i = -ChunkXYRangeInWorld; i <= ChunkXYRangeInWorld; ++i)
	{
		for (int j = -ChunkXYRangeInWorld; j <= ChunkXYRangeInWorld; ++j)
		{
			for (int k = -ChunkZRangeInWorld; k <= ChunkZRangeInWorld; ++k)
			{
				FVector SpawnChunkCoord(x + i, y + j, z + k);
				int32 SpawnChunkIndex = FindChunkIndex(SpawnChunkCoord);

				AVoxelChunk* SpawnChunk = GetWorld()->SpawnActor<AVoxelChunk>(FVector(SpawnChunkCoord * ChunkSize), FRotator::ZeroRotator);
				SpawnChunk->SetChunkIndex(SpawnChunkCoord);
				bool ret = SpawnChunk->GenerateVoxelType(FVector(SpawnChunkCoord));
				if (ret)
				{
					ChunkArray.Add(MoveTemp(SpawnChunk));
				}
				else
				{
					SpawnChunk->Destroy();
				}
			}
		}
	}
}

/*
 * Generate and Update Chunk
 * (x, y) ++ -- -+ +-
 */
void AMyPlayerController::UpdateChunkMap()
{
	FVector PlayerLocation = GetPawn()->GetActorLocation();
	PlayerLocation /= ChunkSize;

	const int x = floor(PlayerLocation.X);
	const int y = floor(PlayerLocation.Y);
	const int z = floor(PlayerLocation.Z);
	FVector StandChunkCoord = FVector(x, y, z);
	int StandChunkIndex = FindChunkIndex(StandChunkCoord);

	static int x_i = 0; // 0 1 2 3 ... ChunkRange 0 1 2 ... 
	static int y_i = 1; // 1 -1 1 -1
	static int g_i = 1; // odd even check
	
	/* y = -ChunkRange ~ 0 / 0 ~ ChunkRange */
	int yStart = (y_i == -1) ? -ChunkXYRangeInWorld : 0;
	int yEnd = yStart + ChunkXYRangeInWorld;
	if (x_i == 0)
	{
		yStart = -ChunkXYRangeInWorld;
		yEnd = ChunkXYRangeInWorld;
	}

	/*
	for (int j = yStart; j <= yEnd; ++j)
	{
		FVector2D CheckChunkCoord(x + x_i, y + j);
		int CheckChunkIndex = FindChunkIndex(CheckChunkCoord);

		if (CheckChunkIndex == -1)
		{
			if (CheckRadius(FVector(CheckChunkCoord * ChunkSize, 0.f), MaxChunkRadius))
			{
				AVoxelChunk* SpawnChunk = GetWorld()->SpawnActor<AVoxelChunk>(FVector(CheckChunkCoord * ChunkSize, 0.f), FRotator::ZeroRotator);
				SpawnChunk->SetChunkIndex(CheckChunkCoord);
				SpawnChunk->GenerateVoxelType(FVector(CheckChunkCoord, 0.f));
				ChunkArray.Add(MoveTemp(SpawnChunk));
			}
		}
		else
		{
			auto& CheckChunk = ChunkArray[CheckChunkIndex];

			if ((StandChunkIndex != CheckChunkIndex) && (CheckChunk->IsCurrentChunk()))
			{
				CheckChunk->SetIsCurrentChunk(false);
				CheckChunk->RefreshLeaves();
			}
		}
	}
	*/

	static const int32 chunkTotalSize = VoxelRangeInChunk * VoxelRangeInChunk;

	for (int j = yStart; j <= yEnd; ++j)
	{
		for (int k = -ChunkZRangeInWorld; k <= ChunkZRangeInWorld; ++k)
		{
			FVector CheckChunkCoord(x + x_i, y + j, z + k);
			int CheckChunkIndex = FindChunkIndex(CheckChunkCoord);

			if (CheckChunkIndex == -1)
			{
				if (CheckRadius(FVector(CheckChunkCoord * ChunkSize), MaxChunkRadius))
				{
					AVoxelChunk* SpawnChunk = GetWorld()->SpawnActor<AVoxelChunk>(FVector(CheckChunkCoord * ChunkSize), FRotator::ZeroRotator);
					SpawnChunk->SetChunkIndex(CheckChunkCoord);
					bool ret = SpawnChunk->GenerateVoxelType(FVector(CheckChunkCoord));
					if (ret)
					{
						ChunkArray.Add(MoveTemp(SpawnChunk));
					}
					else
					{
						SpawnChunk->Destroy();
					}
				}
			}
			else
			{
				auto& CheckChunk = ChunkArray[CheckChunkIndex];

				if ((StandChunkIndex != CheckChunkIndex) && (CheckChunk->IsCurrentChunk()))
				{
					CheckChunk->SetIsCurrentChunk(false);
					CheckChunk->RefreshLeaves();
				}
			}
		}
	}

	/* Refresh Stand Chunk */
	for (int i = -2; i <= 2; ++i)
	{
		for (int j = -2; j <= 2; ++j)
		{
			for (int k = -2; k <= 2; ++k)
			{
				FVector StandAroundChunkCoord(x + i, y + j, z + k);
				int StandAroundChunkIndex = FindChunkIndex(StandAroundChunkCoord);

				if (StandAroundChunkIndex != -1)
				{
					auto& StandAroundChunk = ChunkArray[StandAroundChunkIndex];
					if (!StandAroundChunk->IsCurrentChunk())
					{
						StandAroundChunk->SetIsCurrentChunk(true);
						StandAroundChunk->RefreshLeaves();
					}
				}
			}
		}
	}

	RemoveChunk();

	// next chunk
	{
		// ++ -- -+ +- ++
		if (x_i == 0)
		{
			x_i += 1;
			return;
		}
		else if (g_i == 1) // multiply x and y by -1
		{
			x_i *= -1;
			y_i *= -1;
			g_i *= -1;
		}
		else if (g_i == -1) // multiply y by -1 // if x_i is positive, increase 1.
		{
			y_i *= -1;
			g_i *= -1;

			if ((x_i >= 1) && (y_i == 1))
			{
				x_i = (x_i + 1) % ChunkXYRangeInWorld;
			}
		}

		//x_i = (((x_i + ChunkRange) + 1) % (ChunkRangeX2 + 1)) - ChunkRange;
	}
}

/*
 * Check Radius and Remove Chunk that are out of range.
 */
void AMyPlayerController::RemoveChunk()
{
	int32 index = 0;

	for (int i = 0; i < ChunkArray.Num(); ++i)
	{
		const FVector CurrentChunkCoord(ChunkArray[i]->GetChunkIndex() * ChunkSize);

		if (!CheckRadius(CurrentChunkCoord, MaxChunkRadius))
		{
			ChunkArray[i]->Destroy();
			ChunkArray.RemoveAtSwap(i);
		}
	}
}

void AMyPlayerController::CheckPlayerStandChunk()
{
	FVector PlayerLocation = GetPawn()->GetActorLocation();

	// Calculate Chunk Index by player location
	PlayerLocation /= ChunkSize;
	PlayerLocation.X = floor(PlayerLocation.X);
	PlayerLocation.Y = floor(PlayerLocation.Y);
	PlayerLocation.Z = floor(PlayerLocation.Z);

	FVector PlayerChunkVector(PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z);
	int PlayerChunkIndex = FindChunkIndex(PlayerChunkVector);

	const int updateIndex = (4 * ChunkXYRangeInWorld + 1) * (ChunkZRangeInWorld * 2 + 1); // 7 is z
	// Plyaer Move && End Update
	if ((PlayerChunkIndex != -1) && (PlayerStandChunkIndex != PlayerChunkIndex) && (GetWorldTimerManager().GetTimerElapsed(MapLoadTimerHandle) == -1))
	{
		GetWorldTimerManager().SetTimer(MapLoadTimerHandle, this, &AMyPlayerController::UpdateChunkMap, 0.2f, true, updateIndex * 0.1f);

		if (PlayerStandChunkIndex < ChunkArray.Num())
		{
			ChunkArray[PlayerStandChunkIndex]->SetIsRunningTime(false);
			ChunkArray[PlayerChunkIndex]->SetIsRunningTime(true);

			PlayerStandChunkIndex = PlayerChunkIndex;
		}
	}
}

void AMyPlayerController::SetDestroyVoxelValueZero()
{
	if (DestroyVoxelChunkIndex != -1)
	{
		ChunkArray[DestroyVoxelChunkIndex]->InitDestroyVoxel();
		DestroyVoxelChunkIndex = -1;
	}
}

bool AMyPlayerController::CheckRadius(const FVector& ChunkCoord, const float Radius)
{
	FVector PlayerLocation = GetPawn()->GetActorLocation();

	// Chunk Range Check
	if (Radius == MaxChunkRadius)
	{
		// Calculate Chunk Index by player location
		PlayerLocation /= ChunkSize;
		PlayerLocation.X = floor(PlayerLocation.X);
		PlayerLocation.Y = floor(PlayerLocation.Y);
		PlayerLocation.Z = floor(PlayerLocation.Z);

		// Calculate Location Coord by index
		PlayerLocation *= ChunkSize;
	}

	// Vector between player and chunk
	const FVector PlayerChunkVector = ChunkCoord - PlayerLocation;
	const float vectorLength = PlayerChunkVector.Size();

	if (vectorLength <= Radius)
	{
		return true;
	}

	return false;
}

int32 AMyPlayerController::FindChunkIndex(const FVector& findIndex)
{
	for (int i = 0; i < ChunkArray.Num(); ++i)
	{
		if (ChunkArray[i]->GetChunkIndex() == findIndex)
		{
			return i;
		}
	}
	return -1;
}

void AMyPlayerController::PlaceVoxel()
{
	if (!QuickSlotUI->isValidCurrentVoxelItem() || bIsOpenInventory)
	{
		return;
	}

	FHitResult Hit;
	int32 index;

	if (CheckVoxel(Hit, index))
	{
		FVector PlaceLocation = Hit.Location + Hit.Normal;
		PlaceLocation /= ChunkSize;
		FVector PlaceLocationChunkIndex(floor(PlaceLocation.X), floor(PlaceLocation.Y), floor(PlaceLocation.Z));
		index = FindChunkIndex(PlaceLocationChunkIndex);

		FVector VoxelLocalLocation = Hit.Location - ChunkArray[index]->GetActorLocation() + Hit.Normal;
		EVoxelType PlaceVoxelType = QuickSlotUI->GetCurrentVoxelItem().VoxelType;

		if (ChunkArray[index]->SetVoxel(VoxelLocalLocation, PlaceVoxelType))
		{
			QuickSlotUI->CurrentVoxelMinusOne();
		}
	}
}

void AMyPlayerController::DestroyVoxel(float Value)
{	
	if (bIsOpenInventory)
	{
		return; 
	}

	if (Value != 0.f)
	{
		FHitResult Hit;
		int32 index;
		if (CheckVoxel(Hit, index))
		{
			if (index != DestroyVoxelChunkIndex)
			{
				SetDestroyVoxelValueZero();
				DestroyVoxelChunkIndex = index;
			}

			FVector DirectionVector = (Hit.Location - GetPawn()->GetActorLocation()).GetSafeNormal();
			FVector HitLocation = Hit.Location + DirectionVector;
			FVector VoxelLocalLocation = HitLocation - ChunkArray[index]->GetActorLocation();

			// DestroyedVoxelType - Return the type of destroyed voxel
			EVoxelType DestroyedVoxelType = EVoxelType::Empty;
			if (ChunkArray[index]->DestroyVoxel(VoxelLocalLocation, DestroyedVoxelType, Value))
			{
				ADestroyedVoxel* DestroyedVoxel = GetWorld()->SpawnActor<ADestroyedVoxel>(HitLocation, FRotator::ZeroRotator);
				DestroyedVoxel->GenerateVoxel(HitLocation, DestroyedVoxelType);
				DestroyedVoxel->SetNum(1);
				DestroyedVoxelArray.Add(MoveTemp(DestroyedVoxel));
			}
		}
	}
}

bool AMyPlayerController::CheckVoxel(FHitResult& OutHit, int32& index)
{
	FVector CamLocation;
	FRotator CamRotation;
	GetPlayerViewPoint(CamLocation, CamRotation);

	const FVector Direction = CamRotation.Vector().GetSafeNormal();

	FVector	StartTrace = CamLocation + Direction * 100.f;
	FVector EndTrace = StartTrace + Direction * 1000.f;

	FCollisionQueryParams TraceParams(FName(TEXT("TraceParams")), true, this);
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(GetPawn());

	// Draw LineTrace
	const FName TraceTag("MyTraceTag");
	auto world = GetWorld();
	world->DebugDrawTraceTag = TraceTag;
	TraceParams.TraceTag = TraceTag;

	GetWorld()->LineTraceSingleByChannel(OutHit, StartTrace, EndTrace, ECollisionChannel::ECC_WorldStatic, TraceParams);

	if (OutHit.GetActor() != NULL)
	{
		const float x = floor(OutHit.GetActor()->GetActorLocation().X / ChunkSize);
		const float y = floor(OutHit.GetActor()->GetActorLocation().Y / ChunkSize);
		const float z = floor(OutHit.GetActor()->GetActorLocation().Z / ChunkSize);

		FVector HitChunk(x, y, z);
		index = FindChunkIndex(HitChunk);

		return ((index != INDEX_NONE) ? true : false);
	}
	else
	{
		return false;
	}
}

void AMyPlayerController::LootingVoxel()
{
	// return no destroy voxel
	if (DestroyedVoxelArray.Num() <= 0)
	{
		return;
	}

	// for each DestroyedVoxelArray and check Radius between player and voxel 
	for (int index = 0; index < DestroyedVoxelArray.Num(); ++index)
	{
		// Current Destroyed Voxel
		auto& cDestroyedVoxel = DestroyedVoxelArray[index];

		// if voxel in LootingRaidus, Looting Voxel
		if (CheckRadius(cDestroyedVoxel->GetBaseLocation(), LootingRadius))
		{
			// check e`s voxelType and Destroy e
			FVoxelItemInfo LootingVoxelItem;
			LootingVoxelItem.VoxelType = cDestroyedVoxel->GetVoxelItemType();
			LootingVoxelItem.Num = cDestroyedVoxel->GetVoxelItemNum();

			if (QuickSlotUI->LootingVoxel(LootingVoxelItem))
			{
				// TODO : fly Voxel 
								// Destory Looting Voxel
				cDestroyedVoxel->Destroy();
				DestroyedVoxelArray.RemoveAtSwap(index);
			}

			continue;
		}

		// if voxel have no lifetime, Delete Voxel -> true (CheckLifeTime)
		if (cDestroyedVoxel->CheckLifeTime())
		{
			DestroyedVoxelArray.RemoveAtSwap(index);
		}
	}
}

void AMyPlayerController::SetIsOpenInventory(bool _bIsOpenInventory)
{
	bIsOpenInventory = _bIsOpenInventory;
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("RClick", IE_Pressed, this, &AMyPlayerController::PlaceVoxel);

	InputComponent->BindAxis("LClickScale", this, &AMyPlayerController::DestroyVoxel);
	InputComponent->BindAction("LClick", IE_Released, this, &AMyPlayerController::SetDestroyVoxelValueZero);
}
