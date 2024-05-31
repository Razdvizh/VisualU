// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Scenario.h"
#include "Templates/SubclassOf.h"
#include "VisualController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneStart);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneEnd);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneLoaded);

class UVisualRenderer;
class APlayerController;
struct FStreamableHandle;

/**
* Describes direction in which scenes "flow" - current scene is expected to change either to the next(forward) or previous(backward) scene in the node.
*/
enum class EVisualControllerNodeDirection : int8
{
	Backward = -1,
	Forward = 1
};

typedef EVisualControllerNodeDirection ENodeDirection;

/**
 * Controls the flow of `FScenario`'s and provides interface for others to observe it.
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class VISUALU_API UVisualController : public UObject
{
	GENERATED_BODY()

public:
	UVisualController();

	/// <returns>Currently visualized <see cref="FScenario">scene</see></returns>
	const FScenario* GetCurrentScene() const;

	/// <returns>Currently visualized <see cref="FScenario"/></returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Scenario", meta = (ToolTip = "Currently visualized Scenario"))
	const FScenario& GetCurrentScenario() const;

	/// <summary>
	/// Whether or not currently visualized scene has a <see cref="UVisualChoice">choice sprite</see>.
	/// </summary>
	/// <returns><c>true</c> if current scene has a <see cref="UVisualChoice">choice</see></returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Scenario", meta = (ToolTip = "Whether or not currently visualized scene has a choice sprite"))
	bool IsWithChoice() const;

	/// <summary>
	/// Releases the handle for the assets of the current scene.
	/// </summary>
	/// <remarks>
	/// If no other handles to these assets exist, assets will be unloaded from memory.
	/// </remarks>
	void CancelNextScene();

	/// <returns><c>true</c> if loading of assets is still in progress</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Scenario|Instantiation", meta = (ToolTip = "Is loading of assets is still in progress"))
	bool IsCurrentSceneLoading() const;

	/// <returns><c>true</c> if assets of the current <see cref="FScenario">scene</see> are already loaded</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Scenario|Instantiation", meta = (ToolTip = "Are assets of the current Scene loaded"))
	bool IsCurrentSceneLoaded() const;

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
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Visual Controller|Scenario", meta = (ToolTip = "Whether or not provided Scenario is exhausted"))
	bool IsScenarioExhausted(const FScenario& Scenario) const;

	/// <returns><c>true</c> if there is a <see cref="FScenario">scene</see> in front of the current one.</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Is there a Scene in front of the current one"))
	bool CanAdvanceScene() const;

	/// <returns><c>true</c> if there is a <see cref="FScenario">scene</see> behind the current one.</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Is there a Scene behind the current one"))
	bool CanRetractScene() const;

	/// <summary>
	/// Visualize the next <see cref="FScenario">scene</see> in the node.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Visualizes the next Scene in the node"))
	void ToNextScene();

	/// <summary>
	/// Visualize the previous <see cref="FScenario">scene</see> in the node.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Visualizes the next Scene in the node"))
	void ToPreviousScene();

	/// <summary>
	/// Jump to any exhausted scene.
	/// </summary>
	/// <param name="Scene"><see cref="FScenario">scene</see> to visualize</param>
	/// <returns><c>true</c> if scene was visualized</returns>
	/// \warning Only use this method on <see cref="FScenario">scenes</see> that was already seen by the player.
	bool ToScene(const FScenario* Scene);

	/// <summary>
	/// Get scene in the node.
	/// </summary>
	/// <param name="Index">Index of the scene</param>
	/// <returns>valid Scenario</returns>
	const FScenario* GetSceneAt(int32 Index);

	/// <summary>
	/// Jump to any exhausted scene.
	/// </summary>
	/// <param name="Scene"><see cref="FScenario">scenario</see> to visualize</param>
	/// <returns><c>true</c> if scenario was visualized</returns>
	/// \warning Only use this method on <see cref="FScenario">scenarios</see> that was already seen by the player.
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Jump to any exhausted Scene"))
	bool ToScenario(const FScenario& Scenario);

	/// <summary>
	/// Sets provided node as active and visualizes the first <see cref="FScenario">Scenario in the node</see>
	/// </summary>
	/// <param name="Rows"><see cref="FScenario">Scenes</see> of the node</param>
	/// <remarks>
	/// <see cref="UVisualScene">Visual Controller</see> operates on only one node at a time, 
	/// information about previous nodes is saved on <see cref="FScenario">Scenario level</see>
	/// </remarks>
	/// \attention Avoid passing the same node as the currently active one 
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Sets provided node as active"))
	void ToNode(const UDataTable* NewNode);

	/**
	* Construct underlying widget and add it to the player screen. Will show currently selected scene.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void Visualize(APlayerController* OwningController, int32 ZOrder = 0);

	/**
	* Destroy underlying widget.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void Discard();

	/**
	* Make underlying widget visible.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void Show();

	/**
	* Make underlying widget collapsed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void Hide();

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Async", meta = (DisplayName = "SetNumScenariosToLoad"))
	void SetNumScenesToLoad(int32 Num);

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Async", meta = (DisplayName = "GetNumScenariosToLoad"))
	FORCEINLINE int32 GetNumScenesToLoad() const { return ScenesToLoad; };

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Transition")
	void SetPlayTransitions(bool bShouldPlay);

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Transition")
	FORCEINLINE bool PlaysTransitions() const { return bPlayTransitions; }

	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnSceneStart OnSceneStart;

	DECLARE_MULTICAST_DELEGATE(FOnNativeSceneStart);
	FOnNativeSceneStart OnNativeSceneStart;

	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnSceneEnd OnSceneEnd;

	DECLARE_MULTICAST_DELEGATE(FOnNativeSceneEnd);
	FOnNativeSceneEnd OnNativeSceneEnd;

	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnSceneLoaded OnSceneLoaded;

	DECLARE_MULTICAST_DELEGATE(FOnNativeSceneLoaded);
	FOnNativeSceneLoaded OnNativeSceneLoaded;

	TSubclassOf<UVisualRenderer> RendererClass;

protected:
	/// <summary>
	/// Loads assets of the <paramref name="Scene"/> into the memory.
	/// </summary>
	/// <param name="Scene">Scenario whose assets should be loaded</param>
	/// <param name="AfterLoadDelegate">Delegate that would be executed after assets are loaded</param>
	TSharedPtr<FStreamableHandle> LoadSceneAsync(const FScenario* Scene);

	TSharedPtr<FStreamableHandle> LoadScene(const FScenario* Scene);

	void PrepareScenes(ENodeDirection Direction = ENodeDirection::Forward);

	/**
	* Requests Renderer to display transition animation.
	*/
	bool TryPlayTransition(const FScenario* From, const FScenario* To);

