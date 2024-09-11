// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Components/ScaleBox.h"
#include "VisualSocket.generated.h"

class SVisualSocket;

/**
* Zero-size scale box that acts as a socket for other widgets
* with additional functionality for UVisualImage.
* 
* @see SVisualSocket
*/
UCLASS(meta = (ToolTip = "Zero-size scale box that acts as a socket for other widgets with additional functionality for visual images."))
class VISUALU_API UVisualSocket : public UScaleBox
{
	GENERATED_BODY()

public:
	UVisualSocket(const FObjectInitializer& ObjectInitializer);

	/**
	* Setter for UVisualSocket::SocketOffset.
	* 
	* @param InSocketPosition new socket offset
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Socket", meta = (ToolTip = "Set offset position of child widget"))
	void SetSocketOffset(FVector2D InSocketPosition);

	/**
	* Setter for UVisualSocket::bAutoPositioning.
	* 
	* @param ShouldAutoPosition new auto positioning policy
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Socket", meta = (ToolTip = "Enable or disable auto positioning for visual image"))
	void SetAutoPositioning(bool ShouldAutoPosition);

	/**
	* Setter for UVisualSocket::ImageDesiredPosition.
	* 
	* @param Position new visual image desired position
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Socket", meta = (ToolTip = "Set position of child visual image"))
	void SetImageDesiredPosition(FVector2D Position);

	/**
	* @return socket offset of the child widget
	*/
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Socket offset"))
	FORCEINLINE FVector2D GetSocketOffset() const { return SocketOffset; }

	/**
	* @return auto positioning policy for child visual image
	*/
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "is child visual image auto positioned"))
	FORCEINLINE bool IsAutoPositioning() const { return bAutoPositioning; }

	/**
	* @return desired position of child visual image
	*/
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Desired position, in slate units, of child visual image"))
	FORCEINLINE FVector2D GetImageDesiredPosition() const { return ImageDesiredPosition; }

#if WITH_EDITOR
	/**
	* Editor only.
	*
	* @return category name in which this widget should be shown in the palette
	*/
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	/**
	* Keeps C++ and derived blueprint classes synced in the widget designer.
	*/
	virtual void SynchronizeProperties() override;

	/**
	* Releases memory allocated for slate widgets.
	*
	* @param bReleaseChildren should memory of child widgets be released
	*/
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	/**
	* @return underlying slate widget
	*/
	virtual TSharedRef<SWidget> RebuildWidget() override;

	/**
	* Offset of the child widget relative to the position of this socket.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = GetSocketOffset, Category = "Content Position", meta = (ToolTip = "Offset of the child widget relative to the position of this socket", EditCondition = "bAutoPositioning == false", Delta = 0.005f))
	FVector2D SocketOffset;

	/**
	* When active, socket offset would reflect UVisualSocket::ImageDesiredPosition.
	* Meaningless for child widget that is not visual image.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = IsAutoPositioning, Category = "Content Position", meta = (ToolTip = "When active, socket offset would reflect image desired position. Meaningless for child widget that is not visual image."))
	bool bAutoPositioning;

	/**
	* Desired position, in slate units, of the center of child visual image.
	* Meaningless for child widget that is not visual image.
	* 
	* @note UVisualSocket::bAutoPositioning must be enabled
	*       for this feature to work
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = GetImageDesiredPosition, Category = "Content Position", meta = (ToolTip = "Desired position of the image's center in size of the object to which this socket is applied.", EditCondition = "bAutoPositioning", EditConditionHides, UIMin = 0, ClampMin = 0))
	FVector2D ImageDesiredPosition;

private:
	/**
	* Underlying slate visual socket.
	* 
	* @see UVisualSocket::RebuildWidget()
	*/
	TSharedPtr<SVisualSocket> SlateVisualSocket;

};
