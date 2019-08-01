// Fill out your copyright notice in the Description page of Project Settings.
#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "VoxelChunk.h"
#include "DestroyedVoxel.h"

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
	ChunkRange = 16;
	ChunkRangeX2 = ChunkRange * 2;
	ChunkSize = ChunkRange * VoxelSize;

	DestroyVoxelChunkIndex = -1;
	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(MapLoadTimerHandle, this, &AMyCharacter::GenerateChunkMap, 0.1f, true);

	// Voxel Type
	//QuickSlotVoxelTypeArray.SetNumUninitialized(9);
	QuickSlotVoxelTypeArray.Init(EVoxelType::Empty, 9);
	CurrentVoxelType = QuickSlotVoxelTypeArray[0];
}

void AMyCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.f))
	{
		const FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.f))
	{
		const FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnAtRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpAtRate * GetWorld()->GetDeltaSeconds());
}

/*
 * Generate and Update Chunk
 * (x, y) ++ -- -+ +-
 */
void AMyCharacter::GenerateChunkMap()
{
	static int x_i = 0; // 0 1 2 3 ... ChunkRange 0 1 2 ... 
	static int y_i = 1; // 1 -1 1 -1
	static int g_i = 1; // odd even check

	// -ChunkRange ~ 0 | 0 ~ ChunkRange
	int yStart = (y_i == -1) ? -ChunkRange : 0;
	int yEnd = yStart + ChunkRange;
	if (x_i == 0)
	{
		yStart = -ChunkRange;
		yEnd = ChunkRange;
	}

	FVector PlayerLocation = GetActorLocation();
	PlayerLocation /= ChunkSize;

	const int x = floor(PlayerLocation.X);
	const int y = floor(PlayerLocation.Y);

	//for (int j = -ChunkRange; j <= ChunkRange; ++j)
	for (int j = yStart; j <= yEnd; ++j)
	{
		int CurX = x + x_i;
		int CurY = y + j;

		FVector2D CurrentIndex = FVector2D(CurX, CurY);

		if (!ChunkCoordArray.Contains(CurrentIndex))
		{
			if (CheckRadius(FVector(CurrentIndex * ChunkSize, 0.f)))
			{
				AVoxelChunk* SpawnChunk = GetWorld()->SpawnActor<AVoxelChunk>(FVector(CurrentIndex * ChunkSize, 0.f), FRotator::ZeroRotator);

				SpawnChunk->GenerateChunk(FVector(CurrentIndex, 0.f));

				ChunkCoordArray.Add(CurrentIndex);
				ChunkArray.Add(MoveTemp(SpawnChunk));
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
				x_i = (x_i + 1) % ChunkRange;
			}
		}

		//x_i = (((x_i + ChunkRange) + 1) % (ChunkRangeX2 + 1)) - ChunkRange;
	}
}

/*
 * Check Radius and Remove Chunk that are out of range.
 */
void AMyCharacter::RemoveChunk()
{
	int32 index = 0;

	for (int i = 0; i < ChunkCoordArray.Num(); ++i)
	{
		const FVector CurrnetChunkCoord(ChunkCoordArray[i] * ChunkSize, 0.f);
		if (!CheckRadius(CurrnetChunkCoord))
		{
			ChunkArray[i]->Destroy();
			ChunkArray.RemoveAt(i);
			ChunkCoordArray.RemoveAt(i);
		}
	}
}

bool AMyCharacter::CheckRadius(const FVector& ChunkCoord)
{
	const float maxLength = ChunkSize * ChunkRange;

	FVector PlayerLocation = GetActorLocation();

	PlayerLocation.Z = 0.f;

	PlayerLocation /= ChunkSize;
	PlayerLocation.X = floor(PlayerLocation.X);
	PlayerLocation.Y = floor(PlayerLocation.Y);
	PlayerLocation *= ChunkSize;

	// Vector between player and chunk
	const FVector PlayerChunkVector = ChunkCoord - PlayerLocation;
	const float vectorLength = PlayerChunkVector.Size();

	if (vectorLength <= maxLength)
		return true;

	return false;
}


/*
 * Place Voxel by CurrentVoxelType(QuickSlot)
 */
void AMyCharacter::PlaceVoxel()
{
	if ((CurrentVoxelType == EVoxelType::Empty) || (CurrentVoxelType == EVoxelType::Count))
	{
		return;
	}

	FHitResult Hit;
	FVector HitLocation;
	int32 index;

	if (CheckVoxel(Hit, HitLocation, index))
	{
		FVector VoxelLocalLocation = Hit.Location - ChunkArray[index]->GetActorLocation() + Hit.Normal;
		ChunkArray[index]->SetVoxel(VoxelLocalLocation, CurrentVoxelType);
	}
}

void AMyCharacter::DestroyVoxel(float Value)
{
	if (Value != 0.f)
	{
		FHitResult Hit;

		FVector HitLocation;
		int32 index;
		if (CheckVoxel(Hit, HitLocation, index))
		{
			if (index != DestroyVoxelChunkIndex)
			{
				SetDestroyVoxelValueZero();
				DestroyVoxelChunkIndex = index;
			}

			FVector VoxelLocalLocation = HitLocation - ChunkArray[index]->GetActorLocation();

			EVoxelType DestroyedVoxelType = EVoxelType::Empty;
			if (ChunkArray[index]->DestroyVoxel(VoxelLocalLocation, DestroyedVoxelType, Value))
			{
				ADestroyedVoxel* DestroyedVoxel = GetWorld()->SpawnActor<ADestroyedVoxel>(HitLocation, FRotator::ZeroRotator);
				DestroyedVoxel->GenerateVoxel(HitLocation, DestroyedVoxelType);
				DestroyedVoxelArray.Add(MoveTemp(DestroyedVoxel));
			}
		}
	}
}

bool AMyCharacter::CheckVoxel(FHitResult& OutHit, FVector& HitLocation, int32& index)
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
		/*if (!OutHit.GetActor()->GetClass()->IsChildOf(AVoxelChunk::StaticClass()))
			return false;*/

		FVector DirectionVector = (OutHit.Location - GetActorLocation()).GetSafeNormal();
		HitLocation = OutHit.Location + DirectionVector;

		const float x = floor(HitLocation.X / ChunkSize);
		const float y = floor(HitLocation.Y / ChunkSize);
		FVector2D HitChunk(x, y);

		index = ChunkCoordArray.Find(HitChunk);

		return ((index != INDEX_NONE) ? true : false);
	}
	else
	{
		return false;
	}
}

void AMyCharacter::SetVoxelType(int32 index)
{
	if ((0 <= index) && (index <= 9))
	{
		CurrentVoxelType = QuickSlotVoxelTypeArray[index];
	}
}

void AMyCharacter::SetQuickSlotVoxelTypeArray(EVoxelType inVoxelType, int32 index)
{
	if ((0 <= index) && (index <= 9))
	{
		QuickSlotVoxelTypeArray[index] = inVoxelType;
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

