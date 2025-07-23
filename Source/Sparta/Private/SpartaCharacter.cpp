#include "SpartaCharacter.h"
#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "SpartaGameState.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 320.0f;
	SprintSpeedMultiplier = 2.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	SlowWalkSpeedMultiplier = 0.3f;
	SlowWalkSpeed = NormalSpeed * SlowWalkSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.0f;
	Health = MaxHealth;
}
void ASpartaCharacter::BeginPlay() 
{
	Super::BeginPlay();
	UpdateOverheadHP();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
				);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);
			}
			if (PlayerController->SlowWalkAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SlowWalkAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSlowWalk
				);
			}
			if (PlayerController->SlowWalkAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SlowWalkAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSlowWalk
				);
			}

		}
	}
}

void  ASpartaCharacter::Move(const FInputActionValue& value)
{	  
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(),MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}

}	  
void  ASpartaCharacter::StartJump(const FInputActionValue& value)
{	  
	if (value.Get<bool>())
	{
		Jump();
	}
}	  
void  ASpartaCharacter::StopJump(const FInputActionValue& value)
{	  
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}	  
void ASpartaCharacter::Look(const FInputActionValue& value)
{	  
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}	  
void  ASpartaCharacter::StartSprint(const FInputActionValue& value)
{	  
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}	  
void  ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void ASpartaCharacter::StartSlowWalk(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SlowWalkSpeed;
	}
}
void ASpartaCharacter::StopSlowWalk(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float HealAmount)
{
	Health = FMath::Clamp(Health + HealAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();
}

float ASpartaCharacter::TakeDamage(
	float DamageAmout,										//몬스터 기본데미지
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser) 
{
	float ActualDamage = Super::TakeDamage(DamageAmout, DamageEvent, EventInstigator, DamageCauser);
			
	Health = FMath::Clamp(Health - DamageAmout, 0.0f, MaxHealth);            // health값이 0이하 maxHealth 이상으로 올라가지않게 제한
	UpdateOverheadHP();
	if (Health <= 0.0f)
	{
		OnDeath();
	}
	
	return ActualDamage;                                  //캐릭터에 방어력이 적용된 실제로 입은 데미지
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if(UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), Health, MaxHealth)));
	}
}