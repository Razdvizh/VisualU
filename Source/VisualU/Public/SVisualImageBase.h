// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/CurveSequence.h"
#include "Widgets/SLeafWidget.h"

class UPaperFlipbook;
class UPaperSprite;
class SVisualImage;

/// <summary>
/// Slate base class for custom widgets that serve as FSlateBrush containers.
/// </summary>
/// <typeparam name="DerivedT">Subclass which derives directly from SVisualImageBase</typeparam>
template<class DerivedT>
class VISUALU_API SVisualImageBase : public SLeafWidget
{

protected:
	virtual FSlateBrush ConvertToBrush() const final;

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override final;

	///ConvertToBrush step methods
	bool IsResourceValid() const;

	UObject* GetFinalResource() const;

	const FVector2D GetImageSize() const;
	///~ConvertToBrush step methods

	///OnPaint step methods
	const FLinearColor GetFinalColorAndOpacity(const FWidgetStyle& InWidgetStyle) const;

	void PreSlateDrawElementExtension() const;

	FGeometry MakeCustomGeometry(const FGeometry& AllotedGeometry) const;

	void PostSlateDrawElementExtension() const;
	///~OnPaint step methods

private:
	SVisualImageBase() {};
	SVisualImageBase(SVisualImageBase const&) {};
	FORCEINLINE SVisualImageBase& operator=(SVisualImageBase const&) { return *this; }
	~SVisualImageBase() {};

	friend DerivedT;
};

template<class DerivedT>
inline FSlateBrush SVisualImageBase<DerivedT>::ConvertToBrush() const
{
	FSlateBrush Brush = FSlateBrush(); // < body

	if (IsResourceValid()) //step 1 - Is Resource Valid
	{
		UObject* FinalResource = GetFinalResource(); //step 2 - Get Final Resource

		checkSlow(FinalResource); // < body

		const FVector2D ImageSize = GetImageSize(); // step 3 - Get Size

		Brush.SetResourceObject(FinalResource); // < body
		Brush.ImageSize = ImageSize; // < body
		Brush.ImageType = ESlateBrushImageType::FullColor; // < body
		Brush.DrawAs = ESlateBrushDrawType::Image; // < body
		Brush.Tiling = ESlateBrushTileType::NoTile; // < body
		Brush.TintColor = FSlateColor(FLinearColor::White); // < body
	}

	return Brush;
}

template<class DerivedT>
inline int32 SVisualImageBase<DerivedT>::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush Brush = ConvertToBrush(); //step 1 - Get FSlateBrush

	const FLinearColor FinalColorAndOpacity = GetFinalColorAndOpacity(InWidgetStyle); //step 2(opt.) - Get Color And Opacity

	if (IsResourceValid()) // < Body
	{
		PreSlateDrawElementExtension();

		const FGeometry CustomGeometry = MakeCustomGeometry(AllottedGeometry); //step 3(opt.) Make Custom Gemoetry
		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, CustomGeometry.ToPaintGeometry(), &Brush, ESlateDrawEffect::None, FinalColorAndOpacity);

		PostSlateDrawElementExtension();
	}

	//hook 2 > PostSlateBoxCreated

	return LayerId;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class DerivedT>
inline bool SVisualImageBase<DerivedT>::IsResourceValid() const
{
	return static_cast<const DerivedT*>(this)->IsResourceValid();
}

template<class DerivedT>
inline UObject* SVisualImageBase<DerivedT>::GetFinalResource() const
{
	return static_cast<const DerivedT*>(this)->GetFinalResource();
}

template<class DerivedT>
inline const FVector2D SVisualImageBase<DerivedT>::GetImageSize() const
{
	return static_cast<const DerivedT*>(this)->GetImageSize();
}

template<class DerivedT>
inline const FLinearColor SVisualImageBase<DerivedT>::GetFinalColorAndOpacity(const FWidgetStyle& InWidgetStyle) const
{
	return static_cast<const DerivedT*>(this)->GetFinalColorAndOpacity(InWidgetStyle);
}

template<class DerivedT>
inline void SVisualImageBase<DerivedT>::PreSlateDrawElementExtension() const
{
	static_cast<const DerivedT*>(this)->PreSlateDrawElementExtension();
}

template<class DerivedT>
inline FGeometry SVisualImageBase<DerivedT>::MakeCustomGeometry(const FGeometry& AllotedGeometry) const
{
	return static_cast<const DerivedT*>(this)->MakeCustomGeometry(AllotedGeometry);
}

template<class DerivedT>
inline void SVisualImageBase<DerivedT>::PostSlateDrawElementExtension() const
{
	static_cast<const DerivedT*>(this)->PostSlateDrawElementExtension();
}
