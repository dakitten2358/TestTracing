#pragma once

#include "CoreMinimal.h"
#include "Debug/DebugDrawComponent.h"
#include "DebugRenderSceneProxy.h"
#include "TraceRenderingComponent.generated.h"

class APlayerController;
class ITraceResultSourceInterface;
class UCanvas;

class FTraceSceneProxy final : public FDebugRenderSceneProxy
{
	friend class FTraceRenderingDebugDrawDelegateHelper;
public:
	TRACETESTING_API virtual SIZE_T GetTypeHash() const override;

	TRACETESTING_API explicit FTraceSceneProxy(const UPrimitiveComponent& InComponent);
	
	TRACETESTING_API virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	//static TRACETESTING_API void CollectTraceData(const UPrimitiveComponent* InComponent, const ITraceResultSourceInterface* QueryDataSource, TArray<FDebugLine>& Lines, TArray<FBox>& Boxes);
	//static TRACETESTING_API void CollectTraceData(const bool bHit,  const FHitResult& HitResult, const FVector& Start, const FVector& End, const ECollisionChannel CollisionChannel, TArray<FDebugLine>& Lines, TArray<FBox>& Boxes);

private:
	// can be 0
	AActor* ActorOwner;
	const ITraceResultSourceInterface* TraceResultDataSource;

	TRACETESTING_API bool SafeIsActorSelected() const;
};

UCLASS(ClassGroup = Debug, MinimalAPI)
class UTraceRenderingComponent : public UDebugDrawComponent
{
	GENERATED_UCLASS_BODY()

protected:
	TRACETESTING_API virtual FBoxSphereBounds CalcBounds(const FTransform &LocalToWorld) const override;

#if UE_ENABLE_DEBUG_DRAWING
	TRACETESTING_API virtual FDebugRenderSceneProxy* CreateDebugSceneProxy() override;
#endif
};
