// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Animation/CurveSequence.h"
#include "Widgets/Layout/SScaleBox.h"

class UVisualImage;

/**
* Slate widget that acts as a socket for the child widget
* with additional features for UVisualImage.
* 
* @seealso UVisualSocket
*/
class VISUALU_API SVisualSocket : public SScaleBox
{

public:
	/**
	* Constructor call for slate declarative syntax.
	*
	* @param Args slate arguments
	*/
	void Construct(const FArguments& Args);

	/*
	* Offsets position of the UVisualImage when it is a child,
	* otherwise paints without changes.
	* 
	* @note currently does not work for the SVisualImage
	* 
	* @param Args contains information about paint of this widget
	* @param AllotedGeometry base geometry for this widget
	* @param MyCullingRect culling bounds of this widget
	* @param OutDrawElements elements to draw in the slate window
	* @param LayerId layer on which elements should be drawn
	* @param InWidgetStyle base widget appearance info
	* @param bParentEnabled is parent widget enabled
	*/
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	/**
	* Calculates desire size.
	* SVisualSocket has zero size.
	* 
	* @return computed desired size of this widget
	*/
	virtual FVector2D ComputeDesiredSize(float) const override;

	/**
	* Handles geometry for child widgets.
	* Offsets by SVisualSocket::SocketPosition
	* and SVisualSocket::ChildDesiredSize.
	* 
	* @param AllottedGeometry base geometry for this widget
	* @param ArrangedChildren result of this children arrangement
	*/
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	/**
	* Setter for SVisualSocket::SocketPosition.
	* Invalidates layout of this widget.
	* 
	* @param InPosition new socket offset
	*/
	void SetSocketOffset(FVector2D InPosition);

	/**
	* Setter for SVisualSocket::VisualImage.
	* 
	* @param InVisualImage child visual image
	*/
	void SetVisualImage(UVisualImage* InVisualImage);

	/**
	* Setter for SVisualSocket::ImageDesiredPosition.
	* 
	* @param InPosition new image position
	*/
	void SetImageDesiredPosition(FVector2D InPosition);

private:
	/**
	* Desired size of the child.
	* It is affected by the scale applied to the SVisualSocket.
	*/
	mutable FVector2D ChildDesiredSize;

	/**
	* Optional child visual image.
	*/
	UVisualImage* VisualImage;

	/**
	* Offset to apply to the child widget.
	*/
	mutable FVector2D SocketPosition;
	
	/**
	* Desired position of the child visual image.
	*/
	FVector2D ImageDesiredPosition;
};
