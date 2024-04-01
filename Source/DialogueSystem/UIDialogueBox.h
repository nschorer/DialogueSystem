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
	void PushLine(const UDSDialogueLineAsset* Line, bool bIsLastEvent);

	UFUNCTION(BlueprintCallable)
	void FastForward();

	UFUNCTION(BlueprintCallable)
	void StopVoice();

	UFUNCTION(BlueprintCallable)
	void PlayContinueOrEndNoise(bool bIsEnd);

	UFUNCTION(BlueprintCallable)
	bool IsDialogueReady() const {return bDialogueReady;}

	UFUNCTION()
	bool GetIsShowing() const {return bIsShowing;}

	UFUNCTION()
	bool GetAnimationInProgress() const {return bAnimationInProgress;}

	//UFUNCTION()
	void Show(bool bShow, const FOnAnimationFinished& OnAnimationFinished);

	UFUNCTION()
	void OnShowFinished();

	UFUNCTION()
	void OnReverseShowFinished();

	UFUNCTION()
	void OnHideFinished();

	// Sloppy
	void SetPlayerController(class ADSPlayerController* DSController) { CachedController = DSController; }

	UFUNCTION()
	FGameplayTag GetTag() const {return Tag;}

protected:
	void UpdateSpeaker(const UDSDialogueLineAsset* NewDialogueEvent);
	void UpdateTextLine(const UDSDialogueLineAsset* NewDialogueEvent);
	void UpdateVoiceLine(const UDSDialogueLineAsset* NewDialogueEvent);
	void UpdateEmotion(const UDSDialogueLineAsset* NewDialogueEvent);

	void CancelInProgressEvents();

	UFUNCTION(BlueprintCallable)
	void SetReadyForNextLine();

// Child Widgets
protected:

	UPROPERTY(EditAnywhere, Category = Dialogue)
	FGameplayTag Tag;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Box|TypeOut")
	bool bTypeOutText = false;

	UPROPERTY(EditAnywhere, Category = "Dialogue Box|TypeOut|Settings", meta = (ClampMin = 0.01, UIMin = 0.01, EditCondition = bTypeOutText, EditConditionHides))
	float FlatTypeOutDelay = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Dialogue Box|TypeOut|Settings", meta = (EditCondition = bTypeOutText, EditConditionHides))
	class UCurveFloat* DynamicTypeOutDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialog Box|Animation")
	bool bHideAnimationIsReverseShow = false;

	float CachedLastDynamicDelay;

	float GetTypeOutDelay();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Box|Continue Or End")
	class USoundBase* ContinueNoise;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Box|Continue Or End")
	class USoundBase* EndNoise;

// Events
public:
	UPROPERTY(BlueprintAssignable)
	FOnDialogueReady OnDialogueReady;

protected:
	FOnAnimationFinished OnShowHideAnimFinished;
	bool bAnimationInProgress;

// Properties
protected:
	UPROPERTY(BlueprintReadOnly)
	bool bDialogueReady;

// Type Out Text
protected:

	UPROPERTY(BlueprintReadonly)
	const class UDSDialogueLineAsset* CachedLine;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* RichTextTable;

	UPROPERTY()
	class ADSPlayerController* CachedController;

	UPROPERTY()
	bool bIsShowing;

private:
	FTimerHandle TypeOutTextHandle;
	int32 TypeOutTextIdx;
	FString TypedOutString;
	FString InProgressString;

	FString CachedTag;

	void TypeOutChar();
};
