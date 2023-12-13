// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TraceResultSourceInterface.generated.h"

USTRUCT()
struct FTraceTestResults
{
	GENERATED_BODY()

	// results, empty = no hit
	TArray<FHitResult> HitResults;

	// query
	FVector Start;
	FVector End;
	ECollisionChannel CollisionChannel;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTraceResultSourceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ITraceResultSourceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FTraceTestResults GetTraceTestResults() const { return FTraceTestResults(); }
};
