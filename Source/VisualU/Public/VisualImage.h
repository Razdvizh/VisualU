// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "VisualImageBase.h"
#include "Engine/StreamableManager.h"
#include "InfoAssignable.h"
#include "VisualImage.generated.h"

class UPaperFlipbook;
class UPaperSprite;
class SVisualImage;

/**
* Collection of data that governs appearance of UVisualImage.
*/
USTRUCT(BlueprintType)
struct FVisualImageInfo : public FVisualInfo
{
	GENERATED_BODY()

public:
	FVisualImageInfo()
		: Expression(),
		ColorAndOpacity(1, 1, 1, 1),
		DesiredScale(1, 1),
		MirrorScale(1, 1),
		bAnimate(false),
		FrameIndex(0)
	{
	}

	/**
	* @see UVisualImage::Flipbook
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info")
	TSoftObjectPtr<UPaperFlipbook> Expression;

	/**
	* @see UVisualImage::ColorAndOpacity
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info")
	FLinearColor ColorAndOpacity;

	/**
	* @see UVisualImage::DesiredScale
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info", meta = (Delta = 0.25f, UIMin = 0, ClampMin = 0, AllowPreserveRatio))
	FVector2D DesiredScale;

	/**
	* @see UVisualImage::MirrorScale
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info", meta = (Delta = 1.f, UIMin = -1, UIMax = 1, ClampMin = -1, ClampMax = 1))
	FVector2D MirrorScale;

	/**
	* @see UVisualImage::bAnimate
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info")
	bool bAnimate;

	/**
	* @see UVisualImage::FrameIndex
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info")
	int32 FrameIndex;

	/**
	* Accepts provided visitor.
	* 
	* @param Visitor visitor that wants to assign visual image info
	*/
	virtual void Accept(IInfoAssignable* Visitor) const override
	{
		Visitor->AssignVisualImageInfo(*this);
	}

	/**
	* String representation of FVisualImageInfo.
	* 
	* @return string describing all members
	*/
	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("Expression: %s\nColor and Opacity: %s\nDesired Scale: %s\nMirror Scale: %s\nAnimate: %s\nFrame Index: %d"),
			!Expression.IsNull() ? *Expression.GetAssetName() : TEXT("None"),
			*ColorAndOpacity.ToString(),
			*DesiredScale.ToString(),
			*MirrorScale.ToString(),
			bAnimate ? TEXT("true") : TEXT("false"),
			FrameIndex);
	}

	FORCEINLINE friend FArchive& operator<< (FArchive& Ar, FVisualImageInfo& ImageInfo)
	{
		Ar << ImageInfo.Expression
		   << ImageInfo.ColorAndOpacity
		   << ImageInfo.DesiredScale
		   << ImageInfo.MirrorScale
		   << ImageInfo.bAnimate
		   << ImageInfo.FrameIndex;

		return Ar;
	}

	FORCEINLINE bool operator== (const FVisualImageInfo& Other) const
	{
		return Expression == Other.Expression
			&& ColorAndOpacity == Other.ColorAndOpacity
			&& DesiredScale == Other.DesiredScale
			&& MirrorScale == Other.MirrorScale
			&& bAnimate == Other.bAnimate
			&& FrameIndex == Other.FrameIndex;
	}

	FORCEINLINE bool operator!= (const FVisualImageInfo& Other) const
	{
		return !(*this == Other);
	}
};

/**
* Widget that can visualize and animate sprite flipbooks.
* Visual image operates on UPaperFlipbook class 
* which is provided by Paper2D built-in plugin.
* Supports asynchronous loading of soft flipbooks.
*/
UCLASS(meta = (ToolTip = "Widget that can visualize and animate sprite flipbooks."))
class VISUALU_API UVisualImage : public UVisualImageBase
{
	GENERATED_BODY()

public:
	UVisualImage(const FObjectInitializer& ObjectInitializer);

	DECLARE_DYNAMIC_DELEGATE_RetVal(UPaperFlipbook*, FGetPaperFlipbook);

protected:
	/**
	* Paper flipbook to visualize.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = GetFlipbook)
	mutable TObjectPtr<UPaperFlipbook> Flipbook;

	/**
	* Delegate for property binding of the flipbook.
	* 
	* @seealso UVisualImage::ToFlipbook()
	*/
	UPROPERTY(meta = (ToolTip = "Implement property binding for Flipbook"))
	FGetPaperFlipbook FlipbookDelegate;

	/**
	* Color and opacity of the flipbook.
	* 
	* @note has no effect on widget appearance for invalid flipbook
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = GetColorAndOpacity, meta = (ToolTip = "Color and opacity of the flipbook"))
	FLinearColor ColorAndOpacity;

	/**
	* Delegate for property binding of the color and opacity.
	*/
	UPROPERTY(meta = (ToolTip = "Delegate for property binding of the color and opacity"))
	FGetLinearColor ColorAndOpacityDelegate;

	/**
	* Scale of the flipbook.
	* Must be larger than zero on both axes.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = GetDesiredScale, meta = (Delta = 0.25f, UIMin = 0, ClampMin = 0, AllowPreserveRatio))
	FVector2D DesiredScale;

	/**
	* Determines the orientation of the sprite on both axes.
	* Only -1 or 1 values are valid.
	* 
	* @note mirror scale does not affect the desired size of the widget
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = GetMirrorScale, meta = (ToolTip = "Determines the orientation of the sprite on both axes", Delta = 1.f, UIMin = -1, UIMax = 1, ClampMin = -1, ClampMax = 1))
	FVector2D MirrorScale;

	/**
	* Decides whether or not flipbook's animation sequence should be played.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = IsAnimated, meta = (ToolTip = "Decides whether or not flipbook's animation sequence should be played"))
	bool bAnimate;

	/**
	* Index of the flipbook's frame that must be rendered.
	* Ignored when UVisualImage::bAnimate is true.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = GetFrameIndex, meta = (ToolTip = "Index of the flipbook's frame that must be rendered", EditCondition = "!bAnimate", EditConditionHides))
	int32 FrameIndex;

	/**
	* Handle to the streamable flipbook.
	*/
	TSharedPtr<FStreamableHandle> FlipbookHandle;

public:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	/**
	* Applies received information to this image.
	* 
	* @param InInfo information for this visual image
	*/
	virtual void AssignVisualImageInfo(const FVisualImageInfo& InInfo) override;

