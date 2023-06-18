// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint\UserWidget.h"
#include "VisualDefaults.generated.h"

struct FAnchors;
struct FMargin;

/// <summary>
/// Extends [FAnchors](https://docs.unrealengine.com/4.26/en-US/API/Runtime/Slate/Widgets/Layout/FAnchors/) 
/// with human-friendly constants for all possible anchors' positions.
/// </summary>
USTRUCT(BlueprintType)
struct FVisualAnchors : public FAnchors
{
	GENERATED_USTRUCT_BODY()

public:
	FVisualAnchors() : Super() {}
	FVisualAnchors(float UniformAnchors) : Super(UniformAnchors) {}
	FVisualAnchors(float Horizontal, float Vertical) : Super(Horizontal, Vertical) {}
	FVisualAnchors(float MinX, float MinY, float MaxX, float MaxY) : Super(MinX, MinY, MaxX, MaxY) {}

	inline static const FAnchors Default = FAnchors();
	inline static const FAnchors FullScreen = FAnchors(0, 0, 1, 1);
	inline static const FAnchors BottomLeft = FAnchors(0, 1, 0, 1);
	inline static const FAnchors TopLeft = FAnchors(0, 0, 0, 0);
	inline static const FAnchors MiddleLeft = FAnchors(0, 0.5, 0, 0.5);
	inline static const FAnchors BottomRight = FAnchors(1, 1, 1, 1);
	inline static const FAnchors TopRight = FAnchors(1, 0, 1, 0);
	inline static const FAnchors MiddleRight = FAnchors(1, 0.5, 1, 0.5);
	inline static const FAnchors BottomCenter = FAnchors(0.5, 1, 0.5, 1);
	inline static const FAnchors TopCenter = FAnchors(0.5, 0, 0.5, 0);
	inline static const FAnchors Center = FAnchors(0.5, 0.5, 0.5, 0.5);
	inline static const FAnchors TopHorizontal = FAnchors(0, 0, 1, 0);
	inline static const FAnchors MiddleHorizontal = FAnchors(0, 0.5, 1, 0.5);
	inline static const FAnchors BottomHorizontal = FAnchors(0, 1, 1, 1);
	inline static const FAnchors LeftVertical = FAnchors(0, 0, 0, 1);
	inline static const FAnchors CenterVertical = FAnchors(0.5, 0, 0.5, 1);
	inline static const FAnchors RightVertical = FAnchors(1, 0, 1, 1);

	/// <summary>
	/// Get a string representation of anchors.
	/// </summary>
	/// <returns>String of a minimum and maximum components</returns>
	FString ToString() const
	{
		return FString::Printf(TEXT("Minimum: %s, Maximum: %s"), *Minimum.ToString(), *Maximum.ToString());
	}
};

/// <summary>
/// Extends [FMargin](https://docs.unrealengine.com/4.26/en-US/API/Runtime/SlateCore/Layout/FMargin/) with <see cref="Zero"/> margin constant 
/// and a <see cref="ToString"/> method.
/// </summary>
USTRUCT(BlueprintType)
struct FVisualMargin : public FMargin
{
	GENERATED_USTRUCT_BODY()

public:
	FVisualMargin() : Super() {}
	FVisualMargin(float UniformMargin) : Super(UniformMargin) {}
	FVisualMargin(float Horizontal, float Vertical) : Super(Horizontal, Vertical) {}
	FVisualMargin(const FVector2D& InVector) : Super(InVector) {}
	FVisualMargin(float InLeft, float InTop, float InRight, float InBottom) : Super(InLeft, InTop, InRight, InBottom) {}
	FVisualMargin(const FVector4& InVector) : Super(InVector) {}

	inline static const FMargin Zero = FMargin(0, 0);
	
	/// <summary>
	/// Get a string representation of margin.
	/// </summary>
	/// <returns>String of all margins</returns>
	FString ToString() const
	{
		return FString::Printf(TEXT("Left: %d, Top: %d, Right: %d, Bottom: %d"), Left, Top, Right, Bottom);
	}
};
