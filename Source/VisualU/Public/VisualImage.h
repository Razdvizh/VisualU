// Fill out your copyright notice in the Description page of Project Settings.

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

/// <summary>
/// Struct with all fields that affect appearance of <see cref="UVisualImage">Visual Image</see>.
/// </summary>
/// <remarks>
/// <see cref="UVisualImage">Visual Image</see> does not contain this struct and does not reflect it in any way.
/// </remarks>
USTRUCT(BlueprintType)
struct FVisualImageInfo : public FVisualInfo
{
	GENERATED_USTRUCT_BODY()

public:
	FVisualImageInfo() : ColorAndOpacity(ForceInit),
		DesiredScale(ForceInit),
		MirrorScale(ForceInit),
		bAnimate(false),
		FrameIndex(0)
	{

	}

	/// <see cref="UVisualImage::Flipbook"/>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info")
	TSoftObjectPtr<UPaperFlipbook> Expression;

	/// <see cref="UVisualImage::ColorAndOpacity"/>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info")
	FLinearColor ColorAndOpacity;

	/// <see cref="UVisualImage::DesiredScale"/>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info")
	FVector2D DesiredScale;

	/// <see cref="UVisualImage::MirrorScale"/>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info")
	FVector2D MirrorScale;

	/// <see cref="UVisualImage::bAnimate"/>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info")
	bool bAnimate;

	/// <see cref="UVisualImage::FrameIndex"/>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Info|Visual Image Info")
	int32 FrameIndex;

	virtual void Accept(IInfoAssignable* Visitor) const override
	{
		Visitor->AssignVisualImageInfo(*this);
	}

	/// <summary>
	/// Get a string representation of all fields.
	/// </summary>
	/// <returns>String of all fields</returns>
	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("Expression: %s Color and Opacity: %s Desired Scale: %s Mirror Scale: %s Animate: %s Frame Index: %d"),
			!Expression.IsNull() ? *Expression.GetAssetName() : TEXT("None"),
			*ColorAndOpacity.ToString(),
			*DesiredScale.ToString(),
			*MirrorScale.ToString(),
			bAnimate ? TEXT("true") : TEXT("false"),
			FrameIndex);
	}

	FORCEINLINE bool operator== (const FVisualImageInfo& Other)
	{
		if (Expression == Other.Expression
			&& ColorAndOpacity == Other.ColorAndOpacity
			&& DesiredScale == Other.DesiredScale
			&& MirrorScale == Other.MirrorScale
			&& bAnimate == Other.bAnimate
			&& FrameIndex == Other.FrameIndex)
		{
			return true;
		}

		return false;
	}

	FORCEINLINE bool operator!= (const FVisualImageInfo& Other)
	{
		return !(*this == Other);
	}
};

/// <summary>
/// Widget that can visualize sprite flipbooks either in animated or static state.
/// </summary>
/// <remarks>
/// UPaperFlipbook class is provided by Paper2D built-in plugin in order to reuse existing assets and make VisualU compatible
/// with normal Unreal pipeline for 2D games. Visual Image sets paper flipbook in a regular synchronous way unless <see cref="UVisualImage::SetFlipbookAsync"/>
/// is called.
/// </remarks>
UCLASS(meta = (ToolTip = "Widget that can visualize sprite flipbooks either in animated or static state."))
class VISUALU_API UVisualImage : public UVisualImageBase
{
	GENERATED_BODY()

public:
	UVisualImage(const FObjectInitializer& ObjectInitializer);

	DECLARE_DYNAMIC_DELEGATE_RetVal(UPaperFlipbook*, FGetPaperFlipbook);

protected:
	/// <summary>
	/// Paper flipbook to visualize.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = GetFlipbook)
	mutable UPaperFlipbook* Flipbook;

	/// <summary>
	/// Implement property binding for <see cref="UVisualImage::Flipbook"/>.
	/// </summary>
	UPROPERTY(meta = (ToolTip = "Implement property binding for Flipbook"))
	FGetPaperFlipbook FlipbookDelegate;

	/// <summary>
	/// Color and opacity of the <see cref="UVisualImage::Flipbook"/>.
	/// </summary>
	/// <remarks>
	/// Note: would not be rendered if <see cref="UVisualImage::Flipbook"/> is None.
	/// </remarks>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = GetColorAndOpacity, meta = (ToolTip = "Color and opacity of the Flipbook"))
	FLinearColor ColorAndOpacity;

	/// <summary>
	/// Implement property binding for <see cref="UVisualImage::ColorAndOpacity"/>.
	/// </summary>
	UPROPERTY(meta = (ToolTip = "Implement property binding for Color and Opacity"))
	FGetLinearColor ColorAndOpacityDelegate;

	/// <summary>
	/// Scale of Paper flipbook. Cannot be less than 0.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = GetDesiredScale, meta = (Delta = 0.25f, UIMin = 0, ClampMin = 0, AllowPreserveRatio))
	FVector2D DesiredScale;

	/// <summary>
	/// Determines the orientation of the sprite on both axes. Only -1 or 1 values are valid.
	/// </summary>
	/// <remarks>
	/// Mirror scale does not change the desired size.
	/// </remarks>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = GetMirrorScale, meta = (ToolTip = "Determines the orientation of the sprite on both axes", Delta = 1.f, UIMin = -1, UIMax = 1, ClampMin = -1, ClampMax = 1))
	FVector2D MirrorScale;

	/// <summary>
	/// Decides whether or not flipbook's animation sequence should be played.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = IsAnimated, meta = (ToolTip = "Decides whether or not flipbook should be played"))
	bool bAnimate;

