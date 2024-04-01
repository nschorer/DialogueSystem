// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "DSTypes.generated.h"

UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UDSCharacterAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText Name;

	// Make this soft
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

UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UDSDialogueLineAsset: public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	UDSCharacterAsset* Speaker;

	UPROPERTY(EditDefaultsOnly)
	EEmotion Emotion;

	UPROPERTY(EditDefaultsOnly, Category = "Text")
	FText TextLine;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* VoiceLine;
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
struct DIALOGUESYSTEM_API FDialogueLine
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
		UDSDialogueLineAsset* DialogueLine;

	UPROPERTY(EditDefaultsOnly)
		FGameplayTag BoxTag;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEM_API FDialogueEvent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Lines")
	TArray<FDialogueLine> Lines;

	// If false, user input will be ignored, and the event will play out by itself.
	// It will auto advance to the next event.
	UPROPERTY(EditDefaultsOnly, Category = "User Control")
	bool bAllowUserControl = true;

	// Let the user press a bud mid-sentence to fast forward dialogue.
	// This will bring the event to the end-state, but will not advance to the next event.
	UPROPERTY(EditDefaultsOnly, Category = "User Control")
	bool bCanFastForward = true;

	// After this event ends, wait this amount of time before starting next event.
	UPROPERTY(EditDefaultsOnly)
	float PauseTime = 0.1f;
};

UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UDialogueAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// The tag is used to look up which scene and mapping context to use, configured in the game instance.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag DialogueType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<struct FDialogueEvent> DialogueEvents;
};

/**
 *
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEM_API FDialogueTypeSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUIDialogueScene> DialogueScene;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DialogueOverrides)
	class UInputMappingContext* MappingContext;

	// If false, user input will be ignored, and the dialogue will play out by itself.
	// It will auto advance from event to event.
	// 
	// Interaction with FDialogueEvent.bAllowUserControl:
	//    If this == true,  then FDialogueEvent.bAllowUserControl can be set to false to override per event.
	//    If this == false, then FDialogueEvent.bAllowUserControl can NOT override.
	UPROPERTY(EditDefaultsOnly)
	bool bAllowManualControl = true;
};

/**
 *
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UStandardDialogueSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FDialogueTypeSettings> DialogueTypes;
};