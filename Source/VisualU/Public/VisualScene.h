// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetRegistryModule.h"
#include "Blueprint/UserWidget.h"
#include "Scenario.h"
#include "Engine/AssetManager.h"
#include "VisualScene.generated.h"

class UTextBlock;
class UCanvasPanel;
class UVisualImage;
class UObjectLibrary;
class UVisualSceneComponent;
class UWidgetBlueprintGeneratedClass;
class UMaterialInterface;
class UBackgroundVisualImage;
class UWidgetAnimation;
class UUMGSequencePlayer;
class UMaterialInterface;
struct FAssetData;
struct FTimerHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneStartEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneEndEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneLoadedEvent);

/// <summary>
/// Class that loads, visualizes and connects <see cref="FScenario">Scenarios</see> in the game.
/// </summary>
/// <remarks>
/// Visual Scene organizes nodes of <see cref="FScenario">Scenarios</see> into
/// the non-threaded tree structure, allowing gameplay designers create multiple ways to complete the game. 
/// Nodes of the tree are Data Tables, which should have at least one
/// <see cref="FScenario">Scenario</see>. Nodes are linked by <see cref="UVisualChoice">Visual Choice</see> sprite that **should** reside in the last
/// <see cref="FScenario">Scenario</see>. Note that such linking is not enforced: you may place additional <see cref="FScenario">Scenarios</see> after
/// the scene with <see cref="UVisualChoice">Visual Choice</see> or not add this sprite at all. Visual Scene operates on one node of tree at a time.
/// It loads resources if they weren't already loaded, constructs requested <see cref="UVisualSprite">sprites</see>, provides interfaces to the data of
/// the currently active <see cref="FScenario">Scenario</see>, and can iterate over them forward and backward or jump to the exhausted (previously seen), 
/// <see cref="FScenario">Scenario</see>. Visual Scene is designed to be efficient in both speed and memory, with priority given to the speed. There is no
/// limitations on amount of <see cref="UVisualSprite">Visual Sprites</see> or size of the node. 
/// Switching to next or previous <see cref="FScenario">Scenario</see> takes constant time (O(1)), random access of <see cref="FScenario">Scenario</see> takes
/// linear time (O(N)) and number of iterations grow with each new choice made by the player.
/// </remarks>
/// \image html Tree_structure.png
/// \warning Do not add any widgets to the Widget tree.
UCLASS(meta = (ToolTip = "Class that loads, visualizes and connects Scenarios in the game."))
class VISUALU_API UVisualScene : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// Static access to the first found <see cref="UVisualScene">Visual Scene</see>.
	/// </summary>
	/// <returns>first found <see cref="UVisualScene">Visual Scene</see></returns>
	/// <remarks>
	/// Essentially a strongly typed version of GetAllObjectsOfClass().
	/// </remarks>
	/// \attention This is an expensive operation, do not use every tick, etc.
	UFUNCTION(BlueprintCallable, Category = "Visual Scene", meta = (ToolTip = "Static access to the first found Visual Scene"))
	static UVisualScene* Get();

	/// <summary>
	/// Visual Scene can be constructed safely as any other <c>UUserWidget</c>.
	/// </summary>
	UVisualScene(const FObjectInitializer& ObjectInitializer);

	/// <summary>
	/// Releases the handle for the assets of the current scene.
	/// </summary>
	/// <remarks>
	/// If no other handles to these assets exist, assets will be unloaded from memory.
	/// </remarks>
	void CancelSceneLoading();

	/// <returns>Currently visualized <see cref="FScenario">scene</see></returns>
	const FScenario* GetCurrentScene() const;

	/// <returns>Currently visualized <see cref="FScenario"/></returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario", meta = (ToolTip = "Currently visualized Scenario"))
	const FScenario& GetCurrentScenario() const;

	/// <summary>
	/// Whether or not currently visualized scene has a <see cref="UVisualChoice">choice sprite</see>.
	/// </summary>
	/// <returns><c>true</c> if current scene has a <see cref="UVisualChoice">choice</see></returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario", meta = (ToolTip = "Whether or not currently visualized scene has a choice sprite"))
	bool IsWithChoice() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario")
	bool IsWithTransition() const;

	/// <returns><c>true</c> if loading of assets is still in progress</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario|Instantiation", meta = (ToolTip = "Is loading of assets is still in progress"))
	bool IsSceneLoading() const;

	/// <summary>
	/// Whether or not the <paramref name="Scene"/> is exhausted.
	/// </summary>
	/// <param name="Scene"><see cref="FScenario">Scene</see> to check</param>
	/// <returns><c>true</c> if <see cref="FScenario">Scene</see> is exhausted</returns>
	/// <remarks>
	/// <see cref="FScenario">Scene</see> is considered exhausted when it was already seen by the player.
	/// </remarks>
	/// <seealso cref="UVisualScene::ExhaustedScenes"/>
	bool IsSceneExhausted(const FScenario* Scene) const;

	/// <summary>
	/// Whether or not the <paramref name="Scenario"/> is exhausted.
	/// </summary>
	/// <param name="Scenario"><see cref="FScenario">Scenario</see> to check</param>
	/// <returns><c>true</c> if <see cref="FScenario">Scenario</see> is exhausted</returns>
	/// <remarks>
	/// <see cref="FScenario">Scenario</see> is considered exhausted when it was already seen by the player.
	/// </remarks>
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Visual Scene|Scenario", meta = (ToolTip = "Whether or not provided Scenario is exhausted"))
	bool IsScenarioExhausted(const FScenario& Scenario) const;

	/// <returns><c>true</c> if there is a <see cref="FScenario">scene</see> in front of the current one.</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control", meta = (ToolTip = "Is there a Scene in front of the current one"))
	bool CanAdvanceScene() const;
	
	/// <returns><c>true</c> if there is a <see cref="FScenario">scene</see> behind the current one.</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control", meta = (ToolTip = "Is there a Scene behind the current one"))
	bool CanRetractScene() const;

	/// <returns><c>true</c> if assets of the current <see cref="FScenario">scene</see> are already loaded</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario|Instantiation", meta = (ToolTip = "Are assets of the current Scene loaded"))
	bool IsSceneLoaded() const;

	/// <summary>
	/// Visualize the next <see cref="FScenario">scene</see> in the node.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control", meta = (ToolTip = "Visualizes the next Scene in the node"))
	void ToNextScene();

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control")
	void TransitionToNextScene();

	void TransitionToNextScene(UWidgetAnimation* DrivingAnim);

	/// <summary>
	/// Visualize the previous <see cref="FScenario">scene</see> in the node.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control", meta = (ToolTip = "Visualizes the next Scene in the node"))
	void ToPreviousScene();

	/// <summary>
	/// Jump to any exhausted scene.
	/// </summary>
	/// <param name="Scene"><see cref="FScenario">scene</see> to visualize</param>
	/// <returns><c>true</c> if scene was visualized</returns>
	/// \warning Only use this method on <see cref="FScenario">scenes</see> that was already seen by the player.
	bool ToScene(const FScenario* Scene);

	const FScenario* GetSceneAt(int32 Index);

	/// <summary>
	/// Jump to any exhausted scene.
	/// </summary>
	/// <param name="Scene"><see cref="FScenario">scenario</see> to visualize</param>
	/// <returns><c>true</c> if scenario was visualized</returns>
	/// \warning Only use this method on <see cref="FScenario">scenarios</see> that was already seen by the player.
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control", meta = (ToolTip = "Jump to any exhausted Scene"))
	bool ToScenario(const FScenario& Scenario);

	/// <summary>
	/// Sets provided node as active and visualizes the first <see cref="FScenario">Scenario in the node</see>
	/// </summary>
	/// <param name="Rows"><see cref="FScenario">Scenes</see> of the node</param>
	/// <remarks>
	/// <see cref="UVisualScene">Visual Scene</see> operates on only one node at a time, 
	/// information about previous nodes is saved on <see cref="FScenario">Scenario level</see>
	/// </remarks>
	/// \attention Avoid passing the same node as the currently active one 
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control", meta = (ToolTip = "Sets provided node as active"))
	void ToNode(const UDataTable* NewNode);

	UPROPERTY(BlueprintAssignable, Category = "Visual Scene|Events")
	FOnSceneStartEvent OnSceneStart;

	DECLARE_MULTICAST_DELEGATE(FOnNativeSceneStartEvent);
	FOnNativeSceneStartEvent OnNativeSceneStart;

	UPROPERTY(BlueprintAssignable, Category = "Visual Scene|Events")
	FOnSceneEndEvent OnSceneEnd;

	DECLARE_MULTICAST_DELEGATE(FOnNativeSceneEndEvent);
	FOnNativeSceneEndEvent OnNativeSceneEnd;

	UPROPERTY(BlueprintAssignable, Category = "Visual Scene|Events")
	FOnSceneLoadedEvent OnSceneLoaded;

	DECLARE_MULTICAST_DELEGATE(FOnNativeSceneLoadedEvent);
	FOnNativeSceneLoadedEvent OnNativeSceneLoaded;

	DECLARE_MULTICAST_DELEGATE(FOnNativeSceneTransitionEndedEvent);
	FOnNativeSceneTransitionEndedEvent OnNativeSceneTransitionEnded;

