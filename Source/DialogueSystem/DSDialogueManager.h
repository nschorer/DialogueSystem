// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DSDialogueManager.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DIALOGUESYSTEM_API UDSDialogueManager : public UObject
{
	GENERATED_BODY()
	
public:
	UDSDialogueManager();

	void Initialize(class ADSPlayerController* MainController);

protected:
	UFUNCTION()
	void OnInitiateDialogue(UDialogueAsset* DialogueData);

	UFUNCTION()
	void OnAdvanceDialogue();

	UFUNCTION()
	void OnEndDialogue();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUIDialogueScene> DialogueSceneClass;

protected:
	UPROPERTY()
	class ADSPlayerController* CachedController;

	UPROPERTY()
	class UUIDialogueScene* CachedDialogueScene;
};
