// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerCharacter.h"
#include "ScheduleBlock.h"

// Sets default values
AWorkerCharacter::AWorkerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorkerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called when the game starts or when spawned
void AWorkerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Init Actions
	for (const TObjectPtr<UScheduleBlock>& Block : Blocks)
	{
		Actions.Add(NewObject<UScheduleAction>(Block, Block->Action));
	}

}

void AWorkerCharacter::UpdateBlockList(const TArray<UScheduleBlock*>& NewBlocks)
{
	Blocks.Reset(NewBlocks.Num());

	for (int i = 0; i < NewBlocks.Num(); i++)
	{
		Blocks[i] = NewBlocks[i];
	}
}

