// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DSInteract.h"
#include "DSInteractActor.generated.h"

UCLASS()
class DIALOGUESYSTEM_API ADSInteractActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADSInteractActor();

	virtual void PostInitializeComponents();

	virtual void BeginDestroy() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	class USphereComponent* InteractAwareSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	class UWidgetComponent* InteractWidget;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnInteractBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnInteractEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:

	UPROPERTY()
	TScriptInterface<IDSInteract> OwningInteractor;

	UPROPERTY()
	class ADSCharacter_Main* InteractingCharacter;

	bool bCanInteract;
};
