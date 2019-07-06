// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class ALandBlock;
UCLASS()
class PORTFOLIO_GAME2_API AMyCharacter : public ACharacter
{
private:
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	class UCameraComponent* FollowCamera;
	
public:
	// Sets default values for this character's properties
	AMyCharacter();

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

// Refresh Map
private:
	FTimerHandle MapRefreshTimeHandler;
	TDoubleLinkedList<TDoubleLinkedList<ALandBlock*>*> MapBlock;
	FVector PlayerPreviousLocation;
	const int BlockRange = 4;
	const float BlockSize = 100.f;

	FORCEINLINE int CalcXY(const float& Coord) { return static_cast<int>((Coord + (BlockSize / 2.f)) / 100.f); }
	void InitializeMap();
	void RefreshMapBaseX(int& OffsetX);
	void RefreshMapBaseY(int& OffsetY);
	void RefreshMap();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
