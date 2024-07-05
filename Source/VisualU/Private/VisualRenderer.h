// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Scenario.h"
#include "VisualRenderer.generated.h"

class UBackgroundVisualImage;
class UVisualSprite;
class UCanvasPanel;
class UWidgetAnimation;
class UMaterialParameterCollection;

/**
 * Responsible for visualizing data from `FScenario`.
 */
UCLASS(NotBlueprintable)
class VISUALU_API UVisualRenderer : public UUserWidget
{
	GENERATED_BODY()

public:
	UVisualRenderer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void DrawScene(const FScenario* Scene);

	bool IsTransitionInProgress() const;

	bool TryDrawTransition(const FScenario* From, const FScenario* To);

	void ForceStopTransition();

protected:
	/// <summary>
	/// Constructs <see cref="UVisualScene::Background"/> and <see cref="UVisualScene::Canvas"/>.
	/// </summary>
	/// <returns>Underlying slate widget</returns>
	/// \warning Do not add any widgets to the Widget tree.
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeOnInitialized() override;

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	void ForEachSprite(TFunction<void(UVisualSprite* Sprite)> Action);

private:
	/// <summary>
	/// Animation used to drive transitions between <see cref="FScenario">scenes</see>.
	/// </summary>
	/// <remarks>
	/// It can safely animate any parameters of the widgets or materials.
	/// Transition ends when this animation ends.
	/// </remarks>
	UPROPERTY(Transient)
	TObjectPtr<UWidgetAnimation> Transition;

	const FScenario* FinalScene;

	/// <summary>
	/// Internal widget for scene background.
	/// </summary>
	UPROPERTY()
	TObjectPtr<UBackgroundVisualImage> Background;

	/// <summary>
	/// Internal widget for scene canvas panel.
	/// </summary>
	/// <remarks>
	/// All <see cref="UVisualSprite">Visual Sprites</see> and <see cref="UVisualScene::Background"/> are children of this panel widget.
	/// </remarks>
	UPROPERTY()
	TObjectPtr<UCanvasPanel> Canvas;
	
};
