// Fill out your copyright notice in the Description page of Project Settings.
#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "LandBlock.h"
#include "TimerManager.h"
#include "MathFunc.h"
#include "VoxelBlock.h"
#include "Vector2D.h"

AMyCharacter::AMyCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 150.f);

	BaseTurnAtRate = 45.f;
	BaseLookUpAtRate = 45.f;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 50.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->AttachTo(CameraBoom, CameraBoom->SocketName);
	FollowCamera->bUsePawnControlRotation = false;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	GenerateBlockMap();

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
 * DuplicateCheck ; 0 : no check / 1 : Block location duplicate check
 */
bool AMyCharacter::CheckBlock(FHitResult &OutHit, FVector &HitLocation, int32 &index)
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
		FVector DirectionVector = (OutHit.Location - GetActorLocation()).GetSafeNormal();
		HitLocation = OutHit.Location + DirectionVector;

		const float x = floor(HitLocation.X / BlockSize);
		const float y = floor(HitLocation.Y / BlockSize);
		FVector2D HitChunk(x, y);

		index = PlacedBlockCoord.Find(HitChunk);

		return ((index != INDEX_NONE) ? true : false);
	}
	else
	{
		return false;
	}
}
bool AMyCharacter::CheckBlockName(AActor* Block, const FString &CheckBlockName)
{
	// LandBlock_number
	FString BlockName = Block->GetName();
	TArray<FString> OutArray;
	BlockName.ParseIntoArray(OutArray, TEXT("_"));
	BlockName = OutArray[0];
	UE_LOG(LogTemp, Warning, TEXT("%s"), *BlockName);

	if (BlockName.Equals(CheckBlockName))
	{
		return true;
	}
	return false;
}
void AMyCharacter::PlaceBlock()
{
	FHitResult Hit;
	FVector HitLocation;
	int32 index;

	UE_LOG(LogTemp, Warning, TEXT("Start"));
	if (CheckBlock(Hit, HitLocation, index))
	{
		UE_LOG(LogTemp, Warning, TEXT("In : %s"), *Hit.Normal.ToString());
		FVector VoxelLocalPosition = Hit.Location - PlacedBlockArray[index]->GetActorLocation() + Hit.Normal;
		PlacedBlockArray[index]->SetVoxel(VoxelLocalPosition, EVoxelType::Rock);
		UE_LOG(LogTemp, Warning, TEXT("End"));
	}
}
void AMyCharacter::DestroyBlock()
{
	FHitResult Hit;

	FVector HitLocation;
	int32 index;
	if (CheckBlock(Hit, HitLocation, index))
	{
		FVector VoxelLocalPosition = HitLocation - PlacedBlockArray[index]->GetActorLocation();

		PlacedBlockArray[index]->SetVoxel(VoxelLocalPosition, EVoxelType::Empty);
	}
}

// 1 2 3 4 5 6 7 8 9
// 0 x 0 x x 0 x
// 1 1 1 1 1 1 1
// 2 1 2 1 1 2 1 2 2
// 1 0 1 0 0 1 0 1 1

bool AMyCharacter::CheckRadius(const FVector& BlockCoord)
{
	const float maxLength = BlockSize * BlockRange;

	FVector PlayerLocation = GetActorLocation();

	PlayerLocation.Z = 0.f;

	PlayerLocation /= BlockSize;
	PlayerLocation.X = floor(PlayerLocation.X);
	PlayerLocation.Y = floor(PlayerLocation.Y);
	PlayerLocation *= BlockSize;

	const FVector PlayerBlockVector = BlockCoord - PlayerLocation;
	const float vectorLength = PlayerBlockVector.Size();

	if (vectorLength <= maxLength)
		return true;

	return false;
}

void AMyCharacter::RemoveBlock()
{
	int32 index = 0;

	for (int i = 0; i < PlacedBlockCoord.Num(); ++i)
	{
		const FVector BlockCoord(PlacedBlockCoord[i] * BlockSize, 0.f);
		if (!CheckRadius(BlockCoord))
		{
			PlacedBlockArray[i]->Destroy();
			PlacedBlockArray.RemoveAt(i);
			PlacedBlockCoord.RemoveAt(i);
		}

	}
}

void AMyCharacter::GenerateBlockMap()
{
	FVector PlayerLocation = GetActorLocation();
	PlayerLocation /= BlockSize;

	int x = floor(PlayerLocation.X);
	int y = floor(PlayerLocation.Y);

	for (int i = -BlockRange; i <= BlockRange; ++i)
	{
		for (int j = -BlockRange; j <= BlockRange; ++j)
		{
			int CurX = x + i;
			int CurY = y + j;

			FVector2D CurrentIndex = FVector2D(CurX, CurY);

			if (!PlacedBlockCoord.Contains(CurrentIndex))
			{
				if (CheckRadius(FVector(CurrentIndex * BlockSize, 0.f)))
				{
					AVoxelBlock* SpawnBlock = GetWorld()->SpawnActor<AVoxelBlock>(FVector(CurrentIndex * BlockSize, 0.f), FRotator::ZeroRotator);
					SpawnBlock->GenerateChunk(FVector(CurrentIndex, 0.f));
					PlacedBlockCoord.Add(CurrentIndex);
					PlacedBlockArray.Add(MoveTemp(SpawnBlock));
				}
			}

		}
	}
	RemoveBlock();
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GenerateBlockMap();
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("LClick", IE_Pressed, this, &AMyCharacter::DestroyBlock);
	PlayerInputComponent->BindAction("RClick", IE_Pressed, this, &AMyCharacter::PlaceBlock);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnAtRate", this, &AMyCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpAtRate", this, &AMyCharacter::LookUpAtRate);
}