private:
	void SetCurrentScene(const FScenario* Scene);

	void AssertNextSceneLoad(ENodeDirection Direction = ENodeDirection::Forward);

private:
	UPROPERTY()
	UVisualRenderer* Renderer;

	/// todo: Make this NextSceneHandle, active scene assets will be always loaded as there is hard references to them, while the NextSceneHandle might be
	/// useful for various tasks, transition for example.
	/// <summary>
	/// Handle for assets of the scene that are loaded into the memory.
	/// </summary>
	TSharedPtr<FStreamableHandle> NextSceneHandle;

	/// <summary>
	/// Currently active Data table with scenes.
	/// </summary>
	/// <seealso cref="FScenario"/>
	TArray<FScenario*> Node;

	/// <summary>
	/// Index of the scene in the node.
	/// </summary>
	int32 SceneIndex;

	/*
	* How many following scenarios will be loaded. 
	* e.g. for value of 5: 5(or how much is left in the node) scenarios after current scene will be loaded asynchronously.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Controller|Async", meta = (DisplayName = "ScenariosToLoad", AllowPrivateAccess = true))
	int32 ScenesToLoad;

	/*
	* Handles for resources of the following scenarios.
	* Number of elements always less or equal to ScenesToLoad.
	*/
	TQueue<TSharedPtr<FStreamableHandle>> SceneHandles;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Controller|Transition", meta = (AllowPrivateAccess = true, ToolTip = "Should Visual Controller attempt to play transition between scenarios"))
	bool bPlayTransitions;

};
