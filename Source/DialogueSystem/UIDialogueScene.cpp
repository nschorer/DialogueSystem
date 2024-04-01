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

		if (bAdvancing)
		{
			return;
		}
		bAdvancing = true;

		// No more events, end dialogue
		if (!Events.IsValidIndex(DialogueIdx))
		{
			EndDialogue();
			return;
		}

		FDialogueEvent& CurrentEvent = Events[DialogueIdx];
		bool bIsLast = DialogueIdx >= Events.Num() - 1;

		// Execute Dialogue Boxes
		for (const FDialogueLine& Line : CurrentEvent.Lines)
		{
			UUIDialogueBox* DialogueBox = GetDialogueBox(Line.BoxTag);
			if (!DialogueBox) continue;

			ActiveDialogueBoxes.AddUnique(DialogueBox);

			FOnAnimationFinished Callback = FOnAnimationFinished::CreateUObject(this, &UUIDialogueScene::ShowAnimationFinished, DialogueBox, Line.DialogueLine, bIsLast);
			if (!ShowDialogueBox(DialogueBox, Callback))
			{
				// It was already showing
				ShowAnimationFinished(DialogueBox, Line.DialogueLine, bIsLast);
			}
		}

		// Execute Pause

		// Execute Camera
	}
}

void UUIDialogueScene::EndDialogue()
{
	// Clean up all dialogue boxes before ending conversation
	FOnAnimationFinished OnHideFinished = FOnAnimationFinished::CreateUObject(this, &UUIDialogueScene::FinalizeEndDialogue);
	HideAllDialogueBoxes(OnHideFinished);
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

	//// To-do: work out a system where we listen to delegates for all active dialogue boxes
	//if (!DialogueBoxes.IsEmpty())
	//{
	//	UUIDialogueBox* DialogueBox = DialogueBoxes[0];
	//	DialogueBox->OnHideAnimationFinished.Unbind();
	//}

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
		ActiveDialogueBox->PlayContinueOrEndNoise(DialogueIdx >= DialogueData->DialogueEvents.Num() - 1);
	}
	ActiveDialogueBoxes.Empty();
}

bool UUIDialogueScene::ShowDialogueBox(UUIDialogueBox* DialogueBox, const FOnAnimationFinished& OnShowFinished)
{
	if (DialogueBox)
	{
		if (!DialogueBox->GetIsShowing())
		{
			DialogueBox->Show(true, OnShowFinished);
			return true;
		}
	}
	return false;
}

bool UUIDialogueScene::HideDialogueBox(UUIDialogueBox* DialogueBox, const FOnAnimationFinished& OnHideFinished)
{
	if (DialogueBox)
	{
		if (DialogueBox->GetIsShowing())
		{
			DialogueBox->Show(false, OnHideFinished);
			return true;
		}
	}
	return false;
}

void UUIDialogueScene::OnReadyToAdvance()
{
	// Wait for all dialogue boxes to be ready
	for (const UUIDialogueBox* ActiveBox : ActiveDialogueBoxes)
	{
		if (!ActiveBox->IsDialogueReady())
		{
			return;
		}
	}

	bInProgress = false;
	OnFastForwardDel.Unbind();

	if (bAutoAdvance)
	{
		AdvanceDialogue();
	}
}

void UUIDialogueScene::ShowAnimationFinished(UUIDialogueBox* DialogueBox, UDSDialogueLineAsset* CurrentLine, bool bIsLast)
{
	bool bAllBoxAnimationsDone = true;
	for (const UUIDialogueBox* ActiveBox : ActiveDialogueBoxes)
	{
		if (ActiveBox->GetAnimationInProgress())
		{
			bAllBoxAnimationsDone = false;
			break;
		}
	}

	if (bAllBoxAnimationsDone)
	{
		bAdvancing = false;
	}

	//DialogueBox->OnShowAnimationFinished.Unbind();

	bInProgress = true;
	OnFastForwardDel.BindUObject(this, &UUIDialogueScene::OnFastForwardDialogue, DialogueBox);

	AdvanceDialogueBox(DialogueBox, CurrentLine, bIsLast);

	//bAutoAdvance = CurrentEvent.bAutoAdvance;
	/*if (CurrentEvent.bCanFastForward)
	{
		OnFastForwardDel.BindUObject(this, &UUIDialogueScene::OnFastForwardDialogue, DialogueBox);
	}
	else
	{
		OnFastForwardDel.Unbind();
	}*/
	
}

void UUIDialogueScene::HideAnimationFinished()
{

}

void UUIDialogueScene::AdvanceDialogueBox(UUIDialogueBox* DialogueBox, UDSDialogueLineAsset* CurrentLine, bool bIsLast)
{
	DialogueBox->PushLine(CurrentLine, bIsLast);
	DialogueIdx++;
}

void UUIDialogueScene::OnFastForwardDialogue(UUIDialogueBox* DialogueBox)
{
	DialogueBox->FastForward();
}

bool UUIDialogueScene::HideAllDialogueBoxes(const FOnAnimationFinished& OnHideFinished)
{
	bool bBoxHidden = false;
	for (auto* DialogueBox : DialogueBoxes)
	{
		bBoxHidden |= HideDialogueBox(DialogueBox, OnHideFinished);
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