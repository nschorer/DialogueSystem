// Fill out your copyright notice in the Description page of Project Settings.


#include "DSDialogueAudio.h"
#include "Components/AudioComponent.h"
#include "DSGameMode.h"

// Sets default values
ADSDialogueAudio::ADSDialogueAudio()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	AudioComponent1 = CreateDefaultSubobject <UAudioComponent>(TEXT("AudioComponent1"));
	AudioComponent1->SetupAttachment(RootComponent);

	AudioComponent2 = CreateDefaultSubobject <UAudioComponent>(TEXT("AudioComponent2"));
	AudioComponent2->SetupAttachment(RootComponent);

	AudioComponent3 = CreateDefaultSubobject <UAudioComponent>(TEXT("AudioComponent3"));
	AudioComponent3->SetupAttachment(RootComponent);

	AudioComponent4 = CreateDefaultSubobject <UAudioComponent>(TEXT("AudioComponent4"));
	AudioComponent4->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADSDialogueAudio::BeginPlay()
{
	Super::BeginPlay();	

	RegisterWithGameMode();

	RegisterAudioComponents();
}

bool ADSDialogueAudio::PlayVoiceLine(class USoundBase* VoiceLine)
{
	if (IsVoiceLineAlreadyBeingPlayed(VoiceLine))
	{
		return false;
	}

	UAudioComponent* AudioComponent = FindAvailableAudioComponent();
	if (!ensureMsgf(AudioComponent, TEXT("There were no available audio components, so voice line wasn't played!"))) return false;

	AudioComponent->SetSound(VoiceLine);
	AudioComponent->Play();

	AudioComponent->OnAudioFinishedNative.AddUObject(this, &ADSDialogueAudio::OnAudioFinished, VoiceLine);

	return true;
}

bool ADSDialogueAudio::StopVoiceLine(USoundBase* VoiceLine)
{
	for (UAudioComponent* AudioComponent : AudioComponents)
	{
		if (AudioComponent->IsPlaying())
		{
			if (AudioComponent->GetSound() == VoiceLine)
			{
				AudioComponent->Stop();
				return true;
			}
		}
	}
	return false;
}

void ADSDialogueAudio::RegisterAudioComponents()
{
	AudioComponents.Add(AudioComponent1);
	AudioComponents.Add(AudioComponent2);
	AudioComponents.Add(AudioComponent3);
	AudioComponents.Add(AudioComponent4);
}

bool ADSDialogueAudio::IsVoiceLineAlreadyBeingPlayed(class USoundBase* VoiceLine)
{
	for (UAudioComponent* AudioComponent : AudioComponents)
	{
		if (AudioComponent->IsPlaying())
		{
			if (AudioComponent->GetSound() == VoiceLine)
			{
				return true;
			}
		}
	}
	return false;
}

UAudioComponent* ADSDialogueAudio::FindAvailableAudioComponent()
{
	for (UAudioComponent* AudioComponent : AudioComponents)
	{
		if (!AudioComponent->IsPlaying())
		{
			return AudioComponent;
		}
	}
	return nullptr;
}

void ADSDialogueAudio::OnAudioFinished(UAudioComponent* FinishedAudio, USoundBase* VoiceLine)
{
	OnDialogueAudioFinished.ExecuteIfBound(VoiceLine);
}

void ADSDialogueAudio::RegisterWithGameMode()
{
	if (ADSGameMode* DSGameMode = GetWorld()->GetAuthGameMode<ADSGameMode>())
	{
		DSGameMode->RegisterDialogueAudioManager(this);
	}
}

