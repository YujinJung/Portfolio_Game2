// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Portfolio_Game2_Common.h"
#include "VoxelItemInfo.h"
#include "MyCharacter.generated.h"

class AVoxelChunk;
class ADestroyedVoxel;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		int32 VoxelRangeInChunk;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		int32 VoxelRangeInChunkX2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		int32 ChunkRangeInWorld;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		TArray<FVector2D> ChunkCoordArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta = (AllowPrivateAccess = true))
		TArray<AVoxelChunk*> ChunkArray;

	// Voxel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		float VoxelSize;
	
	// Destroy Voxel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		TArray<ADestroyedVoxel*> DestroyedVoxelArray;

	// Quick Slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		TArray<FVoxelItemInfo> QuickSlotVoxelItemArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		FVoxelItemInfo CurrentVoxelItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		int32 DestroyVoxelChunkIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		uint8 MaxVoxelItemNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
		float LootingRadius;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = true))
		bool isPause;




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

	// Place / Destroy Voxel 
	UFUNCTION(BlueprintCallable, Category = Voxel)
		void PlaceVoxel();
	UFUNCTION(BlueprintCallable, Category = Voxel)
		void DestroyVoxel(float Value);
	UFUNCTION(BlueprintCallable, Category = Voxel)
		bool CheckVoxel(FHitResult& OutHit, int32& index);

	// Quick Slot / Inventory System
	UFUNCTION(BlueprintCallable, Category = Voxel)
		void LootingVoxel();
	UFUNCTION(BlueprintPure, Category = Voxel)
		uint8 GetQuickSlotEmptyIndex(const FVoxelItemInfo& LootingVoxel);
	UFUNCTION(BlueprintPure, Category = Voxel)
		FORCEINLINE FVoxelItemInfo GetCurrentVoxelItem() const { return CurrentVoxelItem; }
	UFUNCTION(BlueprintPure, Category = Voxel)
		FORCEINLINE TArray<FVoxelItemInfo> GetQuickSlotVoxelItemArray() const { return QuickSlotVoxelItemArray; }

	UFUNCTION(BlueprintCallable, Category = Voxel)
		void SetCurrentVoxelItem(const FVoxelItemInfo& VoxelInfo);
	UFUNCTION(BlueprintCallable, Category = Voxel)
		void SetCurrentVoxelItemWithIndex(int32 index);
	UFUNCTION(BlueprintCallable, Category = Voxel)
		void SetQuickSlotVoxelItemArray(EVoxelType inVoxelType, int32 num, int32 index);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = UI)
		void PauseGame();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
