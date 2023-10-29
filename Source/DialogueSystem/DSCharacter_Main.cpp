// Fill out your copyright notice in the Description page of Project Settings.


#include "DSCharacter_Main.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DSPlayerController.h"


//////////////////////////////////////////////////////////////////////////
// ADSCharacter

ADSCharacter_Main::ADSCharacter_Main()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ADSCharacter_Main::Tick(float DeltaSeconds)
{
	ProcessInteractables();
}

void ADSCharacter_Main::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	bCanInteract = true;
}

void ADSCharacter_Main::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADSCharacter_Main::InputMove);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADSCharacter_Main::InputLook);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ADSCharacter_Main::InputInteract);
	}

}

void ADSCharacter_Main::InputMove(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADSCharacter_Main::InputLook(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADSCharacter_Main::InputInteract()
{
	if (BestInteractable)
	{
		BestInteractable->InteractNative();
		CurrentInteractable = BestInteractable;
	}
}

void ADSCharacter_Main::ProcessInteractables()
{
	ADSPlayerController* DSController = Cast<ADSPlayerController>(Controller);
	if (!DSController) return;

	FVector CameraLocation = FollowCamera->GetComponentLocation();

	TScriptInterface<IDSInteract> OldBestInteractable = BestInteractable;
	float BestInteractValue = -1.f;
	BestInteractable = nullptr;

	for (int32 i = Interactables.Num() - 1; i >= 0; i--)
	{
		TScriptInterface<IDSInteract> Interactable = Interactables[i];
		bool bInteractPossible = false;
		if (Interactable)
		{
			if (AActor* InteractableActor = Cast<AActor>(Interactable.GetObject()))
			{
				if (bCanInteract)
				{
					bInteractPossible = true;

					FVector CharacterToTarget = InteractableActor->GetActorLocation() - GetActorLocation();
					// Distance Check
					if (CharacterToTarget.SizeSquared() < 90000) // 300
					{
						// Make sure our character is somewhat looking at the target
						if (GetActorForwardVector().Dot(CharacterToTarget) > 0)
						{
							FVector CameraToTarget = InteractableActor->GetActorLocation() - CameraLocation;
							CameraToTarget.Normalize();
							FVector CameraForward = FollowCamera->GetForwardVector();
							CameraForward.Normalize();

							float DotProduct = CameraToTarget.Dot(CameraForward);
							if (DotProduct > 0.77f && DotProduct > BestInteractValue)
							{
								BestInteractValue = DotProduct;
								BestInteractable = Interactable;
							}
						}
					}
				}
			}
		}
		else
		{
			Interactables.RemoveAt(i);
		}

		Interactable->SetInteractPossible(bInteractPossible);
	}

	// BestInteractable changed
	if (OldBestInteractable != BestInteractable)
	{
		if (OldBestInteractable)
		{
			OldBestInteractable->SetFocus(false);
		}
		if (BestInteractable)
		{
			BestInteractable->SetFocus(true);
		}
	}
}

void ADSCharacter_Main::AddInteractable(TScriptInterface<IDSInteract> Interactable)
{
	Interactables.Add(Interactable);
}

void ADSCharacter_Main::RemoveInteractable(TScriptInterface<IDSInteract> Interactable)
{
	Interactables.Remove(Interactable);
	Interactable->SetInteractPossible(false);
}

void ADSCharacter_Main::BeginDialogue()
{
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
		}
	}

	bCanInteract = false;
}

void ADSCharacter_Main::EndDialogue()
{
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	bCanInteract = true;
	if (CurrentInteractable)
	{
		CurrentInteractable->StopInteracting();
	}
}
