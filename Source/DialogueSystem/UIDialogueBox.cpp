// Fill out your copyright notice in the Description page of Project Settings.


#include "UIDialogueBox.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "DSDialogueManager.h"
#include "DSFunctionLibrary.h"
#include "UMG/Public/Components/WidgetSwitcher.h"
#include "UMG/Public/Animation/WidgetAnimation.h"
#include "UMG/Public/Animation/WidgetAnimationEvents.h"
#include "DSDialogueAudio.h"
#include "DSPlayerController.h"

void UUIDialogueBox::NativeConstruct()
{
	Super::NativeConstruct();

	bDialogueReady = true;
}

void UUIDialogueBox::SetReadyForNextLine()
{
	GetWorld()->GetTimerManager().ClearTimer(TypeOutTextHandle);
	bDialogueReady = true;
	OnDialogueReady.Broadcast();
	ContinueOrEnd->SetVisibility(ESlateVisibility::HitTestInvisible);
}

float UUIDialogueBox::GetTypeOutDelay()
{
	if (DynamicTypeOutDelay)
	{
		float CurveIdx = TypeOutTextIdx / 10.0f;
		float DynamicDelay = DynamicTypeOutDelay->GetFloatValue(CurveIdx);
		if (DynamicDelay > 0.0f)
		{
			CachedLastDynamicDelay = DynamicDelay;
			return DynamicDelay;
		}
		else
		{
			// If we don't have a valid value, but we DID before, use the last valid value
			if (CachedLastDynamicDelay)
			{
				return CachedLastDynamicDelay;
			}
		}
	}

	if (FlatTypeOutDelay > 0.0f)
	{
		return FlatTypeOutDelay;
	}

	return 0.001f;
}

void UUIDialogueBox::PushLine(const UDSDialogueLineAsset* Line, bool bIsLastEvent)
{
	ContinueOrEnd->SetVisibility(ESlateVisibility::Collapsed);
	ContinueOrEnd->SetActiveWidgetIndex(bIsLastEvent ? 1 : 0);

	CancelInProgressEvents(/*CachedDialogueEvent*/);

	if (!Line)
	{
		ensureMsgf(false, TEXT("UUIDialogueBox::PushLine -- Invalid line."));
	}

	CachedLine = Line;

	UpdateSpeaker(Line);
	UpdateTextLine(Line);
	UpdateVoiceLine(Line);
	UpdateEmotion(Line);
}

void UUIDialogueBox::FastForward()
{
	if (!bAnimationInProgress)
	{
		FString DisplayStr = TypedOutString;
		CachedTag.Empty();
		DisplayStr.AppendChar(TEXT(' ')); // Adding a space at the end prevents weird UMG string wrapping early
		DialogueBodyText->SetText(FText::FromString(DisplayStr));
		SetReadyForNextLine();
	}
}

void UUIDialogueBox::StopVoice()
{
	if (CachedLine && CachedLine->VoiceLine)
	{
		if (ADSDialogueAudio* DialogueAudio = UDSFunctionLibrary::GetDialogueAudio(CachedController)) // probably bad
		{
			DialogueAudio->StopVoiceLine(CachedLine->VoiceLine);
		}
	}
}

void UUIDialogueBox::PlayContinueOrEndNoise(bool bIsEnd)
{
	if (ADSDialogueAudio* DialogueAudio = UDSFunctionLibrary::GetDialogueAudio(CachedController)) // probably bad
	{
		USoundBase* SoundToPlay = bIsEnd ? EndNoise : ContinueNoise;
		DialogueAudio->StopVoiceLine(SoundToPlay);
		DialogueAudio->PlayVoiceLine(SoundToPlay);
	}
}

