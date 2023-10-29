// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DSCharacter.h"
#include "DSCharacter_Main.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API ADSCharacter_Main : public ADSCharacter
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Input

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

public:
	ADSCharacter_Main();

	//virtual void PostInitializeComponents();

	virtual void Tick(float DeltaSeconds);

protected:

	/** Called for movement input */
	void InputMove(const FInputActionValue& Value);

	/** Called for looking input */
	void InputLook(const FInputActionValue& Value);

	/** Called for interact input */
	void InputInteract();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	void AddInteractable(TScriptInterface<IDSInteract> Interactable);
	void RemoveInteractable(TScriptInterface<IDSInteract> Interactable);

	bool GetCanInteract() const { return bCanInteract; }

protected:

	UPROPERTY()
	TArray<TScriptInterface<IDSInteract>> Interactables;

	UPROPERTY()
	TScriptInterface<IDSInteract> BestInteractable;

	void ProcessInteractables();

	bool bCanInteract;
	TScriptInterface<IDSInteract> CurrentInteractable;

public:
	// Dialogue

	void BeginDialogue();

	void EndDialogue();
};
