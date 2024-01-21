// Fill out your copyright notice in the Description page of Project Settings.


#include "UIDialogueScene.h"
#include "Kismet/GameplayStatics.h"
#include "DSPlayerController.h"
#include "DSGameMode.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "UIDialogueBox.h"
#include "DSTypes.h"
#include "UMG/Public/Blueprint/WidgetBlueprintLibrary.h"

void UUIDialogueScene::NativeOnInitialized()
{
	RegisterDialogueBoxes();
}

void UUIDialogueScene::BeginDialogue(UDialogueAsset* Data)
{
	if (!Data) return;
	DialogueData = Data;

	if (ADSPlayerController* DSController = Cast<ADSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		DSController->BeginDialogue();

		CachePlayerControllerInBoxes(DSController);

		DialogueIdx = 0;

		OnBeginDialogue.Broadcast();

		AdvanceDialogue();

	}
	else
	{
		EndDialogue();
	}
}

void UUIDialogueScene::AdvanceDialogue()
{
	TArray<FDialogueEvent> Events = DialogueData->DialogueEvents;

	// If the last event is still in progress, try to fast forward it
	if (IsPreviousEventInProgress())
	{
		TryFinishPreviousEvent();
	}
	// Push next event
	else
	{
		// Conflicts with TryFinishPreviousEvent?
		CleanUpLastEvent();

		// No more events, end dialogue
		if (!Events.IsValidIndex(DialogueIdx))
		{
			EndDialogue();
			return;
		}

		FDialogueEvent& CurrentEvent = Events[DialogueIdx];

		// Execute Dialogue Boxes
		for (const FDialogueLine& Line : CurrentEvent.Lines)
		{
			UUIDialogueBox* DialogueBox = GetDialogueBox(Line.BoxTag);
			if (!DialogueBox) continue;

			ActiveDialogueBoxes.AddUnique(DialogueBox);

			DialogueBox->OnShowAnimationFinished.BindUObject(this, &UUIDialogueScene::ShowAnimationFinished, DialogueBox, Line.DialogueLine);
			if (!ShowDialogueBox(DialogueBox))
			{
				// It was already showing
				ShowAnimationFinished(DialogueBox, Line.DialogueLine);
			}
		}

		// Execute Pause

		// Execute Camera
	}
}

void UUIDialogueScene::EndDialogue()
{
	// Clean up all dialogue boxes before ending conversation
	for (UUIDialogueBox* DialogueBox : DialogueBoxes)
	{
		if (DialogueBox)
		{
			DialogueBox->OnHideAnimationFinished.BindUObject(this, &UUIDialogueScene::FinalizeEndDialogue);
			DialogueBox->Show(false);
		}
	}
}

void UUIDialogueScene::FinalizeEndDialogue()
{
	// Don't finalize until all dialogue boxes have finished their hide animations
	for (UUIDialogueBox* DialogueBox : DialogueBoxes)
	{
		if (DialogueBox->GetIsShowing())
		{
			return;
		}
	}

	// To-do: work out a system where we listen to delegates for all active dialogue boxes
	if (!DialogueBoxes.IsEmpty())
	{
		UUIDialogueBox* DialogueBox = DialogueBoxes[0];
		DialogueBox->OnHideAnimationFinished.Unbind();
	}

	if (ADSPlayerController* DSController = Cast<ADSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		DSController->EndDialogue();
	}

	OnEndDialogue.Broadcast();
}

bool UUIDialogueScene::IsPreviousEventInProgress()
{
	return bInProgress;
}

void UUIDialogueScene::TryFinishPreviousEvent()
{
	if (bInProgress)
	{
		OnFastForwardDel.ExecuteIfBound();
	}
}

void UUIDialogueScene::CleanUpLastEvent()
{
	for (UUIDialogueBox* ActiveDialogueBox : ActiveDialogueBoxes)
	{
		ActiveDialogueBox->FastForward();
		ActiveDialogueBox->StopVoice();
	}
	ActiveDialogueBoxes.Empty();
}

bool UUIDialogueScene::ShowDialogueBox(UUIDialogueBox* DialogueBox, bool bForce)
{
	if (DialogueBox)
	{
		if (bForce || !DialogueBox->GetIsShowing())
		{
			DialogueBox->Show(true);
			return true;
		}
	}
	return false;
}

bool UUIDialogueScene::HideDialogueBox(UUIDialogueBox* DialogueBox, bool bForce)
{
	if (DialogueBox)
	{
		if (bForce || DialogueBox->GetIsShowing())
		{
			DialogueBox->Show(false);
			return true;
		}
	}
	return false;
}

