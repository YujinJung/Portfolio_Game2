// Fill out your copyright notice in the Description page of Project Settings.
#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "VoxelChunk.h"
#include "DestroyedVoxel.h"
#include "QuickSlot.h"

// 0 - 1 3 , 2 - 1 5 , 6 - 3 7 , 8 - 7 5
FVector2D UpdateChunkCoord[3][3] = {
		{FVector2D(-1, 1), FVector2D(0, 1), FVector2D(1, 1)},
		{FVector2D(-1, 0), FVector2D(0, 0), FVector2D(1, 0)},
		{FVector2D(-1, -1), FVector2D(0, -1), FVector2D(1, -1)},
};

AMyCharacter::AMyCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 150.f);

	BaseTurnAtRate = 45.f;
	BaseLookUpAtRate = 45.f;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 50.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->AttachTo(CameraBoom, CameraBoom->SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Voxel Size
	VoxelSize = 100.f;
	VoxelRangeInChunk = 16;
	VoxelRangeInChunkX2 = VoxelRangeInChunk * 2;
	ChunkSize = VoxelRangeInChunk * VoxelSize;

	ChunkRangeInWorld = 8;
	MaxChunkRadius = ChunkSize * ChunkRangeInWorld;

	DestroyVoxelChunkIndex = -1;
	PlayerStandChunkIndex = 0;
	
	LootingRadius = 200.f;
	isPause = false;

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitChunkMap();
	GetWorldTimerManager().SetTimer(MapLoadTimerHandle, this, &AMyCharacter::CheckPlayerStandChunk, 0.1f, true);

	QuickSlotUI = NewObject<UQuickSlot>();
}

