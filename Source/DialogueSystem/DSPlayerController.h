// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "DSPlayerController.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitiateDialogue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInitiateDialogue, class UDialogueAsset*, DialogueData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAdvanceDialogue);

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API ADSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	// APawn interface
	virtual void SetupInputComponent() override;

	// To add mapping context
	virtual void BeginPlay();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* DialogueMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputAction* AdvanceDialogueAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputAction* LookAction;

	UFUNCTION()
	void AdvanceDialogue();

	void Look(const FInputActionValue& Value);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	void BeginDialogue() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	void EndDialogue() const;

	UPROPERTY(BlueprintAssignable)
	FOnInitiateDialogue OnInitiateDialogue;

	UPROPERTY(BlueprintAssignable)
	FOnAdvanceDialogue OnAdvanceDialogue;
};
