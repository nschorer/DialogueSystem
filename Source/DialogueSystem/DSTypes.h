// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "DSTypes.generated.h"

UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UDSCharacterAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Portrait;

	UPROPERTY(EditDefaultsOnly)
	UDialogueAsset* DialogueData;
};

UENUM(BlueprintType)
enum class EEmotion : uint8 {
	Happy,
	Afraid,
	Sad,
	Angry,
	Smug,
	Disgusted,
	Confused,
	Surprised,
	Embarrassed
};

USTRUCT(BlueprintType)
struct DIALOGUESYSTEM_API FDialogueLine
{
	GENERATED_BODY()

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UDSCharacterAsset* Speaker;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EEmotion Emotion;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FText Line;

		UPROPERTY(EditDefaultsOnly)
		class UDataTable* RichTextTable;
};

UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UTextStyle : public UObject
{
	GENERATED_BODY()

protected:
		UPROPERTY(EditDefaultsOnly)
		class UDataTable* RichTextTable;		
};

UENUM(BlueprintType)
enum class EDialogueEventType : uint8
{
	None,
	SingleSpeaker,
	Pause
};

UENUM(BlueprintType)
enum class EDialogueBox : uint8
{
	Box1,
	Box2,
	Box3,
	Box4,
	Box5,
	Box6,
	Box7,
	Box8
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEM_API FDialogueEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	EDialogueEventType Type;

public:

	UPROPERTY(EditDefaultsOnly, meta=(EditConditionHides, 
		EditCondition =	"Type == EDialogueEventType::SingleSpeaker"))
	UDSCharacterAsset* Speaker;

	UPROPERTY(EditDefaultsOnly, meta =(EditConditionHides,
		EditCondition = "Type == EDialogueEventType::SingleSpeaker"))
	EDialogueBox EBox;

	UPROPERTY(EditDefaultsOnly, meta=(EditConditionHides, 
		EditCondition = "Type == EDialogueEventType::SingleSpeaker"))
	EEmotion Emotion;

	UPROPERTY(EditDefaultsOnly, meta=(EditConditionHides, 
		EditCondition = "Type == EDialogueEventType::SingleSpeaker"))
	FText TextLine;

	UPROPERTY(EditDefaultsOnly, meta = (EditConditionHides,
		EditCondition = "Type == EDialogueEventType::SingleSpeaker"))
	USoundBase* VoiceLine;

	UPROPERTY(EditDefaultsOnly, meta=(EditConditionHides, 
		EditCondition = "Type == EDialogueEventType::SingleSpeaker"))
	bool bCanFastForward = true;

	UPROPERTY(EditDefaultsOnly, meta = (EditConditionHides,
		EditCondition = "Type == EDialogueEventType::Pause"))
	float PauseTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, meta = (EditConditionHides,
		EditCondition = "Type == EDialogueEventType::SingleSpeaker"))
	bool bAutoAdvance = false;

	// Overrides

	UPROPERTY(EditDefaultsOnly)
	bool bShowOverrides;


	UPROPERTY(EditDefaultsOnly, Category = "Override", meta = (EditConditionHides,
		EditCondition = "(Type == EDialogueEventType::SingleSpeaker || Type == EDIalogueEventType::DoubleSpeaker) && bShowOverrides"))
	class UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Override", meta = (EditConditionHides,
		EditCondition = "(Type == EDialogueEventType::SingleSpeaker || Type == EDIalogueEventType::DoubleSpeaker) && bShowOverrides"))
	class UUIDialogueBox* DialogueBox;

	UPROPERTY(EditDefaultsOnly, Category = "Override", meta = (EditConditionHides,
		EditCondition = "(Type == EDialogueEventType::SingleSpeaker || Type == EDIalogueEventType::DoubleSpeaker) && bShowOverrides"))
	class UDataTable* RichTextTable;

	UPROPERTY(EditDefaultsOnly, meta=(EditConditionHides, 
		EditCondition = "Type == EDialogueEventType::DoubleSpeaker && bShowOverrides"))
	class UInputMappingContext* S2_MappingContext;

	UPROPERTY(EditDefaultsOnly, meta=(EditConditionHides, 
		EditCondition = "Type == EDialogueEventType::DoubleSpeaker && bShowOverrides"))
	class UUIDialogueBox* S2_DialogueBox;

	UPROPERTY(EditDefaultsOnly, meta=(EditConditionHides, 
		EditCondition = "Type == EDialogueEventType::DoubleSpeaker && bShowOverrides"))
	class UDataTable* S2_RichTextTable;
};

UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UDialogueAsset : public UDataAsset
{
	GENERATED_BODY()

public:
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DialogueOverrides)
		class UInputMappingContext* OverrideMappingContext;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DialogueOverrides)
		class UUIDialogueBox* OverrideDialogueBox;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DialogueOverrides)
		class UDataTable* OverrideRichTextTable;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Events)
		TArray<struct FDialogueEvent> DialogueEvents;
};