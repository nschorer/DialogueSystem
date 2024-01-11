// Fill out your copyright notice in the Description page of Project Settings.


#include "DSDialogueManager.h"
#include "DSPlayerController.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "UIDialogueScene.h"

UDSDialogueManager::UDSDialogueManager()
{

}

void UDSDialogueManager::Initialize(class ADSPlayerController* MainController)
{
	CachedController = MainController;
	MainController->OnInitiateDialogue.AddDynamic(this, &UDSDialogueManager::OnInitiateDialogue);
}

void UDSDialogueManager::OnInitiateDialogue(UDialogueAsset* DialogueData)
{
	if (!DialogueData) return;

	CachedDialogueScene = CreateWidget<UUIDialogueScene>(CachedController, DialogueSceneClass);
	if (CachedDialogueScene)
	{
		CachedDialogueScene->AddToViewport();

		CachedController->OnAdvanceDialogue.AddDynamic(this, &UDSDialogueManager::OnAdvanceDialogue);
		CachedDialogueScene->OnEndDialogue.AddDynamic(this, &UDSDialogueManager::OnEndDialogue);

		CachedDialogueScene->BeginDialogue(DialogueData);
	}
}

void UDSDialogueManager::OnAdvanceDialogue()
{
	CachedDialogueScene->AdvanceDialogue();
}

void UDSDialogueManager::OnEndDialogue()
{
	CachedDialogueScene->RemoveFromParent();

	CachedController->OnAdvanceDialogue.RemoveDynamic(this, &UDSDialogueManager::OnAdvanceDialogue);
	CachedDialogueScene->OnEndDialogue.RemoveDynamic(this, &UDSDialogueManager::OnEndDialogue);
}
