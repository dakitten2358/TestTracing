#include "TraceTestingPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Texture2D.h"
#include "TraceRenderingComponent.h"
#include "Components/SkeletalMeshComponent.h"

#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
#include "Misc/TransactionObjectEvent.h"
#include "Editor/EditorEngine.h"
extern UNREALED_API UEditorEngine* GEditor;
#endif // WITH_EDITOR

#include "Engine/Selection.h"
#include "Components/BillboardComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TraceTestingPawn)

ATraceTestingPawn::ATraceTestingPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static FName CollisionProfileName(TEXT("NoCollision"));
	GetCapsuleComponent()->SetCollisionProfileName(CollisionProfileName);

#if WITH_EDITORONLY_DATA
	TraceRenderComponent = CreateEditorOnlyDefaultSubobject<UTraceRenderingComponent>(TEXT("TraceRender"));
	if (TraceRenderComponent)
	{
		TraceRenderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		UArrowComponent* ArrowComp = FindComponentByClass<UArrowComponent>();
		if (ArrowComp != NULL)
		{
			ArrowComp->SetRelativeScale3D(FVector(2, 2, 2));
			ArrowComp->bIsScreenSizeScaled = true;
		}

		UBillboardComponent* SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
		if (!IsRunningCommandlet() && (SpriteComponent != nullptr))
		{
			struct FConstructorStatics
			{
				ConstructorHelpers::FObjectFinderOptional<UTexture2D> TextureObject;
				FName ID_Misc;
				FText NAME_Misc;
				FConstructorStatics()
					: TextureObject(TEXT("/Engine/EditorResources/S_Pawn"))
					, ID_Misc(TEXT("Misc"))
					, NAME_Misc(NSLOCTEXT("SpriteCategory", "Misc", "Misc"))
				{
				}
			};
			static FConstructorStatics ConstructorStatics;

			SpriteComponent->Sprite = ConstructorStatics.TextureObject.Get();
			SpriteComponent->SetRelativeScale3D(FVector(1, 1, 1));
			SpriteComponent->bHiddenInGame = true;
			//SpriteComponent->Mobility = EComponentMobility::Static;
			SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_Misc;
			SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Misc;
			SpriteComponent->SetupAttachment(RootComponent);
			SpriteComponent->bIsScreenSizeScaled = true;
		}
	}
#endif // WITH_EDITORONLY_DATA

	// Default to no tick function, but if we set 'never ticks' to false (so there is a tick function) it is enabled by default
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = true;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DefaultLandMovementMode = MOVE_None;
	}

#if WITH_EDITOR
	if (HasAnyFlags(RF_ClassDefaultObject) && GetClass() == StaticClass())
	{
		USelection::SelectObjectEvent.AddStatic(&ATraceTestingPawn::OnEditorSelectionChanged);
		USelection::SelectionChangedEvent.AddStatic(&ATraceTestingPawn::OnEditorSelectionChanged);
	}
#endif // WITH_EDITOR
}

void ATraceTestingPawn::RunTrace()
{
	FVector Start = GetActorLocation();
	FRotator Rotation = GetActorRotation();
	GetActorEyesViewPoint(Start, Rotation);

	const FVector ViewDir = Rotation.Vector();
	FVector End = Start + (ViewDir * Distance);

	TraceTestResults = FTraceTestResults();
	TraceTestResults.Start = Start;
	TraceTestResults.End = End;
	TraceTestResults.CollisionChannel = CollisionChannel;

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, CollisionChannel);
	if (bHit)
	{
		TraceTestResults.HitResults.Add(HitResult);
	}
}

void ATraceTestingPawn::UpdateDrawing()
{
#if WITH_EDITORONLY_DATA
	if (HasAnyFlags(RF_ClassDefaultObject) == true)
	{
		return;
	}

	UBillboardComponent* SpriteComponent = FindComponentByClass<UBillboardComponent>();
	if (SpriteComponent != NULL)
	{
		SpriteComponent->MarkRenderStateDirty();
	}	

	if (TraceRenderComponent != NULL && TraceRenderComponent->GetVisibleFlag())
	{
		TraceRenderComponent->MarkRenderStateDirty();

#if WITH_EDITOR
		if (GEditor != NULL)
		{
			GEditor->RedrawLevelEditingViewports();
		}
#endif // WITH_EDITOR
	}
#endif // WITH_EDITORONLY_DATA
}

FTraceTestResults ATraceTestingPawn::GetTraceTestResults() const
{
	return TraceTestResults;
}

void ATraceTestingPawn::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::Tick(DeltaTime);

	RunTrace();
}

void ATraceTestingPawn::PostLoad()
{
	Super::PostLoad();

	if (UBillboardComponent* SpriteComponent = FindComponentByClass<UBillboardComponent>())
	{
		SpriteComponent->bHiddenInGame = true;
	}

	UWorld* World = GetWorld();
	PrimaryActorTick.bCanEverTick = World && (World->IsGameWorld() == false);

	if (PrimaryActorTick.bCanEverTick == false)
	{
		// Also disable components that may tick
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->PrimaryComponentTick.bCanEverTick = false;
		}
		if (GetMesh())
		{
			GetMesh()->PrimaryComponentTick.bCanEverTick = false;
		}
	}
}

#if WITH_EDITOR

void ATraceTestingPawn::OnEditorSelectionChanged(UObject* NewSelection)
{
	TArray<ATraceTestingPawn*> SelectedPawns;
	ATraceTestingPawn* SelectedPawn = Cast<ATraceTestingPawn>(NewSelection);
	if (SelectedPawn)
	{
		SelectedPawns.Add(Cast<ATraceTestingPawn>(NewSelection));
	}
	else 
	{
		USelection* Selection = Cast<USelection>(NewSelection);
		if (Selection != NULL)
		{
			Selection->GetSelectedObjects<ATraceTestingPawn>(SelectedPawns);
		}
	}

	for (ATraceTestingPawn* TracePawn : SelectedPawns)
	{
		TracePawn->RunTrace();
	}
}

void ATraceTestingPawn::OnPropertyChanged(const FName PropName)
{

	RunTrace();
	UpdateDrawing();
}

void ATraceTestingPawn::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr)
	{
		OnPropertyChanged(PropertyChangedEvent.MemberProperty->GetFName());
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void ATraceTestingPawn::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	
	RunTrace();
	UpdateDrawing();
}

void ATraceTestingPawn::PostTransacted(const FTransactionObjectEvent& TransactionEvent)
{
	Super::PostTransacted(TransactionEvent);

	if (TransactionEvent.GetEventType() == ETransactionObjectEventType::UndoRedo)
	{
		if (TransactionEvent.GetChangedProperties().Num() > 0)
		{
			// targeted update
			for (const FName PropertyName : TransactionEvent.GetChangedProperties())
			{
				OnPropertyChanged(PropertyName);
			}
		}
		else
		{
			// fallback - make sure the results are up to date
			RunTrace();
		}
	}
}

#endif // WITH_EDITOR
