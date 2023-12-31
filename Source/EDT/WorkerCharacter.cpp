// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerCharacter.h"
#include "AIController.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "ScheduleBlock.h"

// Sets default values
AWorkerCharacter::AWorkerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorkerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bTimelineStarted && !bBlockRunning)
	{
		RunBlock(*Blocks[CurrentBlockIndex]);
		bBlockRunning = true;
	}

	if (bTimerRunning && bBlockRunning)
	{
		Timer += DeltaTime;
		CurrentBlockCompletion = FMath::GetMappedRangeValueClamped<float>(UE::Math::TVector2<float>(0.0f, Blocks[CurrentBlockIndex]->BlockDuration), UE::Math::TVector2<float>(0.0f, 1.0f), Timer);

		if (Timer >= Blocks[CurrentBlockIndex]->BlockDuration)
		{
			EndCurrentBlock();
		}
	}
}

// Called when the game starts or when spawned
void AWorkerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!AIController)
	{
		AIController = GetController<AAIController>();
		AIController->ReceiveMoveCompleted.AddDynamic(this, &AWorkerCharacter::OnMoveCompleted);
	}

	UGameplayStatics::GetAllActorsOfClass(this, AInteractable::StaticClass(), AllInteractablesActors);
	if (AllInteractablesActors.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No Actor To Interact, Should Not Append !"));
	}
}

void AWorkerCharacter::EndCurrentBlock()
{
	bInteract = false;
	bBlockRunning = false;
	bTimerRunning = false;
	Timer = 0.0f;
	CurrentBlockCompletion = 0.0f;
	CurrentActorToUse->bInteract = false;

	CurrentBlockIndex++;
	if (CurrentBlockIndex >= Blocks.Num())
	{
		CurrentBlockIndex = 0;
	}
	UE_LOG(LogTemp, Log, TEXT("Current Action Ended"));
}

void AWorkerCharacter::StartActionOfCurrentBlock()
{
	bInteract = true;
	bTimerRunning = true;
	CurrentActorToUse->bInteract = true;
	UE_LOG(LogTemp, Log, TEXT("Start Block Action"));
}

void AWorkerCharacter::StartTimeline()
{
	bTimelineStarted = true;
	UE_LOG(LogTemp, Log, TEXT("Start Timeline"));
}

void AWorkerCharacter::StopTimeline()
{
	bTimelineStarted = false;
	bTimerRunning = false;
}

void AWorkerCharacter::ResumeTimeline()
{
	bTimelineStarted = true;
	bTimerRunning = true;
}

void AWorkerCharacter::ResetTimeline()
{
	bTimelineStarted = false;
	bBlockRunning = false;
	bTimerRunning = false;
	Timer = 0.0f;
	CurrentBlockIndex = 0;
	CurrentBlockCompletion = 0.0f;
	UE_LOG(LogTemp, Log, TEXT("Reset Timeline"));
}

void AWorkerCharacter::RunBlock(const UScheduleBlock& Block)
{
	if (!AIController)
	{
		AIController = GetController<AAIController>();
		AIController->ReceiveMoveCompleted.AddDynamic(this, &AWorkerCharacter::OnMoveCompleted);
	}

	//Find Actor to Interact with
	for (int i = 0; i < AllInteractablesActors.Num(); i++)
	{
		AInteractable* actor = Cast<AInteractable>(AllInteractablesActors[i]);
		if (actor->InteractIndex == Block.ActorToInteractIndex)
		{
			CurrentActorToUse = actor;
		}
	}

	if (CurrentActorToUse == nullptr)
	{
		ResetTimeline();
		UE_LOG(LogTemp, Error, TEXT("No Actor To Interact, Should Not Append !"));
		return;
	}

	//Move To Actor
	FAIMoveRequest MoveReq;
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAcceptanceRadius(AcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
	MoveReq.SetGoalLocation(CurrentActorToUse->InteractPosition);
	MoveReq.SetNavigationFilter(AIController->GetDefaultNavigationFilterClass());

	FPathFollowingRequestResult ResultData = AIController->MoveTo(MoveReq);
	switch (ResultData.Code)
	{
	case EPathFollowingRequestResult::RequestSuccessful:
		UE_LOG(LogTemp, Log, TEXT("Worker Move To Goal"));
		break;

	case EPathFollowingRequestResult::AlreadyAtGoal:
		StartActionOfCurrentBlock();
		UE_LOG(LogTemp, Log, TEXT("Worker Already At Goal"));
		break;

	case EPathFollowingRequestResult::Failed:
		UE_LOG(LogTemp, Error, TEXT("Worker fail on RunBlock"));
		break;

	default:
		UE_LOG(LogTemp, Error, TEXT("Worker fail on RunBlock"));
		break;
	}
}

void AWorkerCharacter::OnMoveCompleted(FAIRequestID ID, EPathFollowingResult::Type Type)
{
	StartActionOfCurrentBlock();
	UE_LOG(LogTemp, Log, TEXT("Worker At Goal"));
}
