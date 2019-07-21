// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlockArray.h"
#include "MyCharacter.generated.h"

class AVoxelBlock;


UCLASS()
class PORTFOLIO_GAME2_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	class UCameraComponent* FollowCamera;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnAtRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpAtRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Character Movement
	void MoveForward(float Value);
	void MoveRight(float Value);

	// Mouse Movement
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	
private:
	// Create / Destroy Block
	bool CheckBlock(FHitResult& OutHit, FVector& HitLocation, int32& index);
	bool CheckBlockName(AActor* Block, const FString& CheckBlockName);
	void PlaceBlock();
	void DestroyBlock();


private:
	const int BlockRange = 6;
	float BlockSize = 2000;
	UPROPERTY()
		TArray<FVector2D> PlacedBlockCoord;
	UPROPERTY()
		TArray<AVoxelBlock*> PlacedBlockArray;

	FTimerHandle GrassTimerHandle;

public:
	// 0 - generate block / 1 - remove block
	void RemoveBlock();
	UFUNCTION()
	void GenerateBlockMap();
	bool CheckRadius(const FVector& BlockCoord);

	FORCEINLINE float CalcBlockNumber(const float& a) { return ((a + (BlockSize * 0.5f)) / 100.f); };

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