void UUIDialogueBox::Show(bool bShow, const FOnAnimationFinished& OnAnimationFinished)
{
	bAnimationInProgress = true;
	OnShowHideAnimFinished = OnAnimationFinished;
	if (bShow)
	{
		bIsShowing = true;

		// Make this into a function
		DialogueSpeakerText->SetVisibility(ESlateVisibility::Collapsed);
		DialogueBodyText->SetVisibility(ESlateVisibility::Collapsed);
		SpeakerImage->SetVisibility(ESlateVisibility::Collapsed);
		ContinueOrEnd->SetVisibility(ESlateVisibility::Collapsed);

		SetVisibility(ESlateVisibility::Visible);
		if (AnimShow)
		{
			PlayAnimation(AnimShow, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
			FWidgetAnimationDynamicEvent OnAnimFinished;
			OnAnimFinished.BindDynamic(this, &UUIDialogueBox::OnShowFinished);
			BindToAnimationFinished(AnimShow, OnAnimFinished);
		}
	}
	else
	{
		bool bPlayedAnimation = false;
		if (bHideAnimationIsReverseShow && AnimShow)
		{
			PlayAnimation(AnimShow, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f, false);
			FWidgetAnimationDynamicEvent OnAnimFinished;
			OnAnimFinished.BindDynamic(this, &UUIDialogueBox::OnReverseShowFinished);
			BindToAnimationFinished(AnimShow, OnAnimFinished);

			bPlayedAnimation = true;
		}
		else if (AnimHide)
		{
			PlayAnimation(AnimHide, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
			FWidgetAnimationDynamicEvent OnAnimFinished;
			OnAnimFinished.BindDynamic(this, &UUIDialogueBox::OnHideFinished);
			BindToAnimationFinished(AnimHide, OnAnimFinished);

			bPlayedAnimation = true;
		}

		if (!bPlayedAnimation)
		{
			SetVisibility(ESlateVisibility::Collapsed);
			OnHideFinished();
		}
	}
}

void UUIDialogueBox::OnShowFinished()
{
	bAnimationInProgress = false;
	UnbindAllFromAnimationFinished(AnimShow);
	OnShowHideAnimFinished.ExecuteIfBound();
	OnShowHideAnimFinished.Unbind();
}

void UUIDialogueBox::OnReverseShowFinished()
{
	bIsShowing = false;
	bAnimationInProgress = false;

	SetVisibility(ESlateVisibility::Collapsed);
	UnbindAllFromAnimationFinished(AnimShow);
	OnShowHideAnimFinished.ExecuteIfBound();
	OnShowHideAnimFinished.Unbind();
}

void UUIDialogueBox::OnHideFinished()
{
	bIsShowing = false;
	bAnimationInProgress = false;

	SetVisibility(ESlateVisibility::Collapsed);
	UnbindAllFromAnimationFinished(AnimHide);
	OnShowHideAnimFinished.ExecuteIfBound();
	OnShowHideAnimFinished.Unbind();
}

void UUIDialogueBox::UpdateSpeaker(const UDSDialogueLineAsset* NewLine)
{
	DialogueSpeakerText->SetText(NewLine->Speaker->Name);

	DialogueSpeakerText->SetVisibility(ESlateVisibility::HitTestInvisible);

	// Move this to its own function
	SpeakerImage->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UUIDialogueBox::UpdateTextLine(const UDSDialogueLineAsset* NewLine)
{
	TypedOutString = NewLine->TextLine.ToString();
	DialogueBodyText->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (bTypeOutText)
	{
		TypeOutTextIdx = 0;
		DialogueBodyText->SetText(FText());
		bDialogueReady = false;
		GetWorld()->GetTimerManager().SetTimer(TypeOutTextHandle, this, &UUIDialogueBox::TypeOutChar, GetTypeOutDelay());
	}
	else
	{
		FastForward();
	}
}

void UUIDialogueBox::UpdateVoiceLine(const UDSDialogueLineAsset* NewLine)
{
	if (NewLine->VoiceLine)
	{
		if (ADSDialogueAudio* DialogueAudio = UDSFunctionLibrary::GetDialogueAudio(CachedController)) // probably bad
		{
			DialogueAudio->PlayVoiceLine(NewLine->VoiceLine);
		}
	}
}

void UUIDialogueBox::UpdateEmotion(const UDSDialogueLineAsset* NewLine)
{
	NewLine->Emotion;
}

void UUIDialogueBox::CancelInProgressEvents()
{
	if (CachedLine && CachedLine->VoiceLine)
	{
		if (ADSDialogueAudio* DialogueAudio = UDSFunctionLibrary::GetDialogueAudio(CachedController)) // probably bad
		{
			DialogueAudio->StopVoiceLine(CachedLine->VoiceLine);
		}
	}
}

void UUIDialogueBox::TypeOutChar()
{
	if (TypedOutString.IsEmpty())
	{
		ensureMsgf(false, TEXT("UUIDialogueBox::TypeOutChar -- Invalid type out string."));
		return;
	}

	// Get all of the text from the previous typed out line, minus all of the 'unread' characters.

	InProgressString = TypedOutString.LeftChop(TypedOutString.Len() - TypeOutTextIdx);

	// Process rich text tags
	while (TypedOutString[TypeOutTextIdx] == '<')
	{
		// Look for opening tag
		if (CachedTag.IsEmpty())
		{
			int32 StartIdx = TypeOutTextIdx;
			TypeOutTextIdx++;
			int32 Len = 2;

			while (TypedOutString[TypeOutTextIdx] != '>')
			{
				TypeOutTextIdx++;
				Len++;
			}
			CachedTag = TypedOutString.Mid(StartIdx+1, Len-2);
			TypeOutTextIdx++;
			InProgressString.AppendChar('<');
			InProgressString.Append(CachedTag);
			InProgressString.AppendChar('>');
		}
		// Look for closing tag
		else
		{
			CachedTag.Empty();
			TypeOutTextIdx += 3;
			InProgressString.Append("</>");
		}
	}

	// Show one more character

	InProgressString.AppendChar(TypedOutString[TypeOutTextIdx]);
	TypeOutTextIdx++;

	// Close hanging tag
	
	if (!CachedTag.IsEmpty())
	{
		InProgressString.Append(TEXT("</>"));
	}

	// Set 'unread' text invisible

	FString CachedUnreadTag = CachedTag;

	FString UnreadTag = TEXT("<DefaultUnread>");

	if (!CachedUnreadTag.IsEmpty())
	{
		UnreadTag.Empty(3);
		UnreadTag.AppendChar('<');
		UnreadTag.Append(CachedUnreadTag);
		UnreadTag.Append(TEXT("Unread"));
		UnreadTag.AppendChar('>');
	}

	InProgressString.Append(UnreadTag);

	FString Unread = TypedOutString.RightChop(TypeOutTextIdx);

	bool bWasClosing = false;
	bool bAddedLastClosing = false;
	for (int32 i = Unread.Len() - 1; i >= 0; i--)
	{
		if (bWasClosing)
		{
			if (Unread[i] != '/')
			{
				Unread.InsertAt(i+1, TEXT("Unread"));
			}
			bWasClosing = false;
		}
		else if (Unread[i] == '>')
		{
			bWasClosing = true;
			if (!bAddedLastClosing)
			{
				Unread.InsertAt(i + 1, TEXT("<DefaultUnread>"));
				bAddedLastClosing = true;
			}
		}
		else if (Unread[i] == '<' && Unread[i+1] != '/')
		{
			Unread.InsertAt(i, TEXT("</>"));
		}
	}

	//InProgressString.Append(TEXT("</>"));
	//
	//InProgressString.Append(UnreadTag);
	InProgressString.Append(Unread);
	InProgressString.Append(TEXT("</>"));

	// Add one extra space. Doing this makes sure that short lines don't prematurely wrap.

	InProgressString.Append(TEXT(" "));

	// Update the display text

	DialogueBodyText->SetText(FText::FromString(InProgressString));

	// Check if done

	if (TypeOutTextIdx >= TypedOutString.Len())
	{
		SetReadyForNextLine();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TypeOutTextHandle, this, &UUIDialogueBox::TypeOutChar, GetTypeOutDelay());
	}
}
