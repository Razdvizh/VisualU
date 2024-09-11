// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VisualImage.h"
#include "VisualSprite.generated.h"

class UWidgetBlueprintGeneratedClass;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpriteDisappear);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpriteAppear);

/**
* Class that represents a sprite that will be visualized by UVisualRenderer.
* Sprite in VisualU is a widget which appearance is governed by UVisualImage
* and lifetime is bound to the lifetime of the FScenario that is currently 
* active in UVisualController. It is represented by FSprite struct
* in scene visual info. Visual sprite can contain scripts of a normal widget
* and can be extended as some specific kind of sprite.
*/
UCLASS(meta = (ToolTip = "Class that represents a sprite that will be visualized by visual renderer."))
class VISUALU_API UVisualSprite : public UUserWidget
{
	GENERATED_BODY()
		
public:
	UVisualSprite(const FObjectInitializer& ObjectInitializer);

	/**
	* Releases memory allocated for slate widgets.
	*
	* @param bReleaseChildren should memory of child widgets be released
	*/
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	/**
	* Assigns provided information to visual images of this sprite.
	* Default implementation will assign first image info
	* to the first visual image, which is the one closest
	* to the root of the {@code Widget tree}.
	* (In Widget designer, the top most visual image will be the first,
	* the down most will be the last). It will continue assigning fields
	* in this manner until there is no visual images left in the tree.
	* Any extra image info will be ignored.
	* \image html AssignVisualInfo_default.png
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Sprite", meta = (ToolTip = "Assigns provided information to visual images of this sprite."))
	virtual void AssignSpriteInfo(const TArray<FVisualImageInfo>& InInfo);

	/**
	* Called when sprite is removed from canvas in the renderer.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Visual Sprite|Events")
	FOnSpriteDisappear OnSpriteDisappear;

	/**
	* Called when sprite is being drawn by the renderer.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Visual Sprite|Events")
	FOnSpriteAppear OnSpriteAppear;
};