	/// <summary>
	/// Index of the flipbook's frame that must be rendered. Ignored if <see cref="UVisualImage::bAnimate"/> is set to <c>true</c>.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", BlueprintGetter = GetFrameIndex, meta = (ToolTip = "Index of the flipbook's frame that must be rendered", EditCondition = "!bAnimate", EditConditionHides))
	int32 FrameIndex;

	/// <summary>
	/// Handle of paper flipbook resource.
	/// </summary>
	TSharedPtr<FStreamableHandle> FlipbookHandle;

public:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	virtual void AssignVisualImageInfo(const FVisualImageInfo& InInfo) override;

	/// <summary>
	/// Play animation sequence of paper flipbook or use <see cref="UVisualImage::FrameIndex"/> to display one frame.
	/// </summary>
	/// <param name="IsAnimated">Paper flipbook state</param>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Play animation sequence of the Flipbook or use FrameIndex to display one frame"))
	void SetAnimate(bool IsAnimated);

	/// <summary>
	/// Set index of paper flipbook frame that should be visualized.
	/// </summary>
	/// <param name="Index">Paper flipbook's frame index</param>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Set index of paper flipbook frame that should be visualized"))
	void SetFrameIndex(int Index);

	/// <summary>
	/// Synchronously set paper flipbook.
	/// </summary>
	/// <param name="InFlipbook">Flipbook to visualize</param>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Synchronously set paper flipbook"))
	void SetFlipbook(UPaperFlipbook* InFlipbook);

	/// <summary>
	/// Synchronously set paper flipbook.
	/// </summary>
	/// <param name="InFlipbook">Flipbook to visualize</param>
	void SetFlipbook(TObjectPtr<UPaperFlipbook> InFlipbook);

	/// <summary>
	/// Asynchronously set paper flipbook.
	/// </summary>
	/// <param name="InFlipbook">Flipbook to visualize</param>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Asynchronously set paper flipbook"))
	void SetFlipbookAsync(TSoftObjectPtr<UPaperFlipbook> InFlipbook);

	/// <summary>
	/// Release handle to paper flipbook resource.
	/// It would also cancel loading of the asset if it is in progress.
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Release handle to paper flipbook resource"))
	void CancelAsyncLoad();

	/// <returns><c>true</c> if paper flipbook resource is not loaded yet.</returns>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Is Flipbook resource loading"))
	bool IsFlipbookLoading() const;

	/// <returns><c>true</c> if paper flipbook resource is loaded.</returns>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Is Flipbook resource loaded"))
	bool IsFlipbookLoaded() const;

	/// <summary>
	/// Set color and opacity of paper flipbook.
	/// </summary>
	/// <param name="InColorAndOpacity">Desired color and opacity</param>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Set Color and Opacity of the Flipbook"))
	void SetColorAndOpacity(const FLinearColor& InColorAndOpacity);

	/// <summary>
	/// Set scale of paper flipbook.
	/// </summary>
	/// <param name="InDesiredScale">New scale of paper flipbook</param>
	/// <seealso cref="UVisualImage::DesiredScale"/>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Set scale of Flipbook"))
	void SetDesiredScale(const FVector2D& InDesiredScale);

	/// <summary>
	/// Set orientation of paper flipbook on X and Y axes.
	/// </summary>
	/// <param name="InMirrorScale">New orientation of paper flipbook</param>
	/// <seealso cref="UVisualImage::MirrorScale"/>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Set orientation of Flipbook on X and Y axes"))
	void SetMirrorScale(const FVector2D& InMirrorScale);

	/// <returns>Displayed paper flipbook</returns>
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Displayed paper flipbook"))
	FORCEINLINE UPaperFlipbook* GetFlipbook() const { return Flipbook; }

	/// <returns><c>true</c> if animation sequence of paper flipbook is playing</returns>
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Is animation sequence of Flipbook playing"))
	FORCEINLINE bool IsAnimated() const { return bAnimate; }

	/// <summary>
	/// Get index of paper flipbook frame that is currently displayed. Meaningless if <see cref="UVisualImage::bAnimate"/> is <c>true</c>.
	/// <returns>Paper flipbook's frame index</returns>
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Get index of paper flipbook frame that is currently displayed. Meaningless if bAnimate is set to true"))
	FORCEINLINE int GetFrameIndex() const { return FrameIndex; }

	/// <returns>Color and opacity of paper flipbook</returns>
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Color and opacity of paper flipbook"))
	FORCEINLINE FLinearColor GetColorAndOpacity() const { return ColorAndOpacity; }

	/// <returns>Scale of paper flipbook</returns>
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Scale of paper flipbook"))
	FORCEINLINE FVector2D GetDesiredScale() const { return DesiredScale; }

	/// <returns>Orientation of paper flipbook</returns>
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Orientation of paper flipbook"))
	FORCEINLINE FVector2D GetMirrorScale() const { return MirrorScale; }

	/// <summary>
	/// Get the visualized frame of paper flipbook.
	/// </summary>
	/// <returns>Currently displayed sprite</returns>
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Visual Image", meta = (ToolTip = "Get the visualized frame of paper flipbook."))
	UPaperSprite* GetCurrentSprite() const;

protected:
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	virtual void SynchronizeProperties() override;

	virtual TSharedRef<SWidget> RebuildWidget() override;

	/// <summary>
	/// Convertion for property binding implementation.
	/// </summary>
	/// <param name="InFlipbook">non-const paper flipbook slate attribute</param>
	/// <returns><c>const</c> paper flipbook</returns>
	const UPaperFlipbook* ToFlipbook(TAttribute<UPaperFlipbook*> InFlipbook) const;

	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, ColorAndOpacity);

protected:
	TSharedPtr<SVisualImage> VisualImageSlate;
};
