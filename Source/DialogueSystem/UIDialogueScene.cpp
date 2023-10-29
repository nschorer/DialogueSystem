// Fill out your copyright notice in the Description page of Project Settings.


#include "UIDialogueScene.h"
#include "Kismet/GameplayStatics.h"
#include "DSPlayerController.h"
#include "DSGameMode.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "UIDialogueBox.h"
#include "DSTypes.h"

void UUIDialogueScene::NativeOnInitialized()
{
	DialogueBox1->OnDialogueReady.AddUniqueDynamic(this, &UUIDialogueScene::OnReadyToAdvance);
	RegisterDialogueBoxes();
}

void UUIDialogueScene::BeginDialogue(UDialogueAsset* Data)
{
	if (!Data) return;
	DialogueData = Data;

	if (ADSPlayerController* DSController = Cast<ADSPlayerController>(UGameplayStatics::GetPlayerController(this,0)))
	{
		DSController->BeginDialogue();
	}

	DialogueIdx = 0;

	OnBeginDialogue.Broadcast();

	AdvanceDialogue();
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
		// No more events, end dialogue
		if (!Events.IsValidIndex(DialogueIdx))
		{
			EndDialogue();
			return;
		}

		FDialogueEvent& CurrentEvent = Events[DialogueIdx];

		switch (CurrentEvent.Type)
		{
		case EDialogueEventType::None:
			break;
		case EDialogueEventType::SingleSpeaker:
			SingleSpeaker(CurrentEvent);
			break;
		case EDialogueEventType::Pause:
			Pause(CurrentEvent);
			break;
		default:
			break;

		}
	}
}

void UUIDialogueScene::EndDialogue()
{
	// To-do: work out a system where we listen to delegates for all active dialogue boxes
	UUIDialogueBox* DialogueBox = nullptr;
	if (!DialogueBoxes.IsEmpty())
	{
		DialogueBox = DialogueBoxes[0];
	}
	if (DialogueBox)
	{
		DialogueBox->OnHideAnimationFinished.BindUObject(this, &UUIDialogueScene::FinalizeEndDialogue);
		if (!HideAllDialogueBoxes())
		{
			FinalizeEndDialogue();
		}
	}
	else
	{
		FinalizeEndDialogue();
	}
}

void UUIDialogueScene::FinalizeEndDialogue()
{
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

bool UUIDialogueScene::ShowDialogueBox(UUIDialogueBox* DialogueBox, bool bForce)
{
	if (DialogueBox)
	{
		if (bForce || !DialogueBox->IsVisible())
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
		if (bForce || DialogueBox->IsVisible())
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

void UUIDialogueScene::ShowAnimationFinished(UUIDialogueBox* DialogueBox, FDialogueEvent CurrentEvent)
{
	DialogueBox->OnShowAnimationFinished.Unbind();

	AdvanceDialogueBox(DialogueBox, CurrentEvent);

	bInProgress = true;
	bAutoAdvance = CurrentEvent.bAutoAdvance;
	if (CurrentEvent.bCanFastForward)
	{
		OnFastForwardDel.BindUObject(this, &UUIDialogueScene::OnFastForwardDialogue, DialogueBox);
	}
	else
	{
		OnFastForwardDel.Unbind();
	}
}

void UUIDialogueScene::HideAnimationFinished()
{

}



void UUIDialogueScene::SingleSpeaker(FDialogueEvent& CurrentEvent)
{
	UUIDialogueBox* DialogueBox = GetDialogueBox(CurrentEvent.EBox);
	if (!DialogueBox) return;
	

	DialogueBox->OnShowAnimationFinished.BindUObject(this, &UUIDialogueScene::ShowAnimationFinished, DialogueBox, CurrentEvent);
	if (!ShowDialogueBox(DialogueBox))
	{
		// It was already showing
		ShowAnimationFinished(DialogueBox, CurrentEvent);
	}

	/*AdvanceDialogueBox(DialogueBox, CurrentEvent);

	bInProgress = true;
	bAutoAdvance = CurrentEvent.bAutoAdvance;
	if (CurrentEvent.bCanFastForward)
	{
		OnFastForwardDel.BindUObject(this, &UUIDialogueScene::OnFastForwardDialogue, DialogueBox);
	}
	else
	{
		OnFastForwardDel.Unbind();
	}*/
}

void UUIDialogueScene::Pause(FDialogueEvent& CurrentEvent)
{
	HideAllDialogueBoxes();
	DialogueIdx++;

	bInProgress = true;
	bAutoAdvance = true;
	GetWorld()->GetTimerManager().SetTimer(PauseTimerHandle, this, &UUIDialogueScene::OnReadyToAdvance, CurrentEvent.PauseTime);
}

void UUIDialogueScene::AdvanceDialogueBox(UUIDialogueBox* DialogueBox, FDialogueEvent& CurrentEvent)
{
	/*if (DialogueBox->IsDialogueReady())
	{
		DialogueBox->PushNextDialogueEvent(CurrentEvent, false);
		DialogueIdx++;
	}
	else
	{
		DialogueBox->FastForward();
	}*/

	DialogueBox->PushNextDialogueEvent(CurrentEvent, false);
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

UUIDialogueBox* UUIDialogueScene::GetDialogueBox(EDialogueBox EBox)
{
	switch (EBox)
	{
	case EDialogueBox::Box1:
		return DialogueBox1;
	case EDialogueBox::Box2:
		return DialogueBox2;
	case EDialogueBox::Box3:
		return DialogueBox3;
	case EDialogueBox::Box4:
		return DialogueBox4;
	case EDialogueBox::Box5:
		return DialogueBox5;
	case EDialogueBox::Box6:
		return DialogueBox6;
	case EDialogueBox::Box7:
		return DialogueBox7;
	case EDialogueBox::Box8:
		return DialogueBox8;
	default:
		return nullptr;
	}
}

void UUIDialogueScene::RegisterDialogueBoxes()
{
	if (DialogueBox1)
	{
		DialogueBoxes.Add(DialogueBox1);
	}

	if (DialogueBox2)
	{
		DialogueBoxes.Add(DialogueBox2);
	}

	if (DialogueBox3)
	{
		DialogueBoxes.Add(DialogueBox3);
	}

	if (DialogueBox4)
	{
		DialogueBoxes.Add(DialogueBox4);
	}

	if (DialogueBox5)
	{
		DialogueBoxes.Add(DialogueBox5);
	}

	if (DialogueBox6)
	{
		DialogueBoxes.Add(DialogueBox6);
	}

	if (DialogueBox7)
	{
		DialogueBoxes.Add(DialogueBox7);
	}

	if (DialogueBox8)
	{
		DialogueBoxes.Add(DialogueBox8);
	}
}
