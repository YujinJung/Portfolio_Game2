// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlockArray.h"
#include "MyCharacter.generated.h"

class ALandBlock;


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
	bool CheckBlock(FHitResult& OutHit, uint8 DuplicateCheck, FVector EndTrace, FVector StartTrace);
	bool CheckBlockName(AActor* Block, const FString& CheckBlockName);
	void PlaceBlock();
	void DestroyBlock();


private:
	const float BlockSize = 100.f;
	const int BlockRange = 20;
	UPROPERTY()
	class USimplexNoiseBPLibrary* SimplexNoiseLib;
	UPROPERTY()
	TArray<FBlockArray> PlacedBlockArray;

	FTimerHandle GrassTimerHandle;

public:
	// 0 - generate block / 1 - remove block
	int8 LockRemoveBlock;
	float CalcDensity(float x, float y);
	void AddBlocks(FBlockArray& PlacedBlock);
	void RemoveBlock();
	UFUNCTION()
	void GenerateBlockMap();

	FORCEINLINE float CalcBlockNumber(const float& a) { return ((a + (BlockSize * 0.5f)) / 100.f); };

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
