// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DSTypes.h"
#include "DSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API UDSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	UStandardDialogueSettings* GeneralDialogueSettings;
};
