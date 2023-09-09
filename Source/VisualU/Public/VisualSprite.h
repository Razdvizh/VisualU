// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VisualImage.h"
#include "VisualSprite.generated.h"

class UWidgetBlueprintGeneratedClass;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpriteBeginRemoveEvent);

/// <summary>
/// Base class for widgets that represent sprites that will be visualized by <see cref="UVisualScene">Visual Scene</see>.
/// </summary>
/// <remarks>
/// Visual form of the classic Visual Novel sprite. Can represent NPC, an interactive object, etc.
/// Visual Sprite contains any amount of <see cref="UVisualImage">Visual Images</see> needed to display a sprite,
/// which can be altered by <see cref="UVisualScene">Visual Scene</see> using information provided in the different <see cref="FScenario">Scenarios</see>.
/// It can contain any functionality of the normal widget and can be extended as some specific kind of the sprite.
/// The arrangement of <see cref="UVisualImage">Visual Images</see> is meant to be done inside blueprint class.
UCLASS(meta = (ToolTip = "Base class for widgets that represent sprites that will be visualized by Visual Scene"))
class VISUALU_API UVisualSprite : public UUserWidget
{
	GENERATED_BODY()
		
public:
	UVisualSprite(const FObjectInitializer& ObjectInitializer);

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	///\todo add the same member function but for UViusalSocket
	/// <summary>
	/// Set the desired fields for the <see cref="UVisualImage">Visual images</see> that this Visual Sprite has.
	/// </summary>
	/// <param name="InInfo">Fields to assign</param>
	/// <remarks>
	/// Default implementation will assign first <c>FVisualImageInfo</c> to the first <see cref="UVisualImage">Visual Image</see>, which is the one closest
	/// to the root of the <c>WidgetTree</c>. (In Blueprint editor, the top most <see cref="UVisualImage">Visual Image</see> will be the first, 
	/// the down most will be the last). It will continue assigning fields in this manner until there is no <see cref="UVisualImage">Visual images</see> 
	/// in the tree. Any extra <c>FVisualImageInfo</c>s will be ignored.
	/// </remarks>
	/// \image html AssignSpriteInfo_default.png
	UFUNCTION(BlueprintCallable, Category = "Visual Sprite", meta = (ToolTip = "Set the desired fields for the Visual images that this Visual Sprite has."))
	virtual void AssignSpriteInfo(const TArray<FVisualImageInfo>& InInfo);

	UPROPERTY(BlueprintAssignable, Category = "Visual Sprite|Events")
	FOnSpriteBeginRemoveEvent OnSpriteBeginRemove;
};
