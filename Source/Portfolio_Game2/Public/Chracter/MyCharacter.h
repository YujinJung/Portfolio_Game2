// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class AVoxelChunk;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta=(AllowPrivateAccess = true))
	int ChunkRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta=(AllowPrivateAccess = true))
	int ChunkRangeX2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta=(AllowPrivateAccess = true))
	float ChunkSize;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta=(AllowPrivateAccess = true))
	TArray<FVector2D> ChunkCoordArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk, meta=(AllowPrivateAccess = true))
	TArray<AVoxelChunk*> ChunkArray;

private:
	//bool CheckBlockName(AActor* Block, const FString& CheckBlockName);
	FORCEINLINE float CalcBlockNumber(const float& a) { return ((a + (ChunkSize * 0.5f)) / 100.f); };

public:
	// Create / Update / Destory Chunk
	UFUNCTION(BlueprintCallable, Category = Chunk)
	void GenerateChunkMap();
	UFUNCTION(BlueprintCallable, Category = Chunk)
	void RemoveChunk();
	UFUNCTION(BlueprintCallable, Category = Chunk)
	bool CheckRadius(const FVector& ChunkCoord);

	// Place / Destroy Voxel 
	UFUNCTION(BlueprintCallable, Category = Voxel)
	void PlaceVoxel();
	UFUNCTION(BlueprintCallable, Category = Voxel)
	void DestroyVoxel();
	UFUNCTION(BlueprintCallable, Category = Voxel)
	bool CheckVoxel(FHitResult& OutHit, FVector& HitLocation, int32& index);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
