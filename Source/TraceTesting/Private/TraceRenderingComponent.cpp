#include "TraceRenderingComponent.h"
#include "Engine/Canvas.h"
#include "TraceResultSourceInterface.h"
#include "SceneInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TraceRenderingComponent)

//----------------------------------------------------------------------//
// FTraceSceneProxy
//----------------------------------------------------------------------//
SIZE_T FTraceSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

FTraceSceneProxy::FTraceSceneProxy(const UPrimitiveComponent& InComponent)
	: FDebugRenderSceneProxy(&InComponent)
	, ActorOwner(nullptr)
{
	FColor NoHitColor = FColor::Green;
	FColor HitColor = FColor::Red;
	FVector BoxSize = FVector(6.0f, 6.0f, 6.0f);

	bWantsSelectionOutline = false;
	
	ActorOwner = InComponent.GetOwner();
	TraceResultDataSource = Cast<const ITraceResultSourceInterface>(ActorOwner);
	
	if (TraceResultDataSource)
	{
		FTraceTestResults results = TraceResultDataSource->GetTraceTestResults();
		FVector Start = results.Start;
		FVector End = results.End;

		if (results.HitResults.Num() == 0)
		{
			Lines.Emplace(Start, End, NoHitColor, 2.0f);
		}
		else
		{
			TArray<FVector> Points;
			Points.Add(Start);
			for (int hitResultIndex = 0; hitResultIndex < results.HitResults.Num(); ++hitResultIndex)
			{
				Points.Add(results.HitResults[hitResultIndex].ImpactPoint);
			}
			Points.Add(End);

			for (int pointIndex = 1; pointIndex < Points.Num(); ++pointIndex)
			{
				FColor c = pointIndex == 1 ? NoHitColor : HitColor;
				Lines.Emplace(Points[pointIndex - 1], Points[pointIndex], c, 2.0f);

				if (pointIndex != (Points.Num() - 1))
					Boxes.Emplace(FDebugBox(FBox(Points[pointIndex] - BoxSize, Points[pointIndex] + BoxSize), HitColor));
			}
		}

	}
}

bool FTraceSceneProxy::SafeIsActorSelected() const
{
	if(ActorOwner)
	{
		return ActorOwner->IsSelected();
	}

	return false;
}

FPrimitiveViewRelevance FTraceSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = true; //View->Family->EngineShowFlags.GetSingleFlag(ViewFlagIndex) && IsShown(View) 
		/*&& (!bDrawOnlyWhenSelected || SafeIsActorSelected())*/;
	Result.bDynamicRelevance = true;
	// ideally the TranslucencyRelevance should be filled out by the material, here we do it conservative
	Result.bSeparateTranslucency = Result.bNormalTranslucency = IsShown(View);
	Result.bOpaque = true;
	Result.bEditorPrimitiveRelevance = true;
	return Result;
}

//----------------------------------------------------------------------//
// UTraceRenderingComponent
//----------------------------------------------------------------------//
UTraceRenderingComponent::UTraceRenderingComponent(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
}

#if UE_ENABLE_DEBUG_DRAWING
FDebugRenderSceneProxy* UTraceRenderingComponent::CreateDebugSceneProxy()
{
	FTraceSceneProxy* NewSceneProxy = new FTraceSceneProxy(*this);
	return NewSceneProxy;
}
#endif


FBoxSphereBounds UTraceRenderingComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds::Builder DebugBoundsBuilder;

	AActor* ActorOwner = Cast<AActor>(GetOwner());
	if (ActorOwner) DebugBoundsBuilder += ActorOwner->GetActorLocation();

	const ITraceResultSourceInterface* TraceResultSource = Cast<const ITraceResultSourceInterface>(GetOwner());

	FTraceTestResults Results = TraceResultSource->GetTraceTestResults();
	if (TraceResultSource) DebugBoundsBuilder += Results.Start;
	if (TraceResultSource) DebugBoundsBuilder += Results.End;
	return DebugBoundsBuilder;
}


