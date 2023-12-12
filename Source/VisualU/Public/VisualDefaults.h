// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/Anchors.h"
#include "Layout/Margin.h"
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
	FVisualAnchors() : FAnchors() {}
	FVisualAnchors(float UniformAnchors) : FAnchors(UniformAnchors) {}
	FVisualAnchors(float Horizontal, float Vertical) : FAnchors(Horizontal, Vertical) {}
	FVisualAnchors(float MinX, float MinY, float MaxX, float MaxY) : FAnchors(MinX, MinY, MaxX, MaxY) {}
	FVisualAnchors(const FAnchors& Other) { Minimum = Other.Minimum; Maximum = Other.Maximum; }

	static FAnchors Default;
	static FAnchors FullScreen;
	static FAnchors BottomLeft;
	static FAnchors TopLeft;
	static FAnchors MiddleLeft;
	static FAnchors BottomRight;
	static FAnchors TopRight;
	static FAnchors MiddleRight;
	static FAnchors BottomCenter;
	static FAnchors TopCenter;
	static FAnchors Center;
	static FAnchors TopHorizontal;
	static FAnchors MiddleHorizontal;
	static FAnchors BottomHorizontal;
	static FAnchors LeftVertical;
	static FAnchors CenterVertical;
	static FAnchors RightVertical;

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
	FVisualMargin() : FMargin() {}
	FVisualMargin(float UniformMargin) : FMargin(UniformMargin) {}
	FVisualMargin(float Horizontal, float Vertical) : FMargin(Horizontal, Vertical) {}
	FVisualMargin(const FVector2D& InVector) : FMargin(InVector) {}
	FVisualMargin(float InLeft, float InTop, float InRight, float InBottom) : FMargin(InLeft, InTop, InRight, InBottom) {}
	FVisualMargin(const FVector4& InVector) : FMargin(InVector) {}

	static FMargin Zero;
	
	/// <summary>
	/// Get a string representation of margin.
	/// </summary>
	/// <returns>String of all margins</returns>
	FString ToString() const
	{
		return FString::Printf(TEXT("Left: %d, Top: %d, Right: %d, Bottom: %d"), Left, Top, Right, Bottom);
	}
};
