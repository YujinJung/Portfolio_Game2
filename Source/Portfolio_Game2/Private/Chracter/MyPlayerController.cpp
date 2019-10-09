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
	for (int index = 0; index < 4 * ChunkXYRangeInWorld + 1; ++index)
	{
		UpdateChunkMap();
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
	FIntVector StandChunkCoord(x, y, z);
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
		bool bFindTopChunk = false;
		for (int k = ChunkZRangeInWorld; k >= -ChunkZRangeInWorld; --k)
		{
			FIntVector CheckChunkCoord(x + x_i, y + j, z + k);
			int CheckChunkIndex = FindChunkIndex(CheckChunkCoord);

			if (CheckChunkIndex == -1)
			{
				if (CheckRadius(FVector(CheckChunkCoord * ChunkSize), MaxChunkRadius))
				{
					AVoxelChunk* SpawnChunk = GetWorld()->SpawnActor<AVoxelChunk>(FVector(CheckChunkCoord * ChunkSize), FRotator::ZeroRotator);
					SpawnChunk->SetChunkIndex(CheckChunkCoord);
					if (!bFindTopChunk)
					{
						SpawnChunk->SetIsTopChunk(true);
						bFindTopChunk = true;
					}
					bool ret = SpawnChunk->GenerateVoxelType(FVector(CheckChunkCoord));
					if (ret)
					{
						ChunkArray.Add(MoveTemp(SpawnChunk));
					}
					else
					{
						bFindTopChunk = false;
						SpawnChunk->Destroy();
					}
				}
			}
			else
			{
				bFindTopChunk = true;
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
				FIntVector StandAroundChunkCoord(x + i, y + j, z + k);
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

	FIntVector PlayerChunkVector(PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z);
	int PlayerChunkIndex = FindChunkIndex(PlayerChunkVector);

	const int updateIndex = 4 * ChunkXYRangeInWorld + 1; // 7 is z
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
	if (DestroyVoxelChunkIndex != -1 && DestroyVoxelChunkIndex < ChunkArray.Num())
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

int32 AMyPlayerController::FindChunkIndex(const FIntVector& findIndex)
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
	FIntVector ChunkCoord;
	if (CheckVoxel(Hit, ChunkCoord))
	{
		FVector PlaceLocation = Hit.Location + (Hit.Normal * VoxelSize / 2);
		FVector PlayerLocation = GetPawn()->GetActorLocation();

		// Player Position
		if (FIntVector(PlaceLocation / VoxelSize) == FIntVector(PlayerLocation / VoxelSize))
		{
			return;
		}
		
		PlaceLocation /= ChunkSize;
		FIntVector PlaceLocationChunkIndex(floor(PlaceLocation.X), floor(PlaceLocation.Y), floor(PlaceLocation.Z));
		int32 ChunkIndex = FindChunkIndex(PlaceLocationChunkIndex);

		EVoxelType PlaceVoxelType = QuickSlotUI->GetCurrentVoxelItem().VoxelType;

		if (ChunkIndex == INDEX_NONE)
		{
			
			AVoxelChunk* SpawnChunk = GetWorld()->SpawnActor<AVoxelChunk>(FVector(PlaceLocationChunkIndex * ChunkSize), FRotator::ZeroRotator);
			SpawnChunk->SetChunkIndex(PlaceLocationChunkIndex);
			bool ret = SpawnChunk->GenerateVoxelType(FVector(PlaceLocationChunkIndex));

			FVector VoxelLocalLocation = Hit.Location - SpawnChunk->GetActorLocation() + (VoxelSize / 2) * FVector::OneVector + Hit.Normal;
			VoxelLocalLocation = VoxelLocalLocation / VoxelSize + FVector::OneVector;

			if (!SpawnChunk->SetVoxel(FIntVector(VoxelLocalLocation), PlaceVoxelType))
			{
				return;
			}

			ChunkArray.Add(MoveTemp(SpawnChunk));
		}
		else
		{
			FVector VoxelLocalLocation = Hit.Location - ChunkArray[ChunkIndex]->GetActorLocation() + (VoxelSize / 2) * FVector::OneVector + Hit.Normal;
			VoxelLocalLocation = VoxelLocalLocation / VoxelSize + FVector::OneVector;
			if (!ChunkArray[ChunkIndex]->SetVoxel(FIntVector(VoxelLocalLocation), PlaceVoxelType))
			{
				return;
			}
		}
		QuickSlotUI->CurrentVoxelMinusOne();
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
		FIntVector ChunkCoord;
		if (!CheckVoxel(Hit, ChunkCoord)) 
		{
			return;
		}
		int32 ChunkIndex = FindChunkIndex(ChunkCoord);

		if (ChunkIndex != INDEX_NONE)
		{
			if (ChunkIndex != DestroyVoxelChunkIndex)
			{
				SetDestroyVoxelValueZero();
				DestroyVoxelChunkIndex = ChunkIndex;
			}

			FVector DirectionVector = (Hit.Location - GetPawn()->GetActorLocation()).GetSafeNormal();
			FVector HitLocation = Hit.Location + DirectionVector;
			FVector VoxelLocalLocation = HitLocation - ChunkArray[ChunkIndex]->GetActorLocation() + (VoxelSize / 2) * FVector::OneVector;
			VoxelLocalLocation = VoxelLocalLocation / VoxelSize + FVector::OneVector;

			// DestroyedVoxelType - Return the type of destroyed voxel
			EVoxelType DestroyedVoxelType = EVoxelType::Empty;
			if (ChunkArray[ChunkIndex]->DestroyVoxel(FIntVector(VoxelLocalLocation), DestroyedVoxelType, Value))
			{
				// Refresh
				{
					FIntVector v(VoxelLocalLocation);

					auto CheckSideChunk = [&](const FIntVector& ChunkOffset, const FIntVector& VoxelOffset1, const FIntVector& VoxelOffset2) {
						FIntVector CheckChunkCoord = ChunkCoord + ChunkOffset;
						int32 CheckChunkIndex = FindChunkIndex(CheckChunkCoord);
						FIntVector CheckVoxelLocalLocation(VoxelLocalLocation);
						CheckVoxelLocalLocation.X *= VoxelOffset1.X;
						CheckVoxelLocalLocation.Y *= VoxelOffset1.Y;
						CheckVoxelLocalLocation.Z *= VoxelOffset1.Z;

						CheckVoxelLocalLocation += VoxelOffset2;

						EVoxelType e = EVoxelType::Empty;
						if (CheckChunkIndex != -1)
						{
							ChunkArray[CheckChunkIndex]->SetVoxel(CheckVoxelLocalLocation, e, true);
						}
						else // no Chunk
						{
							AVoxelChunk* SpawnChunk = GetWorld()->SpawnActor<AVoxelChunk>(FVector(CheckChunkCoord * ChunkSize), FRotator::ZeroRotator);
							SpawnChunk->SetChunkIndex(CheckChunkCoord);
							bool ret = SpawnChunk->GenerateVoxelType(FVector(CheckChunkCoord));
							SpawnChunk->SetVoxel(CheckVoxelLocalLocation, e, true);
							ChunkArray.Add(MoveTemp(SpawnChunk));
						}
					};

					// 0 ~ 17 - 0, 17 is edge and 1 ~ 16 is chunk voxel
					// destroy voxel index 1 and 16 is first, last index voxel
					if (v.X == 1)
					{
						CheckSideChunk(FIntVector(-1, 0, 0), FIntVector(0, 1, 1), FIntVector(VoxelRangeInChunk + 1, 0, 0));
					}
					if (v.X == VoxelRangeInChunk)
					{
						CheckSideChunk(FIntVector(1, 0, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 0));
					}

					if (v.Y == 1)
					{
						CheckSideChunk(FIntVector(0, -1, 0), FIntVector(1, 0, 1), FIntVector(0, VoxelRangeInChunk + 1, 0));
					}
					if (v.Y == VoxelRangeInChunk)
					{
						CheckSideChunk(FIntVector(0, 1, 0), FIntVector(1, 0, 1), FIntVector(0, 0, 0));
					}

					if (v.Z == 1)
					{
						CheckSideChunk(FIntVector(0, 0, -1), FIntVector(1, 1, 0), FIntVector(0, 0, VoxelRangeInChunk + 1));
					}
					if (v.Z == VoxelRangeInChunk)
					{
						CheckSideChunk(FIntVector(0, 0, 1), FIntVector(1, 1, 0), FIntVector(0, 0, 0));
					}
				}


				// Drop Destroy Voxel
				//FVector DestroyedVoxelLocation = HitLocation/* - (20.f) * FVector::OneVector*/;
				FVector DestroyedVoxelLocation = FVector(ChunkCoord * ChunkSize + FIntVector(VoxelLocalLocation - (1.f) * FVector::OneVector) * VoxelSize);
				DestroyedVoxelLocation += DirectionVector * (VoxelSize * 0.25);
				ADestroyedVoxel* DestroyedVoxel = GetWorld()->SpawnActor<ADestroyedVoxel>(DestroyedVoxelLocation, FRotator::ZeroRotator);
				DestroyedVoxel->GenerateVoxel(DestroyedVoxelLocation, DestroyedVoxelType);
				DestroyedVoxel->SetNum(1);
				DestroyedVoxelArray.Add(MoveTemp(DestroyedVoxel));
			}
		}
	}
}

bool AMyPlayerController::CheckVoxel(FHitResult& OutHit, FIntVector& ChunkIndex)
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

		ChunkIndex = FIntVector(x, y, z);

		return true;
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
