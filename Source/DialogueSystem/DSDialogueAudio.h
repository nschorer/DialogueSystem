// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DSDialogueAudio.generated.h"

class USoundBase;

UCLASS(Blueprintable, BlueprintType)
class DIALOGUESYSTEM_API ADSDialogueAudio : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADSDialogueAudio();

	UPROPERTY()
	class UAudioComponent* AudioComponent1;

	UPROPERTY()
	class UAudioComponent* AudioComponent2;

	UPROPERTY()
	class UAudioComponent* AudioComponent3;

	UPROPERTY()
	class UAudioComponent* AudioComponent4;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;


public:

	bool PlayVoiceLine(USoundBase* VoiceLine);
	bool StopVoiceLine(USoundBase* VoiceLine);

protected:

	void RegisterAudioComponents();

	bool IsVoiceLineAlreadyBeingPlayed(USoundBase* VoiceLine);

	UAudioComponent* FindAvailableAudioComponent();

	UFUNCTION()
	void OnAudioFinished(UAudioComponent* AudioComponent);

	void RegisterWithGameMode();

protected:
	TArray<UAudioComponent*> AudioComponents;
};
