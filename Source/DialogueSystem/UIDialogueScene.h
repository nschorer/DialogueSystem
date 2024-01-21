// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIDialogueScene.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueSceneEvent);
DECLARE_DELEGATE(FOnFastForward)

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API UUIDialogueScene : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	//virtual void NativePreConstruct() override;
	//virtual void NativeConstruct() override;
	//virtual void NativeDestruct() override;
	//virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	UFUNCTION(BlueprintCallable)
	void BeginDialogue(class UDialogueAsset* Data);

	UFUNCTION(BlueprintCallable)
	void AdvanceDialogue();

	UFUNCTION(BlueprintCallable)
	void EndDialogue();
	

public:

	UPROPERTY(BlueprintAssignable)
	FDialogueSceneEvent OnBeginDialogue;

	UPROPERTY(BlueprintAssignable)
	FDialogueSceneEvent OnEndDialogue;

protected:
	//void SingleSpeaker(FDialogueEvent& CurrentEvent);
	//void Pause(FDialogueEvent& CurrentEvent);

protected:
	UUIDialogueBox* GetDialogueBox(FGameplayTag Tag);

	UFUNCTION(BlueprintImplementableEvent)
	void GatherDialogueBoxes();

	void RegisterDialogueBoxes();

	void AdvanceDialogueBox(UUIDialogueBox* DialogueBox, UDSDialogueLineAsset* CurrentLine);

	bool IsPreviousEventInProgress();
	void TryFinishPreviousEvent();

	void CleanUpLastEvent();

	bool ShowDialogueBox(UUIDialogueBox* DialogueBox, bool bForce = false);

	bool HideDialogueBox(UUIDialogueBox* DialogueBox, bool bForce = false);
	bool HideAllDialogueBoxes(bool bForce = false);

	void CachePlayerControllerInBoxes(class ADSPlayerController* DSController);

	UFUNCTION()
	void OnReadyToAdvance();

	FOnFastForward OnFastForwardDel;
	void OnFastForwardDialogue(UUIDialogueBox* DialogueBox);

	UFUNCTION()
	void ShowAnimationFinished(UUIDialogueBox* DialogueBox, UDSDialogueLineAsset* CurrentLine);

	UFUNCTION()
	void HideAnimationFinished();

	void FinalizeEndDialogue();

protected:

	UPROPERTY(BlueprintReadWrite)
	TArray<UUIDialogueBox*> DialogueBoxes;

	UPROPERTY()
	class UDialogueAsset* DialogueData;

	int32 DialogueIdx;
	bool bInProgress;

	bool bAutoAdvance;

	FTimerHandle PauseTimerHandle;

	UPROPERTY()
	TArray<UUIDialogueBox*> ActiveDialogueBoxes;
};