	/**
	* Setter for UVisualController::bAnimate.
	* 
	* @param IsAnimated new flipbook animation state
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Either play animation sequence of the flipbook or use frame index to display certain frame"))
	void SetAnimate(bool IsAnimated);

	/**
	* Setter for UVisualImage::FrameIndex.
	* 
	* @param Index index of frame to display
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Sets index of flipbook frame that should be visualized"))
	void SetFrameIndex(int Index);

	/**
	* Synchronous setter for flipbook.
	* 
	* @param InFlipbook new flipbook to display
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Synchronously sets flipbook"))
	void SetFlipbook(UPaperFlipbook* InFlipbook);

	/**
	* Asynchronous setter for flipbook.
	*
	* @param InFlipbook new soft flipbook to display
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Asynchronously sets flipbook"))
	void SetFlipbookAsync(TSoftObjectPtr<UPaperFlipbook> InFlipbook);

	/**
	* Releases streamed flipbook.
	* 
	* @note does not affect already loaded flipbook
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Releases streamed flipbook"))
	void CancelAsyncLoad();

	/**
	* @return {@code true} when streaming is still in progress.
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Is flipbook loading"))
	bool IsFlipbookLoading() const;

	/**
	* @return {@code true} when streaming has finished.
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Is flipbook loaded"))
	bool IsFlipbookLoaded() const;

	/**
	* Setter for UVisualImage::ColorAndOpacity.
	* 
	* @param InColorAndOpacity new color and opacity of the flipbook
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Sets color and opacity of the flipbook"))
	void SetColorAndOpacity(const FLinearColor& InColorAndOpacity);

	/**
	* Setter for UVisualImage::DesiredScale.
	* 
	* @param InDesiredScale new desired scale of the flipbook
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Sets scale of flipbook"))
	void SetDesiredScale(const FVector2D& InDesiredScale);

	/**
	* Setter for UVisualImage::MirrorScale.
	* 
	* @param InMirrorScale new mirror scale of the flipbook
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Sets orientation of the flipbook on X and Y axes"))
	void SetMirrorScale(const FVector2D& InMirrorScale);

	/**
	* @return current flipbook
	*/
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Displayed flipbook"))
	FORCEINLINE UPaperFlipbook* GetFlipbook() const { return Flipbook; }

	
	/**
	* @return animation state of the flipbook
	*/
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Is animation sequence of the flipbook playing"))
	FORCEINLINE bool IsAnimated() const { return bAnimate; }

	/**
	* @return Index of the flipbook frame that should be displayed.
	*		  Meaningless when flipbook is animated.
	*/
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Index of the flipbook frame that should be displayed. Meaningless when flipbook is animated"))
	FORCEINLINE int32 GetFrameIndex() const { return FrameIndex; }

	/**
	* @return color and opacity of the flipbook
	*/
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Color and opacity of the flipbook"))
	FORCEINLINE FLinearColor GetColorAndOpacity() const { return ColorAndOpacity; }

	/**
	* @return desired scale of the flipbook
	*/
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Desired scale of the flipbook"))
	FORCEINLINE FVector2D GetDesiredScale() const { return DesiredScale; }

	/**
	* @return mirror scale of the flipbook
	*/
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Mirror scale of the flipbook"))
	FORCEINLINE FVector2D GetMirrorScale() const { return MirrorScale; }

	/**
	* @return sprite of the flipbook that is currently visualized by this widget
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Sprite of the flipbook that is currently visualized"))
	UPaperSprite* GetCurrentSprite() const;

protected:
#if WITH_EDITOR
	/**
	* Editor only.
	* 
	* @return category name in which this widget should be shown in the palette
	*/
	virtual const FText GetPaletteCategory() override;
#endif
	
	/**
	* Keeps C++ and derived blueprint classes synced in the widget designer.
	*/
	virtual void SynchronizeProperties() override;

	/**
	* @return underlying slate widget
	*/
	virtual TSharedRef<SWidget> RebuildWidget() override;
	
	/**
	* Asynchronously stream soft flipbook in the memory.
	* Will trigger assertion for invalid soft flipbook.
	* 
	* @param FlipbookToLoad soft flipbook to stream in memory
	* @param AfterLoadDelegate delegate to execute after loading is complete
	* @param Priority how urgent is loading of the soft flipbook
	* @return handle to the loaded flipbook
	*/
	TSharedPtr<FStreamableHandle> AsyncLoadFlipbook(TSoftObjectPtr<UPaperFlipbook> FlipbookToLoad, FStreamableDelegate AfterLoadDelegate, TAsyncLoadPriority Priority);

	/**
	* Converts flipbook attribute to the flipbook pointer.
	* Used to implement property binding of the flipbook.
	* 
	* @param InFlipbook flipbook attribute to convert
	* @return retrieved flipbook
	*/
	const UPaperFlipbook* ToFlipbook(TAttribute<UPaperFlipbook*> InFlipbook) const;

	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, ColorAndOpacity);

protected:
	/**
	* Underlying slate widget.
	* 
	* @seealso UVisualImage::RebuildWidget()
	*/
	TSharedPtr<SVisualImage> VisualImageSlate;

};
