// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DSInteract.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDSInteract : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DIALOGUESYSTEM_API IDSInteract
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	void Interact();

	UFUNCTION()
	virtual void InteractNative() = 0;

	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	bool CanInteract();

	UFUNCTION()
	virtual bool CanInteractNative() = 0;

	UFUNCTION()
	virtual void TryInteract() {if(CanInteractNative()) InteractNative(); }

	UFUNCTION()
	virtual void SetFocus(bool bIsFocused) = 0;

	UFUNCTION()
	virtual void SetInteractPossible(bool bIsPossible) = 0;

	UFUNCTION()
	virtual void StopInteracting() = 0;
};
