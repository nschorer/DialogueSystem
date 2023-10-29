// Fill out your copyright notice in the Description page of Project Settings.


#include "DSInteractComponent.h"
#include "Components/SphereComponent.h"
#include "UMG/Public/Components/WidgetComponent.h"
#include "DSCharacter_Main.h"

// Sets default values for this component's properties
UDSInteractComponent::UDSInteractComponent()
{
	InteractAwareSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractAwareSphere"));
	//InteractAwareSphere->SetupAttachment(this);
	InteractAwareSphere->SetSphereRadius(500.0f);

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	//InteractWidget->SetupAttachment(this);
}

// Called when the game starts
void UDSInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningInteractor = TScriptInterface<IDSInteract>(GetOwner());
	checkf(OwningInteractor, TEXT("InteractActor does not have IDSInteract parent."));

	InteractAwareSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &UDSInteractComponent::OnInteractBeginOverlap);
	InteractAwareSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &UDSInteractComponent::OnInteractEndOverlap);
}

void UDSInteractComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (InteractingCharacter)
	{
		InteractingCharacter->RemoveInteractable(OwningInteractor);
	}
}

void UDSInteractComponent::OnInteractBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OwningInteractor)
	{
		if (ADSCharacter_Main* MainCharacter = Cast<ADSCharacter_Main>(OtherActor))
		{
			MainCharacter->AddInteractable(OwningInteractor);
			InteractingCharacter = MainCharacter;
		}
	}
}

void UDSInteractComponent::OnInteractEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OwningInteractor)
	{
		if (ADSCharacter_Main* MainCharacter = Cast<ADSCharacter_Main>(OtherActor))
		{
			MainCharacter->RemoveInteractable(OwningInteractor);
			InteractingCharacter = nullptr;
		}
	}
}

