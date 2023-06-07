// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SScaleBox.h"

class UVisualImage;
/**
 * 
 */
class VISUALU_API SVisualSocket : public SScaleBox
{

public:
	void Construct(const FArguments& Args);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual FVector2D ComputeDesiredSize(float) const override;

	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	void SetSocketOffset(FVector2D InPosition);

	void SetVisualImage(UVisualImage* InVisualImage);

	void SetImageDesiredPosition(FVector2D InPosition);

private:
	mutable FVector2D ChildDesiredSize;

	FCurveSequence CurveSequence;

	UVisualImage* VisualImage;

	mutable FVector2D SocketPosition;

	FVector2D ImageDesiredPosition;
};
