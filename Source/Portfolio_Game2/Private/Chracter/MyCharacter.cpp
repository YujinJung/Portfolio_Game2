// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "LandBlock.h"
#include "MathFunc.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

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
bool AMyCharacter::CheckBlock(FHitResult &OutHit, FVector EndTrace = FVector::ZeroVector, uint8 DuplicateCheck = 0)
{
	FVector CamLocation;
	FRotator CamRotation;
	Controller->GetPlayerViewPoint(CamLocation, CamRotation);

	const FVector Direction = CamRotation.Vector();
	if (DuplicateCheck == 0)
	{
		EndTrace = CamLocation + Direction * 2000.f;
	}

	FCollisionQueryParams TraceParams(FName(TEXT("TraceParams")), true, this);
	TraceParams.bReturnPhysicalMaterial = true;
	GetWorld()->LineTraceSingleByChannel(OutHit, CamLocation, EndTrace, ECollisionChannel::ECC_WorldStatic, TraceParams);

	if (OutHit.GetActor() != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
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
		const FVector HitBlockCenter = HitBlock->GetActorLocation();
		FVector PlaceDirection = Hit.Location - HitBlockCenter;
		// TODO: Block Rotation check
		const FRotator Rotation = FRotator::ZeroRotator;

		// Vector; HitActor`s Center -> HitActor`s Hit Location 
		// Set zero except largest coord value 
		float& MaxValue = MaxAbsoluteValue(MaxAbsoluteValue(PlaceDirection.X, PlaceDirection.Y), PlaceDirection.Z);
		FVector PlaceLocation = PlaceDirection * 2.0f + HitBlockCenter;

		// Block Location Duplication Check
		FHitResult DuplicateHitResult;
		if (!CheckBlock(DuplicateHitResult, PlaceLocation, 1))
		{
			GetWorld()->SpawnActor<ALandBlock>(PlaceLocation, Rotation);
		}
	}
}
void AMyCharacter::DestroyBlock()
{
	FHitResult Hit;

	if (CheckBlock(Hit))
	{
		//const FVector Location = Hit.GetActor()->GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("Destroy Block : %s"), *Location.ToString());
		Hit.GetActor()->Destroy();
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

