// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyCharacterController.generated.h"

/**
 * 
 */
UCLASS()
class AMyCharacterController : public APlayerController
{
	GENERATED_BODY()

private:
	virtual void SetPawn(APawn* InPawn) override;

	virtual void BeginPlay() override;

	virtual void Tick( float DeltaTime ) override;
	
};
