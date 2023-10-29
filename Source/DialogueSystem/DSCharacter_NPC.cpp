// Fill out your copyright notice in the Description page of Project Settings.


#include "DSCharacter_NPC.h"
#include "Components/SphereComponent.h"
#include "UMG/Public/Components/WidgetComponent.h"
#include "DSCharacter_Main.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DSPlayerController.h"
#include "DSCharacter.h"

ADSCharacter_NPC::ADSCharacter_NPC()
{
	InteractAwareSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractAwareSphere"));
	InteractAwareSphere->SetupAttachment(RootComponent);
	InteractAwareSphere->SetGenerateOverlapEvents(true);
	InteractAwareSphere->SetSphereRadius(700.0f);
	InteractAwareSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InteractAwareSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(RootComponent);
	InteractWidget->SetVisibility(false);
	InteractWidget->SetDrawSize(FVector2D(32.f, 32.f));
}

void ADSCharacter_NPC::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InteractAwareSphere->OnComponentBeginOverlap.AddDynamic(this, &ADSCharacter_NPC::OnInteractBeginOverlap);
	InteractAwareSphere->OnComponentEndOverlap.AddDynamic(this, &ADSCharacter_NPC::OnInteractEndOverlap);
}

void ADSCharacter_NPC::BeginPlay()
{
	Super::BeginPlay();
}

void ADSCharacter_NPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateInteractWidget();
}

void ADSCharacter_NPC::BeginDestroy()
{
	Super::BeginDestroy();
	if (CachedInteractor)
	{
		RemoveInteractable(CachedInteractor);
	}
}

void ADSCharacter_NPC::SetInteractPossible(bool bIsPossible)
{
	if (CachedIsInteractPossible != bIsPossible)
	{
		CachedIsInteractPossible = bIsPossible;
		if (!CachedInteracting)
		{
			PlayInteractAvailableAnimation(bIsPossible);
		}
	}
}

void ADSCharacter_NPC::SetFocus(bool bIsFocused)
{
	CachedHasInteractFocus = bIsFocused;
	PlayInteractFocusAnimation(CachedHasInteractFocus);
	if (bIsFocused)
	{
		InteractWidget->SetVisibility(bIsFocused);
	}
}

void ADSCharacter_NPC::AddInteractable(class ADSCharacter_Main* MainCharacter)
{
	MainCharacter->AddInteractable(this);
	CachedInteractor = MainCharacter;
}

void ADSCharacter_NPC::RemoveInteractable(class ADSCharacter_Main* MainCharacter)
{
	MainCharacter->RemoveInteractable(this);
	CachedInteractor = nullptr;
}

void ADSCharacter_NPC::StopInteracting()
{
	CachedInteracting = false;
}

void ADSCharacter_NPC::InteractNative()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Interact!"));
	if (ADSPlayerController* DSController = Cast<ADSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		UDialogueAsset* DialogueData = CharacterData ? CharacterData->DialogueData : nullptr;
		DSController->OnInitiateDialogue.Broadcast(DialogueData);
	}
	PlayInteractAnimation();
	CachedInteracting = true;
}

bool ADSCharacter_NPC::CanInteractNative()
{
	return true;
}

void ADSCharacter_NPC::OnInteractBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Interact Begin Overlap"));
	if (ADSCharacter_Main* MainCharacter = Cast<ADSCharacter_Main>(OtherActor))
	{
		AddInteractable(MainCharacter);
	}
}

void ADSCharacter_NPC::OnInteractEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == this) return;

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Interact End Overlap"));
	if (ADSCharacter_Main* MainCharacter = Cast<ADSCharacter_Main>(OtherActor))
	{
		RemoveInteractable(MainCharacter);
	}
}

void ADSCharacter_NPC::UpdateInteractWidget()
{
	if (InteractWidget->IsVisible())
	{
		InteractWidget->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(InteractWidget->GetComponentLocation(), UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation()));
	}
}
