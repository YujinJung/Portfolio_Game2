// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class AVoxelChunk;
class ADestroyedVoxel;
class UQuickSlot;
enum class EVoxelType : uint8;

UCLASS()
class AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	virtual void SetPawn(APawn* InPawn) override;

	virtual void BeginPlay() override;

	virtual void Tick( float DeltaTime ) override;


private:
	FTimerHandle MapLoadTimerHandle;
	
	// Voxel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
	float VoxelSize;

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
	int32 ChunkXYRangeInWorld;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
	int32 ChunkZRangeInWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
	TArray<AVoxelChunk*> ChunkArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
	TMap<FString, EVoxelType> VoxelArray;

	// Refresh Chunk
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
	int32 PlayerStandChunkIndex;


	// Destroy Voxel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
	TArray<ADestroyedVoxel*> DestroyedVoxelArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
	int32 DestroyVoxelChunkIndex;

	// Quick Slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = QuickSlot, meta = (AllowPrivateAccess = true))
	UQuickSlot* QuickSlotUI;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = QuickSlot, meta = (AllowPrivateAccess = true))
	bool bIsOpenInventory;

	// Looting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
	float LootingRadius;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
	bool bIsChangeQuickSlot;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = true))
	bool bIsPause;


public:
	// Create / Update / Destory Chunk
	UFUNCTION(BlueprintCallable, Category = Chunk)
	void InitChunkMap();
	UFUNCTION(BlueprintCallable, Category = Chunk)
	void UpdateChunkMap();
	UFUNCTION(BlueprintCallable, Category = Chunk)
	void RemoveChunk();
	UFUNCTION(BlueprintCallable, Category = Chunk)
	int32 FindChunkIndex(const FIntVector& findIndex);

	UFUNCTION()
	void CheckPlayerStandChunk();


	// Place / Destroy Voxel 
	UFUNCTION(BlueprintCallable, Category = Voxel)
	void PlaceVoxel();
	UFUNCTION(BlueprintCallable, Category = Voxel)
	void DestroyVoxel(float Value);
	UFUNCTION(BlueprintCallable, Category = Voxel)
	bool CheckVoxel(FHitResult& OutHit, FIntVector& index);


	// Reset Destory Voxel
	UFUNCTION(BlueprintCallable, Category = Voxel)
	void SetDestroyVoxelValueZero();

	// Quick Slot / Inventory System
	UFUNCTION(BlueprintCallable, Category = Voxel)
	void LootingVoxel();
	
	UFUNCTION(BlueprintPure, Category = QuickSlot)
	FORCEINLINE bool IsChangeQuickSlot() const { return bIsChangeQuickSlot; }
	UFUNCTION(BlueprintCallable, Category = QuickSlot)
	void SetIsChangeQuickSlot(bool _bIsChangeQuickSlot) { bIsChangeQuickSlot = _bIsChangeQuickSlot; }

	UFUNCTION(BlueprintPure, Category = QuickSlot)
	FORCEINLINE UQuickSlot* GetQuickSlotUI() const { return QuickSlotUI; }

	UFUNCTION(BlueprintPure, Category = Inventory)
	FORCEINLINE bool IsOpenInventory() const { return bIsOpenInventory; }
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SetIsOpenInventory(bool _bIsOpenInventory);

	// Pause
	UFUNCTION(BlueprintCallable, Category = UI)
	void PauseGame();

	UFUNCTION(BlueprintPure, Category = UI)
	FORCEINLINE bool IsPause() const { return bIsPause; }

	virtual void SetupInputComponent() override;

public:
	UFUNCTION(BlueprintCallable, Category = Chunk)
	bool CheckRadius(const FVector& ChunkCoord, const float Radius);
};
