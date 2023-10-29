// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DSDialogueEvent.generated.h"


USTRUCT(BlueprintType)
struct DIALOGUESYSTEM_API FDialogueEvent2
{
	GENERATED_BODY()
};

//UENUM(BlueprintType)
//enum class EDialogueEventType : uint8
//{
//	None,
//	SingleSpeaker,
//	DoubleSpeaker,
//};
//
///**
// * 
// */
//USTRUCT(BlueprintType)
//struct DIALOGUESYSTEM_API FDialogueEvent
//{
//	GENERATED_BODY()
//
//public:
//	UPROPERTY(EditDefaultsOnly)
//	EDialogueEventType Type;
//
//	virtual void Execute();
//
//	void SingleSpeaker();
//	void DoubleSpeaker();
//
//public:
//
//	//************************************
//	// SingleSpeaker
//	//************************************
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::SingleSpeaker", DisplayName = "Speaker"))
//	UDSCharacterAsset* SingleSpeaker_Speaker;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::SingleSpeaker", DisplayName = "Emotion"))
//	EEmotion SingleSpeaker_Emotion;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::SingleSpeaker", DisplayName = "Line"))
//	FText SingleSpeaker_Line;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::SingleSpeaker", DisplayName = "RichTextTable"))
//	class UDataTable* SingleSpeaker_RichTextTable;
//
//	//************************************
//	// DoubleSpeaker
//	//************************************
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::DoubleSpeaker", DisplayName = "Speaker"))
//	UDSCharacterAsset* DoubleSpeaker_Speaker1;														 
//																									 
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::DoubleSpeaker", DisplayName = "Emotion"))
//	EEmotion DoubleSpeaker_S1_Emotion;											
//																									 
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::DoubleSpeaker", DisplayName = "Line"))
//	FText DoubleSpeaker_S1_Line;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::DoubleSpeaker", DisplayName = "RichTextTable"))
//	class UDataTable* DoubleSpeaker_S1_RichTextTable;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::DoubleSpeaker", DisplayName = "Speaker2"))
//	UDSCharacterAsset* DoubleSpeaker_Speaker2;														 
//																									 
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::DoubleSpeaker", DisplayName = "Speaker2_Emotion"))
//	EEmotion DoubleSpeaker_S2_Emotion;											
//																									 
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::DoubleSpeaker", DisplayName = "Speaker2_Line"))
//	FText DoubleSpeaker_S2_Line;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "Type == EDialogueEventType::DoubleSpeaker", DisplayName = "Speaker2_RichTextTable"))
//	class UDataTable* DoubleSpeaker_S2_RichTextTable;
//};
