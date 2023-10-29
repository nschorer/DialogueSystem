// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "DSInteract.h"
#include "DSCharacter.generated.h"


UCLASS(config=Game)
class ADSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADSCharacter();	

protected:
	virtual void BeginPlay();

	// Character

	void UpdateCharacterData();

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Data)
	class UDSCharacterAsset* CharacterData;
};

