// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DSGameMode.generated.h"

UCLASS(minimalapi)
class ADSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADSGameMode();

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	UInputMappingContext* GetDefaultMappingContext() const {return DefaultMappingContext;}

	UFUNCTION(BlueprintCallable)
	TSubclassOf<UUIDialogueBox> GetDefaultDialogueBox() const {return DefaultDialogueBox;}

	UFUNCTION(BlueprintCallable)
	UDataTable* GetDefaultRichTextTable() const {return DefaultRichTextTable;}

public:
	void RegisterDialogueAudioManager(class ADSDialogueAudio* InDialogueAudioManager);

protected:

	FTimerHandle DialogueManagerHandle;

	UFUNCTION()
	void SetupDialogueManager();

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, Category = Dialogue)
	TSubclassOf<class UDSDialogueManager> DialogueManagerClass;

	UPROPERTY(EditDefaultsOnly, Category = Dialogue)
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = Dialogue)
	class TSubclassOf<UUIDialogueBox> DefaultDialogueBox;

	UPROPERTY(EditDefaultsOnly, Category = Dialogue)
	class UDataTable* DefaultRichTextTable;

public:
	UPROPERTY()
	class UDSDialogueManager* DialogueManager;

	UPROPERTY()
	class ADSDialogueAudio* DialogueAudioManager;
};