void AMyCharacter::MoveForward(float Value)
{
	if ((!isPause) && (Controller != NULL) && (Value != 0.f))
	{
		const FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if ((!isPause) && (Controller != NULL) && (Value != 0.f))
	{
		const FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::TurnAtRate(float Rate)
{
	if (!isPause)
	{
		AddControllerYawInput(Rate * BaseTurnAtRate * GetWorld()->GetDeltaSeconds());
	}
}

void AMyCharacter::LookUpAtRate(float Rate)
{
	if (!isPause)
	{
		AddControllerPitchInput(Rate * BaseLookUpAtRate * GetWorld()->GetDeltaSeconds());
	}
}

void AMyCharacter::InitChunkMap()
{
	FVector PlayerLocation = GetActorLocation();
	PlayerLocation /= ChunkSize;
	const int x = floor(PlayerLocation.X);
	const int y = floor(PlayerLocation.Y);

	for (int i = -ChunkRangeInWorld; i <= ChunkRangeInWorld; ++i)
	{
		for (int j = -ChunkRangeInWorld; j <= ChunkRangeInWorld; ++j)
		{
			FVector2D CurrentIndex(x + i, y + j);
			/*if (CheckRadius(FVector(CurrentIndex * ChunkSize, 0.f), MaxChunkRadius))
			{
				AVoxelChunk* SpawnChunk = GetWorld()->SpawnActor<AVoxelChunk>(FVector(CurrentIndex * ChunkSize, 0.f), FRotator::ZeroRotator);

				SpawnChunk->GenerateChunk(FVector(CurrentIndex, 0.f));

				ChunkCoordArray.Add(CurrentIndex);
				ChunkArray.Add(MoveTemp(SpawnChunk));
			}*/

		
			// Need spawn chunk
			// 0, 1, 2
			const int UpdateRowIndex = (i / ChunkRangeInWorld) + 1;
			const int UpdateColIndex = (j / ChunkRangeInWorld) + 1;
			const FVector2D UpdateOffset = UpdateChunkCoord[UpdateRowIndex][UpdateColIndex];
			int UpdateIndex = CalculateIndex(UpdateOffset.X, UpdateOffset.Y);

			GenerateChunk(CurrentIndex, UpdateIndex);
			/*
			 * 12 
			 * 3*   Draw 1,2 3 
			 */
			/*if ((UpdateIndex.X != 0) && (UpdateIndex.Y != 0))
			{
				// Draw 1
				UpdateChunkArray.Add(FVector(CurrentIndex + FVector2D(UpdateIndex.X, UpdateIndex.Y), CurIndex));

				// Draw 2 
				const int xIndex = (UpdateRowIndex + 1) % 2;
				CurIndex = CalculateIndex(xIndex, UpdateIndex.Y);
				UpdateChunkArray.Add(FVector(CurrentIndex + FVector2D(UpdateIndex.X, 0.f), CurIndex));

				// Draw 3 
				const int yIndex = (UpdateColIndex + 1) % 2;
				CurIndex = CalculateIndex(UpdateIndex.X, yIndex);
				UpdateChunkArray.Add(FVector(CurrentIndex + FVector2D(0, UpdateIndex.Y), CurIndex));
			}
			else if (UpdateIndex.X != 0)
			{
				UpdateChunkArray.Add(FVector(CurrentIndex + FVector2D(UpdateIndex.X, 0.f), CurIndex));
			}
			else if (UpdateIndex.Y != 0)
			{
				UpdateChunkArray.Add(FVector(CurrentIndex + FVector2D(0, UpdateIndex.Y), CurIndex));
			}*/
		}
	}
}

bool AMyCharacter::GenerateChunk(const FVector2D& CurrentIndex, int UpdateIndex)
{
	int UpdateXIndex = UpdateIndex / 3;
	int UpdateYIndex = UpdateIndex % 3;
	const FVector2D Offset = UpdateChunkCoord[UpdateXIndex][UpdateYIndex];

	if (CheckRadius(FVector(CurrentIndex * ChunkSize, 0.f), MaxChunkRadius))
	{
		AVoxelChunk* SpawnChunk = GetWorld()->SpawnActor<AVoxelChunk>(FVector(CurrentIndex * ChunkSize, 0.f), FRotator::ZeroRotator);

		SpawnChunk->GenerateChunk(FVector(CurrentIndex, 0.f));

		ChunkCoordArray.Add(CurrentIndex);
		ChunkArray.Add(MoveTemp(SpawnChunk));

		/*
		 * 12
		 * 3*   Draw 1,2 3
		 */
		if ((Offset.X != 0) && (Offset.Y != 0))
		{
			// Draw 1
			UpdateChunkArray.Add(FVector(Offset + FVector2D(Offset.X, Offset.Y), UpdateIndex));

			// Draw 2 
			const int xIndex = (UpdateXIndex + 1) % 2;
			UpdateIndex = CalculateIndex(xIndex, Offset.Y);
			UpdateChunkArray.Add(FVector(Offset + FVector2D(Offset.X, 0.f), UpdateIndex));

			// Draw 3 
			const int yIndex = (UpdateYIndex + 1) % 2;
			UpdateIndex = CalculateIndex(Offset.X, yIndex);
			UpdateChunkArray.Add(FVector(Offset + FVector2D(0, Offset.Y), UpdateIndex));
		}
		else if (Offset.X != 0)
		{
			UpdateChunkArray.Add(FVector(Offset + FVector2D(Offset.X, 0.f), UpdateIndex));
		}
		else if (Offset.Y != 0)
		{
			UpdateChunkArray.Add(FVector(Offset + FVector2D(0, Offset.Y), UpdateIndex));
		}

		return true;
	}

	return false;
}

/*
 * Generate and Update Chunk
 * (x, y) ++ -- -+ +-
 */
void AMyCharacter::UpdateChunkMap()
{
	FVector PlayerLocation = GetActorLocation();
	PlayerLocation /= ChunkSize;
	const int x = floor(PlayerLocation.X);
	const int y = floor(PlayerLocation.Y);

	for (int i = 0; i < UpdateChunkArray.Num(); ++i)
	{
		int32 index = UpdateChunkArray[i].Z;
		FVector2D CheckChunk(x + UpdateChunkArray[i].X, y + UpdateChunkArray[i].Y);

		if(GenerateChunk(CheckChunk, index))
		{
			UpdateChunkArray.RemoveAtSwap(i);
		}
	}

	/*
	auto GenerateChunk = [&](FVector2D ChunkIndex)  {
		if (CheckRadius(FVector(ChunkIndex * ChunkSize, 0.f), MaxChunkRadius))
		{
			AVoxelChunk* SpawnChunk = GetWorld()->SpawnActor<AVoxelChunk>(FVector(ChunkIndex * ChunkSize, 0.f), FRotator::ZeroRotator);

			SpawnChunk->GenerateChunk(FVector(ChunkIndex, 0.f));

			ChunkCoordArray.Add(ChunkIndex);
			ChunkArray.Add(MoveTemp(SpawnChunk));
		}
 	};

	static int i = -ChunkRangeInWorld;
	for (int j = -ChunkRangeInWorld; j <= 0; ++j)
	{
		FVector2D CurrentIndex = FVector2D(x + i, y + j);
		FVector2D CurrentOppositeIndex = FVector2D(x - i, y - j);

		if (!ChunkCoordArray.Contains(CurrentIndex))
		{
			GenerateChunk(CurrentIndex);
		}
		else if (!ChunkCoordArray.Contains(CurrentOppositeIndex))
		{
			GenerateChunk(CurrentOppositeIndex);
		}
		else
		{
			break;
		}
	}


	i = ((i == ChunkRangeInWorld) ? (-ChunkRangeInWorld) : (i + 1));
	*/
}

/*
 * Check Radius and Remove Chunk that are out of range.
 */
void AMyCharacter::RemoveChunk()
{
	int32 index = 0;

	for (int i = 0; i < ChunkCoordArray.Num(); ++i)
	{
		const FVector CurrentChunkCoord(ChunkCoordArray[i] * ChunkSize, 0.f);

		if (!CheckRadius(CurrentChunkCoord, MaxChunkRadius))
		{
			ChunkArray[i]->Destroy();
			ChunkArray.RemoveAt(i);
			ChunkCoordArray.RemoveAt(i);

			continue;
		}
	}
}

void AMyCharacter::CheckPlayerStandChunk()
{
	FVector PlayerLocation = GetActorLocation();

	// Calculate Chunk Index by player location
	PlayerLocation.Z = 0.f;
	PlayerLocation /= ChunkSize;
	PlayerLocation.X = floor(PlayerLocation.X);
	PlayerLocation.Y = floor(PlayerLocation.Y);

	FVector2D PlayerChunkVector = FVector2D(PlayerLocation.X, PlayerLocation.Y);
	int PlayerChunkIndex = ChunkCoordArray.Find(PlayerChunkVector);

	if (PlayerStandChunkIndex != PlayerChunkIndex)
	{
		UpdateChunkMap();
		RemoveChunk();

		ChunkArray[PlayerStandChunkIndex]->SetIsRunningTime(false);
		ChunkArray[PlayerChunkIndex]->SetIsRunningTime(true);

		PlayerStandChunkIndex = PlayerChunkIndex;
	}
}
bool AMyCharacter::CheckRadius(const FVector& ChunkCoord, const float& Radius)
{
	FVector PlayerLocation = GetActorLocation();

	// Chunk Range Check
	if (Radius == MaxChunkRadius)
	{
		// Calculate Chunk Index by player location
		PlayerLocation.Z = 0.f;
		PlayerLocation /= ChunkSize;
		PlayerLocation.X = floor(PlayerLocation.X);
		PlayerLocation.Y = floor(PlayerLocation.Y);

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


void AMyCharacter::PlaceVoxel()
{
	if(!QuickSlotUI->isValidCurrentVoxelItem())
	{
		return;
	}

	FHitResult Hit;
	int32 index;

	if (CheckVoxel(Hit, index))
	{
		FVector VoxelLocalLocation = Hit.Location - ChunkArray[index]->GetActorLocation() + Hit.Normal;
		EVoxelType PlaceVoxelType = QuickSlotUI->GetCurrentVoxelItem().VoxelType;
		QuickSlotUI->CurrentVoxelMinusOne();
		ChunkArray[index]->SetVoxel(VoxelLocalLocation, PlaceVoxelType);
	}
}

void AMyCharacter::DestroyVoxel(float Value)
{
	if (Value != 0.f)
	{
		FHitResult Hit;

		int32 index;
		if (CheckVoxel(Hit,index))
		{
			if (index != DestroyVoxelChunkIndex)
			{
				SetDestroyVoxelValueZero();
				DestroyVoxelChunkIndex = index;
			}

			FVector DirectionVector = (Hit.Location - GetActorLocation()).GetSafeNormal();
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

bool AMyCharacter::CheckVoxel(FHitResult& OutHit, int32& index)
{
	FVector CamLocation;
	FRotator CamRotation;
	Controller->GetPlayerViewPoint(CamLocation, CamRotation);

	const FVector Direction = CamRotation.Vector();

	FVector	StartTrace = CamLocation;
	FVector EndTrace = StartTrace + Direction * 1000.f;

	FCollisionQueryParams TraceParams(FName(TEXT("TraceParams")), true, this);
	TraceParams.bReturnPhysicalMaterial = true;

	// Draw LineTrace
	//const FName TraceTag("MyTraceTag");
	//auto world = GetWorld();
	//world->DebugDrawTraceTag = TraceTag;
	//TraceParams.TraceTag = TraceTag;

	GetWorld()->LineTraceSingleByChannel(OutHit, StartTrace, EndTrace, ECollisionChannel::ECC_WorldStatic, TraceParams);

	if (OutHit.GetActor() != NULL)
	{
		const float x = floor(OutHit.GetActor()->GetActorLocation().X / ChunkSize);
		const float y = floor(OutHit.GetActor()->GetActorLocation().Y / ChunkSize);

		FVector2D HitChunk(x, y);

		index = ChunkCoordArray.Find(HitChunk);

		return ((index != INDEX_NONE) ? true : false);
	}
	else
	{
		return false;
	}
}


void AMyCharacter::LootingVoxel()
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

void AMyCharacter::SetDestroyVoxelValueZero()
{
	if (DestroyVoxelChunkIndex != -1)
	{
		ChunkArray[DestroyVoxelChunkIndex]->InitDestroyVoxel();
		DestroyVoxelChunkIndex = -1;
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LootingVoxel();
}

void AMyCharacter::PauseGame()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		isPause = !isPause;

		if (isPause)
		{
			PlayerController->bShowMouseCursor = true;
			PlayerController->bEnableClickEvents = true;
			PlayerController->bEnableMouseOverEvents = true;
		}
		else if (!isPause)
		{
			PlayerController->bShowMouseCursor = false;
			PlayerController->bEnableClickEvents = false;
			PlayerController->bEnableMouseOverEvents = false;
		}
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	//PlayerInputComponent->BindAction("LClick", IE_Pressed, this, &AMyCharacter::DestroyVoxel);
	PlayerInputComponent->BindAction("RClick", IE_Pressed, this, &AMyCharacter::PlaceVoxel);

	PlayerInputComponent->BindAxis("LClickScale", this, &AMyCharacter::DestroyVoxel);
	PlayerInputComponent->BindAction("LClick", IE_Released, this, &AMyCharacter::SetDestroyVoxelValueZero);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnAtRate", this, &AMyCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpAtRate", this, &AMyCharacter::LookUpAtRate);
}
