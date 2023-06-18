// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/CurveSequence.h"
#include "Widgets/Layout/SScaleBox.h"

class UVisualImage;

/// <summary>
/// Slate widget that acts as a socket for child widget with additional features for <see cref="UVisualImage">Visual Image</see>.
/// </summary>
/// <seealso cref="UVisualSocket"/>
class VISUALU_API SVisualSocket : public SScaleBox
{

public:
	void Construct(const FArguments& Args);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual FVector2D ComputeDesiredSize(float) const override;

	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	void SetSocketOffset(FVector2D InPosition);

	/// \todo Change this to the Slate Widget
	void SetVisualImage(UVisualImage* InVisualImage);

	void SetImageDesiredPosition(FVector2D InPosition);

private:
	mutable FVector2D ChildDesiredSize;

	FCurveSequence CurveSequence;

	/// \todo Change this to the Slate Widget
	UVisualImage* VisualImage;

	mutable FVector2D SocketPosition;

	FVector2D ImageDesiredPosition;
};