void UUIDialogueScene::OnReadyToAdvance()
{
	bInProgress = false;
	OnFastForwardDel.Unbind();

	if (bAutoAdvance)
	{
		AdvanceDialogue();
	}
}

void UUIDialogueScene::ShowAnimationFinished(UUIDialogueBox* DialogueBox, UDSDialogueLineAsset* CurrentLine)
{
	DialogueBox->OnShowAnimationFinished.Unbind();

	AdvanceDialogueBox(DialogueBox, CurrentLine);

	bInProgress = true;
	//bAutoAdvance = CurrentEvent.bAutoAdvance;
	/*if (CurrentEvent.bCanFastForward)
	{
		OnFastForwardDel.BindUObject(this, &UUIDialogueScene::OnFastForwardDialogue, DialogueBox);
	}
	else
	{
		OnFastForwardDel.Unbind();
	}*/
	OnFastForwardDel.BindUObject(this, &UUIDialogueScene::OnFastForwardDialogue, DialogueBox);
}

void UUIDialogueScene::HideAnimationFinished()
{

}



//void UUIDialogueScene::SingleSpeaker(FDialogueEvent& CurrentEvent)
//{
//	UUIDialogueBox* DialogueBox = nullptr;// GetDialogueBox(CurrentEvent.EBox);
//	if (!DialogueBox) return;
//	
//	CurrentEventDialogueBox = DialogueBox;
//
//	DialogueBox->OnShowAnimationFinished.BindUObject(this, &UUIDialogueScene::ShowAnimationFinished, DialogueBox, CurrentEvent);
//	if (!ShowDialogueBox(DialogueBox))
//	{
//		// It was already showing
//		ShowAnimationFinished(DialogueBox, CurrentEvent);
//	}
//
//	/*AdvanceDialogueBox(DialogueBox, CurrentEvent);
//
//	bInProgress = true;
//	bAutoAdvance = CurrentEvent.bAutoAdvance;
//	if (CurrentEvent.bCanFastForward)
//	{
//		OnFastForwardDel.BindUObject(this, &UUIDialogueScene::OnFastForwardDialogue, DialogueBox);
//	}
//	else
//	{
//		OnFastForwardDel.Unbind();
//	}*/
//}

//void UUIDialogueScene::Pause(FDialogueEvent& CurrentEvent)
//{
//	HideAllDialogueBoxes();
//	DialogueIdx++;
//
//	bInProgress = true;
//	bAutoAdvance = true;
//	GetWorld()->GetTimerManager().SetTimer(PauseTimerHandle, this, &UUIDialogueScene::OnReadyToAdvance, CurrentEvent.PauseTime);
//}

void UUIDialogueScene::AdvanceDialogueBox(UUIDialogueBox* DialogueBox, UDSDialogueLineAsset* CurrentLine)
{
	DialogueBox->PushLine(CurrentLine, false);
	DialogueIdx++;
}

void UUIDialogueScene::OnFastForwardDialogue(UUIDialogueBox* DialogueBox)
{
	DialogueBox->FastForward();
}

bool UUIDialogueScene::HideAllDialogueBoxes(bool bForce)
{
	bool bBoxHidden = false;
	for (auto* DialogueBox : DialogueBoxes)
	{
		bBoxHidden |= HideDialogueBox(DialogueBox, false);
	}
	return bBoxHidden;
}

void UUIDialogueScene::CachePlayerControllerInBoxes(class ADSPlayerController* DSController)
{
	for (UUIDialogueBox* DialogueBox : DialogueBoxes)
	{
		DialogueBox->SetPlayerController(DSController);
	}
}

UUIDialogueBox* UUIDialogueScene::GetDialogueBox(FGameplayTag Tag)
{
	for (UUIDialogueBox* DialogueBox : DialogueBoxes)
	{
		if (Tag.IsValid() && DialogueBox->GetTag().MatchesTag(Tag))
		{
			return DialogueBox;
		}
		else
		{
			ensureAlwaysMsgf(false, TEXT("UUIDialogueScene::GetDialogueBox -- Could not find dialogue box with tag."));
		}
	}
	return nullptr;
}

void UUIDialogueScene::RegisterDialogueBoxes()
{
	DialogueBoxes.Empty();
	GatherDialogueBoxes();

	if (DialogueBoxes.IsEmpty())
	{
		ensureMsgf(false, TEXT("UUIDialogueScene::RegisterDialogueBoxes -- No dialogue boxes registered. Make sure to implement GatherDialogueBoxes() in your blueprint."));
	}

	for (UUIDialogueBox* DialogueBox : DialogueBoxes)
	{
		DialogueBox->OnDialogueReady.AddUniqueDynamic(this, &UUIDialogueScene::OnReadyToAdvance);
	}
}