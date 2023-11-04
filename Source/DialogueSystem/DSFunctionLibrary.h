// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DSFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API UDSFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta=(WorldContextObject = "WorldContext"))
	static class ADSDialogueAudio* GetDialogueAudio(UObject* WorldContext);
};
