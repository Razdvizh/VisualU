// Copyright (c) 2024 Evgeny Shustov

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
 * Responsible for visualizing data from described by FScenario.
 * Renderer supports custom transitions between scene backgrounds that are
 * defined by FBackground::TransitionMaterial. Transition is driven by widget
 * animation and can be configured in UVisualUSettings class.
 * 
 * @note not blueprintable
 */
UCLASS(NotBlueprintable)
class VISUALU_API UVisualRenderer : public UUserWidget
{
	GENERATED_BODY()

public:
	UVisualRenderer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	* Assembles widgets for the scene.
	* 
	* @param Scene scene to render
	*/
	virtual void DrawScene(const FScenario* Scene);

	/**
	* @return {@code true} for ongoing visual transition between secenes
	* 
	* @see UVisualRenderer::TryDrawTransition()
	*/
	bool IsTransitionInProgress() const;

	/**
	* Tries to start and visualize transition material
	* with backgrounds from specified scenes.
	* Logical transition between scenes is handled by UVisualController.
	* 
	* @param From scene which is drawn currently
	* @param To scene that will be drawn after transition ends
	* @return {@code true} for started transition
	* 
	* @see UVisualRenderer::Transition
	*/
	bool TryDrawTransition(const FScenario* From, const FScenario* To);

	/**
	* Unconditionally stops ongoing transition.
	*/
	void ForceStopTransition();

protected:
	/**
	* Constructs underlying slate widget and widgets needed for drawing scenes.
	* Renderer always has a canvas panel to which visual sprites are added, and
	* one persistent visual image for scene background.
	* 
	* @return underlying slate widget
	* 
	* @see UVisualRenderer::Background
	*	   UVisualRenderer::Canvas
	*/
	virtual TSharedRef<SWidget> RebuildWidget() override;

	/**
	* Initializes renderer properties and transition animation.
	*/
	virtual void NativeOnInitialized() override;

	/**
	* Finishes transition.
	* 
	* @param Animation finished transition animation
	*/
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	/**
	* Iterates over each UVisualSprite in the canvas panel.
	* 
	* @param Action callable that will be executed for each sprite
	*/
	void ForEachSprite(TFunction<void(UVisualSprite* Sprite)> Action);

private:
	/**
	* Widget animation used to drive transition between scenes.
	* It can be configured in UVisualUSettings.
	* 
	* @note Transient. It will not be serialized.
	*/
	UPROPERTY(Transient)
	TObjectPtr<UWidgetAnimation> Transition;

	/**
	* Scene that will be displayed after transition ends.
	* It temporarily used during transition process.
	* It is invalid when transition does not occur.
	*/
	const FScenario* FinalScene;

	/**
	* Persistent widget that displays scene background.
	*/
	UPROPERTY()
	TObjectPtr<UBackgroundVisualImage> Background;

	/**
	* Persistent widget that holds background and all sprites of the scene.
	*/
	UPROPERTY()
	TObjectPtr<UCanvasPanel> Canvas;
	
};