protected:
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

	/// <summary>
	/// Handle for assets of the scene that are loaded into the memory.
	/// </summary>
	TSharedPtr<FStreamableHandle> ActiveSceneHandle;

	/// <summary>
	/// Currently active Data table with scenes.
	/// </summary>
	/// <seealso cref="FScenario"/>
	TArray<FScenario*> Node;

	/// <summary>
	/// Index of the scene in the node.
	/// </summary>
	int32 SceneIndex;

	/// <summary>
	/// Scenes that has been already seen in the game.
	/// Only the scenes **past** the currently active scene are exhausted.
	/// </summary>
	/// <remarks>
	/// Must be used only as a stack data structure, because scene might be restored and removed from the stack.
	/// In other words, when player returns back to the scene that was seen previously, or was exhausted, that scene
	/// is not considered exhausted anymore untill the player advances forward again.
	/// </remarks>
	TArray<FScenario*> ExhaustedScenes;

protected:
	/// <summary>
	/// Constructs <see cref="UVisualScene::Background"/> and <see cref="UVisualScene::Canvas"/>.
	/// </summary>
	/// <returns>Underlying slate widget</returns>
	/// \warning Do not add any widgets to the Widget tree.
	virtual TSharedRef<SWidget> RebuildWidget() override;

	/// <summary>
	/// Acquires the first node and initializes fields.
	/// </summary>
	virtual void NativeOnInitialized() override;

	/// <summary>
	/// Loads assets and constructs the first <see cref="FScenario">scene</see>.
	/// </summary>
	/// <seealso cref="UVisualUSettings::FirstDataTable"/>
	virtual void NativeConstruct() override;

	/// <summary>
	/// Constructs <see cref="UVisualSprite">Visual Sprites</see> and processes all data from <paramref name="Scene"/>.
	/// </summary>
	/// <param name="Scene">Scenario to construct</param>
	/// <remarks>
	/// Assumes that assets of <paramref name="Scene"/> are already loaded.
	/// </remarks>
	virtual void ConstructScene(const FScenario* Scene);

	/// <summary>
	/// Loads assets of the <paramref name="Scene"/> into the memory.
	/// </summary>
	/// <param name="Scene">Scenario whose assets should be loaded</param>
	/// <param name="AfterLoadDelegate">Delegate that would be executed after assets are loaded</param>
	/// <param name="UnloadScene">Whether or not to release the handle. <c>true</c> by default</param>
	virtual void LoadScene(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate, bool UnloadScene = true);

	/// <summary>
	/// Removes all <see cref="UVisualSprite">Visual Sprites</see> from <see cref="UVisualScene::Canvas"/>.
	/// </summary>
	/// <returns><c>true</c> if at least one sprite was removed</returns>
	bool ClearSprites();

	void PlayTransition(UWidgetAnimation* DrivingAnim);

	void StopTransition() const;

private:
	void ConstructScene();

	void LoadAndConstruct();

	void SetCurrentScene(const FScenario* Scene);

	FTimerHandle TransitionHandle;

	FTimerDelegate OnTransitionEnd;
};
