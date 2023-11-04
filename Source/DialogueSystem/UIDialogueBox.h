// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMG/Public/Components/Image.h"
#include "DSTypes.h"
#include "UIDialogueBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueReady);
DECLARE_DELEGATE(FOnAnimationFinished);

struct FDialogueEvent;

/**
 * 
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UUIDialogueBox : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:

	UFUNCTION(BlueprintCallable)
	void PushNextDialogueEvent(const FDialogueEvent& NewDialogueEvent, bool bIsLastEvent);

	UFUNCTION(BlueprintCallable)
	void FastForward();

	UFUNCTION(BlueprintCallable)
	bool IsDialogueReady() const {return bDialogueReady;}

	UFUNCTION()
	void Show(bool bShow);

	UFUNCTION()
	void OnShowFinished();

	UFUNCTION()
	void OnReverseShowFinished();

	UFUNCTION()
	void OnHideFinished();

	// Sloppy
	void SetPlayerController(class ADSPlayerController* DSController) { CachedController = DSController; }

protected:
	// Grab data from FDialogueLine
	void UpdateSpeaker(const FDialogueEvent& NewDialogueEvent);
	void UpdateTextLine(const FDialogueEvent& NewDialogueEvent);
	void UpdateVoiceLine(const FDialogueEvent& NewDialogueEvent);
	void UpdateEmotion(const FDialogueEvent& NewDialogueEvent);

	void CancelInProgressEvents();

	UFUNCTION(BlueprintCallable)
		void SetReadyForNextLine();

// Child Widgets
protected:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* DialogueSpeakerText;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class URichTextBlock* DialogueBodyText;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	class UImage* SpeakerImage;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	class UWidgetSwitcher* ContinueOrEnd;

	UPROPERTY( Transient, meta=(BindWidgetAnimOptional))
	class UWidgetAnimation* AnimShow;

	UPROPERTY( Transient, meta=(BindWidgetAnimOptional))
	class UWidgetAnimation* AnimHide;

// Configurable Variables
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue Box|TypeOut")
	bool bTypeOutText = false;

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue Box|TypeOut|Settings", meta = (ClampMin = 0.01, UIMin = 0.01, EditCondition = bTypeOutText, EditConditionHides))
	float FlatTypeOutDelay = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue Box|TypeOut|Settings", meta = (EditCondition = bTypeOutText, EditConditionHides))
	class UCurveFloat* DynamicTypeOutDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialog Box|Animation")
	bool bHideAnimationIsReverseShow = false;

	float CachedLastDynamicDelay;

	float GetTypeOutDelay();

// Events
public:
	UPROPERTY(BlueprintAssignable)
	FOnDialogueReady OnDialogueReady;

	FOnAnimationFinished OnShowAnimationFinished;
	FOnAnimationFinished OnHideAnimationFinished;

// Properties
protected:
	UPROPERTY(BlueprintReadOnly)
	bool bDialogueReady;

// Type Out Text
protected:

	UPROPERTY(BlueprintReadonly)
	struct FDialogueEvent CachedDialogueEvent;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* RichTextTable;

	UPROPERTY()
	class ADSPlayerController* CachedController;

private:
	FTimerHandle TypeOutTextHandle;
	int32 TypeOutTextIdx;
	FString TypedOutString;
	FString InProgressString;

	FString CachedTag;

	void TypeOutChar();
};
