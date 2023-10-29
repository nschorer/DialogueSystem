// Fill out your copyright notice in the Description page of Project Settings.


#include "DSPlayerController.h"
#include "../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "DSCharacter_NPC.h"
#include "DSCharacter_Main.h"

void ADSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ADSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(AdvanceDialogueAction, ETriggerEvent::Triggered, this, &ADSPlayerController::AdvanceDialogue);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADSPlayerController::Look);
	}
}

void ADSPlayerController::AdvanceDialogue()
{
	OnAdvanceDialogue.Broadcast();
}

void ADSPlayerController::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// add yaw and pitch input to controller
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ADSPlayerController::BeginDialogue() const
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DialogueMappingContext, 10);

		if (ADSCharacter_Main* DSCharacter = Cast<ADSCharacter_Main>(GetPawn()))
		{
			DSCharacter->BeginDialogue();
		}
	}
}

void ADSPlayerController::EndDialogue() const
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(DialogueMappingContext);
		
		if (ADSCharacter_Main* DSCharacter = Cast<ADSCharacter_Main>(GetPawn()))
		{
			DSCharacter->EndDialogue();
		}
	}
}
