// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VisualDefaults.generated.h"

struct FAnchors;
struct FMargin;

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

	FString ToString() const
	{
		return FString::Printf(TEXT("Minimum: %s, Maximum: %s"), *Minimum.ToString(), *Maximum.ToString());
	}
};

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

	FString ToString() const
	{
		return FString::Printf(TEXT("Left: %d, Top: %d, Right: %d, Bottom: %d"), Left, Top, Right, Bottom);
	}
};
