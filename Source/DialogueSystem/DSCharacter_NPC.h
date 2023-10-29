// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DSCharacter.h"
#include "DSInteract.h"
#include "DSCharacter_NPC.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHasInteractFocus, bool, bHasFocus);

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API ADSCharacter_NPC : public ADSCharacter, public IDSInteract
{
	GENERATED_BODY()

public:
	ADSCharacter_NPC();

	virtual void PostInitializeComponents() override;

	virtual void BeginDestroy() override;


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
	class USphereComponent* InteractAwareSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
	class UWidgetComponent* InteractWidget;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	// Interact
public:
	void InteractNative() override;
	bool CanInteractNative() override;
	void SetFocus(bool bIsFocused) override;
	void SetInteractPossible(bool bIsPossible) override;
	void StopInteracting() override;

	void AddInteractable(class ADSCharacter_Main* MainCharacter);
	void RemoveInteractable(class ADSCharacter_Main* MainCharacter);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayInteractAvailableAnimation(bool bAvailable);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayInteractFocusAnimation(bool bHasFocus);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayInteractAnimation();

	UPROPERTY(BlueprintAssignable)
	FOnHasInteractFocus OnHasInteractFocus;

protected:
	bool CachedHasInteractFocus;
	bool CachedIsInteractPossible;
	bool CachedInteracting;

protected:

	UPROPERTY()
	class ADSCharacter_Main* CachedInteractor;

	UFUNCTION()
	void OnInteractBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateInteractWidget();
};
