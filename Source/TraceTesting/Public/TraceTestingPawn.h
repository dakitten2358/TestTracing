#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TraceResultSourceInterface.h"
#include "TraceTestingPawn.generated.h"

UCLASS(hidedropdown, hidecategories=(Advanced, Attachment, Mesh, Animation, Clothing, Physics, Rendering, Lighting, Activation, CharacterMovement, AgentPhysics, Avoidance, MovementComponent, Velocity, Shape, Camera, Input, Layers, SkeletalMesh, Optimization, Pawn, Replication, Actor), MinimalAPI)
class ATraceTestingPawn : public ACharacter, public ITraceResultSourceInterface
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Category=Trace, EditAnywhere)
	float Distance = 1000.0f;

	UPROPERTY(Category=Trace, EditAnywhere)
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECollisionChannel::ECC_Camera;

	UPROPERTY(Transient)
	FTraceTestResults TraceTestResults;

public:
	virtual FTraceTestResults GetTraceTestResults() const override;

	/** This pawn class spawns its controller in PostInitProperties to have it available in editor mode*/
	TRACETESTING_API virtual void TickActor( float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction ) override;
	TRACETESTING_API virtual void PostLoad() override;

	void RunTrace();

#if WITH_EDITOR
	TRACETESTING_API virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	TRACETESTING_API virtual void PostEditMove(bool bFinished) override;
	TRACETESTING_API virtual void PostTransacted(const FTransactionObjectEvent& TransactionEvent) override;
	TRACETESTING_API void OnPropertyChanged(const FName PropName);

private:
	static TRACETESTING_API void OnEditorSelectionChanged(UObject* NewSelection);
#endif // WITH_EDITOR

private:
	void UpdateDrawing();

#if WITH_EDITORONLY_DATA
private:
	/** Editor Preview */
	UPROPERTY(Transient)
	TObjectPtr<UTraceRenderingComponent> TraceRenderComponent;
#endif // WITH_EDITORONLY_DATA
};
