// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Animation/CurveSequence.h"
#include "Widgets/SLeafWidget.h"

class UPaperFlipbook;
class UPaperSprite;
class SVisualImage;

/**
* Base slate class for widgets that can display some render resource.
* Utilizes CRTP with double dispatch for derived classes,
* template function is SVisualImageBase::ConvertToBrush.
* 
* @param <DerivedT> derived class of SVisualImageBase
* 
* @seealso UVisualImageBase
*/
template<class DerivedT>
class VISUALU_API SVisualImageBase : public SLeafWidget
{

protected:
	/**
	* Handles conversion of the render resource to the slate brush.
	* 
	* @note this function is finalized
	* 
	* @return slate brush made from SVisualImageBase::GetFinalResource()
	*/
	virtual FSlateBrush ConvertToBrush() const final;

	/**
	* Paints render resource as a brush after applying all modifiers.
	* 
	* @param Args contains information about paint of this widget
	* @param AllotedGeometry base geometry for this widget
	* @param MyCullingRect culling bounds of this widget
	* @param OutDrawElements elements to draw in the slate window
	* @param LayerId layer on which elements should be drawn
	* @param InWidgetStyle base widget appearance info
	* @param bParentEnabled is parent widget enabled
	*/
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override final;

	/**
	* Used during resource-to-brush conversion
	* to stop it early when resource is invalid.
	* 
	* @return validity of the supplied resource
	*/
	bool IsResourceValid() const;

	/**
	* Resource to be displayed by the widget.
	* 
	* @return render resource
	*/
	UObject* GetFinalResource() const;

	/**
	* @return slate size of the resource
	*/
	const FVector2D GetImageSize() const;

	/**
	* @param InWidgetStyle base widget style
	* @return color and opacity to be applied to the resource
	*/
	const FLinearColor GetFinalColorAndOpacity(const FWidgetStyle& InWidgetStyle) const;

	/**
	* Optional extension to the SVisualImageBase::OnPaint template.
	* Called before render resource is drawn.
	*/
	void PreSlateDrawElementExtension() const;

	/**
	* @param AllotedGeometry base geometry for this widget
	* @return geometry of this widget
	*/
	FGeometry MakeCustomGeometry(const FGeometry& AllotedGeometry) const;

	/**
	* Optional extension to the SVisualImageBase::OnPaint template.
	* Called after render resource is drawn.
	*/
	void PostSlateDrawElementExtension() const;

private:
	/**
	* It is an abstract class, can't be constructed.
	*/
	SVisualImageBase() = default;
	SVisualImageBase(SVisualImageBase const&) = delete;
	SVisualImageBase& operator=(SVisualImageBase const&) = delete;
	~SVisualImageBase() = default;

	friend DerivedT;
};

template<class DerivedT>
inline FSlateBrush SVisualImageBase<DerivedT>::ConvertToBrush() const
{
	FSlateBrush Brush = FSlateBrush();

	if (IsResourceValid())
	{
		UObject* FinalResource = GetFinalResource();

		checkSlow(FinalResource);

		const FVector2D ImageSize = GetImageSize();

		Brush.SetResourceObject(FinalResource);
		Brush.ImageSize = ImageSize;
		Brush.ImageType = ESlateBrushImageType::FullColor;
		Brush.DrawAs = ESlateBrushDrawType::Image;
		Brush.Tiling = ESlateBrushTileType::NoTile;
		Brush.TintColor = FSlateColor(FLinearColor::White);
	}

	return Brush;
}

template<class DerivedT>
inline int32 SVisualImageBase<DerivedT>::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush Brush = ConvertToBrush();

	const FLinearColor FinalColorAndOpacity = GetFinalColorAndOpacity(InWidgetStyle);

	if (IsResourceValid())
	{
		PreSlateDrawElementExtension();

		const FGeometry CustomGeometry = MakeCustomGeometry(AllottedGeometry);
		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, CustomGeometry.ToPaintGeometry(), &Brush, ESlateDrawEffect::None, FinalColorAndOpacity);

		PostSlateDrawElementExtension();
	}

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
