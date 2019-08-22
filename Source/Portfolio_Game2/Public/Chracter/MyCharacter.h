// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Portfolio_Game2_Common.h"
#include "VoxelItemInfo.h"
#include "MyCharacter.generated.h"

class AVoxelChunk;
class ADestroyedVoxel;
class UQuickSlot;

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
	FTimerHandle MapLoadTimerHandle;

	// Chunk
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		float ChunkSize;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		float MaxChunkRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		int32 VoxelRangeInChunk;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		int32 VoxelRangeInChunkX2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		int32 ChunkRangeInWorld;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		TArray<AVoxelChunk*> ChunkArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		TArray<FVector> UpdateChunkArray;

	// Voxel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		float VoxelSize;
	
	// Destroy Voxel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		TArray<ADestroyedVoxel*> DestroyedVoxelArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		int32 DestroyVoxelChunkIndex;

	// Looting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		float LootingRadius;
	
	// Quick Slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = QuickSlot, meta = (AllowPrivateAccess = true))
		UQuickSlot* QuickSlotUI;

	// UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = true))
		bool isPause;

	// Refresh Chunk
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		int32 PlayerStandChunkIndex;

	UFUNCTION(BlueprintCallable, Category = Voxel)
		void SetDestroyVoxelValueZero();

private:
	//bool CheckBlockName(AActor* Block, const FString& CheckBlockName);
	FORCEINLINE float CalcBlockNumber(const float& a) { return ((a + (ChunkSize * 0.5f)) / 100.f); };

public:
	UFUNCTION(BlueprintCallable, Category = Chunk)
	void InitChunkMap();
	// Create / Update / Destory Chunk
	UFUNCTION(BlueprintCallable, Category = Chunk)
		void UpdateChunkMap();
	UFUNCTION(BlueprintCallable, Category = Chunk)
		void RemoveChunk();

	UFUNCTION(BlueprintCallable, Category = Chunk)
		bool CheckRadius(const FVector& ChunkCoord, const float Radius);

	UFUNCTION(BlueprintCallable, Category = Chunk)
	int32 FindChunkIndex(const FVector2D& findIndex);

	// Place / Destroy Voxel 
	UFUNCTION(BlueprintCallable, Category = Voxel)
		void PlaceVoxel();
	UFUNCTION(BlueprintCallable, Category = Voxel)
		void DestroyVoxel(float Value);
	UFUNCTION(BlueprintCallable, Category = Voxel)
		bool CheckVoxel(FHitResult& OutHit, int32& index);

	UFUNCTION()
	void CheckPlayerStandChunk();

	// Quick Slot / Inventory System
	UFUNCTION(BlueprintCallable, Category = Voxel)
		void LootingVoxel();
	UFUNCTION(BlueprintPure, Category = QuickSlot)
		UQuickSlot* GetQuickSlotUI() const { return QuickSlotUI; }

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = UI)
		void PauseGame();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
