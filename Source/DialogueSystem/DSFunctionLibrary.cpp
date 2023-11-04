// Fill out your copyright notice in the Description page of Project Settings.


#include "DSFunctionLibrary.h"
#include "DSGameMode.h"
#include "DSDialogueAudio.h"

class ADSDialogueAudio* UDSFunctionLibrary::GetDialogueAudio(UObject* WorldContext)
{
	if (UWorld* World = WorldContext->GetWorld())
	{
		if (ADSGameMode* DSGameMode = World->GetAuthGameMode<ADSGameMode>())
		{
			return DSGameMode->DialogueAudioManager;
		}
	}
	return nullptr;
}

