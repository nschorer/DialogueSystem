// Copyright Epic Games, Inc. All Rights Reserved.

#include "DSGameMode.h"
#include "DSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "DSDialogueManager.h"
#include "Kismet/GameplayStatics.h"
#include "DSPlayerController.h"
#include "DSDialogueAudio.h"

ADSGameMode::ADSGameMode()
{
	if (PawnClass)
	{
		DefaultPawnClass = PawnClass;
	}

	DialogueManager = CreateDefaultSubobject<UDSDialogueManager>(TEXT("Dialog Manager"));
}

void ADSGameMode::BeginPlay()
{
	Super::BeginPlay();

	//GetWorld()->GetTimerManager().SetTimer(DialogueManagerHandle, this, &ADSGameMode::SetupDialogueManager, 3.f, false);
	SetupDialogueManager();

	TArray<AActor*> DialogueAudioManagers;
	UGameplayStatics::GetAllActorsOfClass(this, ADSDialogueAudio::StaticClass(), DialogueAudioManagers);
	if (!DialogueAudioManagers.IsEmpty())
	{
		if (ADSDialogueAudio* InAudioManager = Cast<ADSDialogueAudio>(DialogueAudioManagers[0]))
		{
			RegisterDialogueAudioManager(InAudioManager);
		}
		
	}
}

void ADSGameMode::RegisterDialogueAudioManager(class ADSDialogueAudio* InDialogueAudioManager)
{
	DialogueAudioManager = InDialogueAudioManager;
}

void ADSGameMode::SetupDialogueManager()
{
	if (ADSPlayerController* DSController = Cast<ADSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		ensureMsgf(DialogueManagerClass, TEXT("Please configure a DialogueManagerClass in DSGameMode."));
		DialogueManager = NewObject<UDSDialogueManager>(this, DialogueManagerClass);
		DialogueManager->Initialize(DSController);
	}
}
