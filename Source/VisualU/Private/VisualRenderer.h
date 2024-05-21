// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VisualRenderer.generated.h"

class UBackgroundVisualImage;
class UCanvasPanel;

/**
 * Responsible for visualizing data from `FScenario`.
 */
UCLASS()
class VISUALU_API UVisualRenderer : public UUserWidget
{
	GENERATED_BODY()

public:
	UVisualRenderer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	/// <summary>
	/// Constructs <see cref="UVisualScene::Background"/> and <see cref="UVisualScene::Canvas"/>.
	/// </summary>
	/// <returns>Underlying slate widget</returns>
	/// \warning Do not add any widgets to the Widget tree.
	virtual TSharedRef<SWidget> RebuildWidget() override;

protected:
	/// <summary>
	/// Animation used to drive transitions between <see cref="FScenario">scenes</see>.
	/// </summary>
	/// <remarks>
	/// It can safely animate any parameters of the widgets or materials.
	/// Transition ends when this animation ends.
	/// </remarks>
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* Transition;

	/// <summary>
	/// Internal widget for scene background.
	/// </summary>
	UBackgroundVisualImage* Background;

	/// <summary>
	/// Internal widget for scene canvas panel.
	/// </summary>
	/// <remarks>
	/// All <see cref="UVisualSprite">Visual Sprites</see> and <see cref="UVisualScene::Background"/> are children of this panel widget.
	/// </remarks>
	UCanvasPanel* Canvas;
	
};
