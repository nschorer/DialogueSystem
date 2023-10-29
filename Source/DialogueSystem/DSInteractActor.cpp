// Fill out your copyright notice in the Description page of Project Settings.


#include "DSInteractActor.h"
#include "Components/SphereComponent.h"
#include "UMG/Public/Components/WidgetComponent.h"
#include "DSCharacter_Main.h"

// Sets default values
ADSInteractActor::ADSInteractActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	InteractAwareSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractAwareSphere"));
	InteractAwareSphere->SetupAttachment(RootComponent);
	InteractAwareSphere->SetSphereRadius(500.0f);

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADSInteractActor::BeginPlay()
{
	Super::BeginPlay();

	OwningInteractor = TScriptInterface<IDSInteract>(GetOutermostObject());
	checkf(OwningInteractor, TEXT("InteractActor does not have IDSInteract parent."));
}

void ADSInteractActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InteractAwareSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ADSInteractActor::OnInteractBeginOverlap);
	InteractAwareSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &ADSInteractActor::OnInteractEndOverlap);
}

void ADSInteractActor::BeginDestroy()
{
	Super::BeginDestroy();
	if (InteractingCharacter)
	{
		InteractingCharacter->RemoveInteractable(OwningInteractor);
	}
}

void ADSInteractActor::OnInteractBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ADSInteractActor::OnInteractEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

