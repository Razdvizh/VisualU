// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ScaleBox.h"
#include "VisualSocket.generated.h"

class SVisualSocket;
/**
 * Zero-size scale box that acts as a socket for images
 */
UCLASS(ClassGroup = UI, meta = (Category = "Visual U"))
class VISUALU_API UVisualSocket : public UScaleBox
{
	GENERATED_BODY()

public:
	UVisualSocket(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Visual Socket")
	void SetSocketOffset(FVector2D InSocketPosition);

	UFUNCTION(BlueprintCallable, Category = "Visual Socket")
	void SetAutoPositioning(bool ShouldAutoPosition);

	UFUNCTION(BlueprintCallable, Category = "Visual Socket")
	void SetImageDesiredPosition(FVector2D Position);

	UFUNCTION(BlueprintGetter)
	FORCEINLINE FVector2D GetSocketOffset() const { return SocketOffset; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool IsAutoPositioning() const { return bAutoPositioning; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE FVector2D GetImageDesiredPosition() const { return ImageDesiredPosition; }

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = GetSocketOffset, meta = (Category = "Content Position", EditCondition = "bAutoPositioning == false", Delta = 0.005f))
	FVector2D SocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = IsAutoPositioning, meta = (Category = "Content Position", ToolTip = "If active, socket offset would reflect image desired position. Note that this option only works for images and visual images."))
	bool bAutoPositioning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = GetImageDesiredPosition, meta = (Category = "Content Position", ToolTip = "Desired position of the image's center in size of the object to which this socket is applied.", EditCondition = "bAutoPositioning", EditConditionHides, UIMin = 0, ClampMin = 0))
	FVector2D ImageDesiredPosition;

	virtual void SynchronizeProperties() override;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TSharedPtr<SVisualSocket> SlateVisualSocket;
};
