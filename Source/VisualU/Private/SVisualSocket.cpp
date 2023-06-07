// Fill out your copyright notice in the Description page of Project Settings.


#include "SVisualSocket.h"
#include "Animation/CurveSequence.h"
#include "VisualImage.h"
#include "PaperSprite.h"
#include "PaperFlipbook.h"

void SVisualSocket::Construct(const FArguments& Args)
{
	SScaleBox::Construct(Args);

	CurveSequence = FCurveSequence();
	if (VisualImage && VisualImage->GetFlipbook())
	{
		CurveSequence.AddCurve(0.f, VisualImage->GetFlipbook()->GetTotalDuration());
		CurveSequence.Play(AsShared(), true, 0.f, false);
	}
}

int32 SVisualSocket::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (VisualImage && VisualImage->GetFlipbook())
	{
		const FVector3d BoxSize = VisualImage->GetCurrentSprite()->GetRenderBounds().GetBox().GetSize();
		const FVector2D ImageSize = FVector2D(BoxSize.X, BoxSize.Z);
		const FVector2D NewSocketOffset = FVector2D((ImageDesiredPosition.X - ImageSize.X / 2) / ImageSize.X, (ImageDesiredPosition.Y - ImageSize.Y / 2) / ImageSize.Y);
		SocketPosition = NewSocketOffset;
	}
	LayerId = SScaleBox::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	return LayerId;
}

FVector2D SVisualSocket::ComputeDesiredSize(float InScale) const
{
	ChildDesiredSize = SScaleBox::ComputeDesiredSize(InScale);

	return FVector2D::ZeroVector;
}

void SVisualSocket::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	const FGeometry NewGeometry = AllottedGeometry.MakeChild(ChildDesiredSize, FSlateLayoutTransform(SocketPosition * ChildDesiredSize));
	SScaleBox::OnArrangeChildren(NewGeometry, ArrangedChildren);
}

void SVisualSocket::SetSocketOffset(FVector2D InPosition)
{
	SocketPosition = InPosition;

	Invalidate(EInvalidateWidget::Layout);
}

void SVisualSocket::SetVisualImage(UVisualImage* InVisualImage)
{
	VisualImage = InVisualImage;
}

void SVisualSocket::SetImageDesiredPosition(FVector2D InPosition)
{
	ImageDesiredPosition = InPosition;
}