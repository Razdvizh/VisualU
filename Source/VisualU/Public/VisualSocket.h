// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ScaleBox.h"
#include "VisualSocket.generated.h"

class SVisualSocket;

/// <summary>
/// Zero-size scale box that acts as a socket for other widgets with additional functionality for <see cref="UVisualImage">Visual Images</see>.
/// </summary>
UCLASS(meta = (ToolTip = "Zero-size scale box that acts as a socket for other widgets with additional functionality for Visual Images."))
class VISUALU_API UVisualSocket : public UScaleBox
{
	GENERATED_BODY()

public:
	UVisualSocket(const FObjectInitializer& ObjectInitializer);

	/// <summary>
	/// Set offset position of child widget.
	/// </summary>
	/// <param name="InSocketPosition">New socket offset</param>
	UFUNCTION(BlueprintCallable, Category = "Visual Socket", meta = (ToolTip = "Set offset position of child widget."))
	void SetSocketOffset(FVector2D InSocketPosition);

	/// <summary>
	/// Enable or disable auto positioning for <see cref="UVisualImage">Visual Images</see>.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Visual Socket", meta = (ToolTip = "Enable or disable auto positioning for Visual Images."))
	void SetAutoPositioning(bool ShouldAutoPosition);

	/// <param name="Position">Position of child <see cref="UVisualImage">Visual Image</see></param>
	UFUNCTION(BlueprintCallable, Category = "Visual Socket", meta = (ToolTip = "Set position of child Visual Image"))
	void SetImageDesiredPosition(FVector2D Position);

	/// <returns>Socket offset</returns>
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Socket offset"))
	FORCEINLINE FVector2D GetSocketOffset() const { return SocketOffset; }

	/// <returns><c>true</c> if child <see cref="UVisualImage">Visual Image</see> is auto positioned</returns>
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "true if child Visual Image is auto positioned"))
	FORCEINLINE bool IsAutoPositioning() const { return bAutoPositioning; }

	/// <returns>Position, in slate units, of child <see cref="UVisualImage">Visual Image</see></returns>
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Position, in slate units, of child Visual Image"))
	FORCEINLINE FVector2D GetImageDesiredPosition() const { return ImageDesiredPosition; }

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	/// <summary>
	/// Offset of child widget position.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = GetSocketOffset, Category = "Content Position", meta = (ToolTip = "Offset of child widget position", EditCondition = "bAutoPositioning == false", Delta = 0.005f))
	FVector2D SocketOffset;

	/// <summary>
	/// If active, socket offset would reflect image desired position. Note: this option only works for <see cref="UVisualImage">Visual Images</see>.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = IsAutoPositioning, Category = "Content Position", meta = (ToolTip = "If active, socket offset would reflect image desired position. Note that this option only works for visual images."))
	bool bAutoPositioning;

	/// <summary>
	/// Desired position of the <see cref="UVisualImage">Visual Image</see> center in size of the object to which this socket is applied.
	/// <see cref="UVisualSocket::bAutoPositioning"/> must be enabled for this feature to work.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = GetImageDesiredPosition, Category = "Content Position", meta = (ToolTip = "Desired position of the image's center in size of the object to which this socket is applied.", EditCondition = "bAutoPositioning", EditConditionHides, UIMin = 0, ClampMin = 0))
	FVector2D ImageDesiredPosition;

	virtual void SynchronizeProperties() override;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TSharedPtr<SVisualSocket> SlateVisualSocket;
};
