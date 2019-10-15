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
	virtual void PossessedBy(AController* NewController) override;
	//virtual void UnPossessed() override;

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
	UFUNCTION(BlueprintCallable, Category = Move)
	void MoveForward(float Value);
	UFUNCTION(BlueprintCallable, Category = Move)
	void MoveRight(float Value);

	// Mouse Movement
	UFUNCTION(BlueprintCallable, Category = MouseMove)
	void TurnAtRate(float Rate);
	UFUNCTION(BlueprintCallable, Category = MouseMove)
	void LookUpAtRate(float Rate);


private:
	// UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = true))
		bool bIsPause;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = UI)
	void SetIsPause(bool _p) { bIsPause = _p; }

	UFUNCTION(BlueprintCallable, Category = UI)
	void PauseGame();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
