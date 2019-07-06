// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "LandBlock.h"
#include "MathFunc.h"

// Sets default values
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

	PlayerPreviousLocation = GetActorLocation();
	InitializeMap();

	GetWorldTimerManager().SetTimer(MapRefreshTimeHandler, this, &AMyCharacter::RefreshMap, 0.1f, true);
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
bool AMyCharacter::CheckBlock(FHitResult &OutHit, uint8 DuplicateCheck = 0, FVector EndTrace = FVector::ZeroVector, FVector StartTrace = FVector::ZeroVector)
{
	FVector CamLocation;
	FRotator CamRotation;
	Controller->GetPlayerViewPoint(CamLocation, CamRotation);

	const FVector Direction = CamRotation.Vector();
	if (DuplicateCheck == 0)
	{
		EndTrace = CamLocation + Direction * 2000.f;
	}
	if (StartTrace == FVector::ZeroVector)
	{
		StartTrace = CamLocation;
	}

	FCollisionQueryParams TraceParams(FName(TEXT("TraceParams")), true, this);
	TraceParams.bReturnPhysicalMaterial = true;
	GetWorld()->LineTraceSingleByChannel(OutHit, StartTrace, EndTrace, ECollisionChannel::ECC_WorldStatic, TraceParams);

	if (OutHit.GetActor() != NULL)
	{
		return true;
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
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *BlockName);

	if (BlockName.Equals(CheckBlockName))
	{
		return true;
	}
	return false;
}
void AMyCharacter::PlaceBlock()
{
	FHitResult Hit(ForceInit);
	AActor* HitBlock;

	auto MaxAbsoluteValue = [](float& a, float& b) -> float& {
		using namespace MathFunc;
		if (AbsoluteValue<float>(a) > AbsoluteValue<float>(b))
		{
			b = 0.f; return a;
		}
		else
		{
			a = 0.f; return b;
		}
	};
	
	if (CheckBlock(Hit))
	{
		HitBlock = Hit.GetActor();
		const FVector HitBlockLocation = HitBlock->GetActorLocation();
		FVector PlaceDirection = Hit.Location - HitBlockLocation;
		// TODO: Block Rotation check
		const FRotator Rotation = FRotator::ZeroRotator;

		// Vector; HitActor`s Center -> HitActor`s Hit Location 
		// Set zero except largest coord value 
		float& MaxValue = MaxAbsoluteValue(MaxAbsoluteValue(PlaceDirection.X, PlaceDirection.Y), PlaceDirection.Z);
		FVector PlaceLocation = PlaceDirection * 2.0f + HitBlockLocation;

		// Block Location Duplication Check
		FHitResult DuplicateHitResult;
		if (!CheckBlock(DuplicateHitResult, 1, PlaceLocation))
		{
			GetWorld()->SpawnActor<ALandBlock>(PlaceLocation, Rotation);
		}

		if(CheckBlockName(HitBlock, FString(TEXT("LandBlock"))))
		{
			ALandBlock* LandBlock = Cast<ALandBlock>(HitBlock);
			LandBlock->SetState(LandCubeState::Dirt);
			LandBlock->SetGrassTime(0.f);
			LandBlock->SetMaterial(BlockSurface::Top, TEXT("/Game/MinecraftContents/Materials/Block/M_Dirt"));
			LandBlock->SetMaterial(BlockSurface::Side, TEXT("/Game/MinecraftContents/Materials/Block/M_Dirt"));
		}
	}
}
void AMyCharacter::DestroyBlock()
{
	FHitResult Hit;

	if (CheckBlock(Hit))
	{
		const FVector HitBlockLocation = Hit.GetActor()->GetActorLocation();

		// Bottom Block Check
		FHitResult BottomBlockHit;
		const FVector HitBlockBottomLocation(HitBlockLocation.X, HitBlockLocation.Y, HitBlockLocation.Z - 100.f);
		if (CheckBlock(BottomBlockHit, 1, HitBlockBottomLocation, HitBlockLocation))
		{
			AActor* BottomBlock = BottomBlockHit.GetActor();
			if (CheckBlockName(BottomBlock, FString(TEXT("LandBlock"))))
			{
				// If Bottom Block is LandBlock, set the grass to grow.
				ALandBlock* LandBlock = Cast<ALandBlock>(BottomBlock);
				LandBlock->SetState(LandCubeState::Grass);
				LandBlock->GrowTimer();
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("Destroy Block : %s"), *Location.ToString());
		Hit.GetActor()->Destroy();
	}
}

void AMyCharacter::InitializeMap()
{
	const FRotator SpawnRotation(FRotator::ZeroRotator);
	FVector PlayerBaseBlock;
	PlayerBaseBlock.X = CalcXY(PlayerBaseBlock.X) * 100.f;
	PlayerBaseBlock.Y = CalcXY(PlayerBaseBlock.Y) * 100.f;
	PlayerBaseBlock.Z = 0.f;

	for (int X = -BlockRange; X <= BlockRange; ++X)
	{
		UE_LOG(LogTemp, Warning, TEXT("%d"), X);
		TDoubleLinkedList<ALandBlock*>* MapBlockX = new TDoubleLinkedList<ALandBlock*>();
		for (int Y = -BlockRange; Y <= BlockRange; ++Y)
		{
			FVector SpawnLocation(PlayerBaseBlock);

			SpawnLocation.X += (X * 100.f);
			SpawnLocation.Y += (Y * 100.f);

			ALandBlock* SpawnBlock = GetWorld()->SpawnActor<ALandBlock>(SpawnLocation, SpawnRotation);
			(*MapBlockX).AddTail(MoveTemp(SpawnBlock));
		}
		MapBlock.AddTail(MoveTemp(MapBlockX));
	}
	
}
void AMyCharacter::RefreshMapBaseX(int& OffsetX)
{
	bool isTop= ((OffsetX < 0) ? true : false);
	OffsetX = (isTop) ? (-1 * OffsetX) : OffsetX;

	for (int i = 1; i <= OffsetX; ++i)
	{
		TDoubleLinkedList<ALandBlock*>* SpawnBlocks = new TDoubleLinkedList<ALandBlock*>();
		if (isTop) // X - 100
		{
			TDoubleLinkedList<ALandBlock*>* Head = MapBlock.GetHead()->GetValue();
			for (auto& e : *Head)
			{
				FVector SpawnLocation = e->GetActorLocation();
				SpawnLocation.X -= 100.f;
				ALandBlock* Block = GetWorld()->SpawnActor<ALandBlock>(SpawnLocation, FRotator::ZeroRotator);
				SpawnBlocks->AddHead(MoveTemp(Block));
			}

			MapBlock.AddHead(MoveTemp(SpawnBlocks));

			auto Tail = MapBlock.GetTail()->GetValue();
			for (auto& e : *Tail)
			{
				e->Destroy();
			}
			MapBlock.RemoveNode(MapBlock.GetTail());
		}
		else // X + 100
		{
			TDoubleLinkedList<ALandBlock*>* Tail = MapBlock.GetTail()->GetValue();
			for (auto& e : *Tail)
			{
				FVector SpawnLocation = e->GetActorLocation();
				SpawnLocation.X += 100.f;
				ALandBlock* Block = GetWorld()->SpawnActor<ALandBlock>(SpawnLocation, FRotator::ZeroRotator);
				SpawnBlocks->AddTail(MoveTemp(Block));
			}

			MapBlock.AddTail(MoveTemp(SpawnBlocks));

			auto Head = MapBlock.GetHead()->GetValue();
			for (auto& e : *Head)
			{
				e->Destroy();
			}
			MapBlock.RemoveNode(MapBlock.GetHead());
		}
	}

}
void AMyCharacter::RefreshMapBaseY(int& OffsetY)
{
	bool isLeft = ((OffsetY < 0) ? true : false);
	OffsetY = (isLeft) ? (-1 * OffsetY) : OffsetY;
	
	for (auto& e : MapBlock)
	{
		for (int i = 1; i <= OffsetY; ++i)
		{
			//TODO : Random Block
			if (isLeft)
			{
				FVector SpawnLocation = e->GetHead()->GetValue()->GetActorLocation();
				SpawnLocation.Y -= 100.f;
				ALandBlock* Block = GetWorld()->SpawnActor<ALandBlock>(SpawnLocation, FRotator::ZeroRotator);
				e->AddHead(MoveTemp(Block));

				ABlock* DeleteBlock = e->GetTail()->GetValue();
				DeleteBlock->Destroy();
				e->RemoveNode(e->GetTail());
			}
			else
			{
				FVector SpawnLocation = e->GetTail()->GetValue()->GetActorLocation();
				SpawnLocation.Y += 100.f;
				ALandBlock* Block = GetWorld()->SpawnActor<ALandBlock>(SpawnLocation, FRotator::ZeroRotator);
				e->AddTail(MoveTemp(Block));

				ABlock* DeleteBlock = e->GetHead()->GetValue();
				DeleteBlock->Destroy();
				e->RemoveNode(e->GetHead());
			}

		}
	}
}
void AMyCharacter::RefreshMap()
{
	const FVector PlayerLocation = GetActorLocation();

	int OffsetX = CalcXY(PlayerLocation.X) - CalcXY(PlayerPreviousLocation.X);
	int OffsetY = CalcXY(PlayerLocation.Y) - CalcXY(PlayerPreviousLocation.Y);

	if (OffsetX != 0)
	{
		RefreshMapBaseX(OffsetX);
	}
	if (OffsetY != 0)
	{
		RefreshMapBaseY(OffsetY);
	}

	// Store PlayerLocation for next tick
	PlayerPreviousLocation = PlayerLocation;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
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

