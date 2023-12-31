// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS(Blueprintable)
class EDT_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteract = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	int InteractIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector InteractPosition = FVector(0.0f, 0.0f, 0.0f);

	// Sets default values for this actor's properties
	AInteractable();

	UFUNCTION(BlueprintImplementableEvent)
	void Interact(float TimeToInteract = 0.0f);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
