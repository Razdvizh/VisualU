// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Scenario.h"
#include "Templates/SubclassOf.h"
#include "Containers/Deque.h"
#include "Async/AsyncWork.h"
#include "VisualController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneStart);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneEnd);

class UVisualRenderer;
class APlayerController;
struct FStreamableHandle;

/**
* !!! DO NOT USE OUTSIDE OF VISUAL CONTROLLER FUNCTIONS !!!
* Describes direction in which scenes move - current scene is expected to change either to the next(forward) or previous(backward) scene in the node.
* None is an internal value and must never be used.
* @note Declared using a namespace in order to support negative enum values. 
* It will break a lot of things in blueprints if this enum is used as a variable or in conversions, etc.
*/
UENUM(BlueprintType)
namespace EVisualControllerDirection
{
	enum Type : int8
	{
		Backward = -1,
		None = 0 UMETA(Hidden),
		Forward = 1
	};
}

namespace UE
{
	namespace VisualU
	{
		namespace Private
		{
			class FFastMoveAsyncWorker : public FNonAbandonableTask
			{
			public:
				FFastMoveAsyncWorker(UVisualController* Controller,
					EVisualControllerDirection::Type Direction,
					bool PlayedTransitions)
					: VisualController(Controller),
					ControllerDirection(Direction),
					bPlayedTransitions(PlayedTransitions)
				{
				};

				void DoWork();

				FORCEINLINE TStatId GetStatId() const
				{
					RETURN_QUICK_DECLARE_CYCLE_STAT(FFastMoveAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
				}

			private:
				UVisualController* VisualController;

				EVisualControllerDirection::Type ControllerDirection;

				bool bPlayedTransitions;

			};

			class FFastMoveAsyncTask : public FAsyncTask<FFastMoveAsyncWorker>
			{
			public:
				FFastMoveAsyncTask(UVisualController* Controller,
					EVisualControllerDirection::Type Direction,
					bool PlayedTransitions)
					: FAsyncTask(Controller, Direction, PlayedTransitions)
				{
				};

			};
		}
	}
}

/**
 * Controls the flow of `FScenario`'s and provides interface for others to observe it.
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, Within = PlayerController)
class VISUALU_API UVisualController : public UObject
{
	GENERATED_BODY()

public:
	UVisualController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginDestroy() override;

	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

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
	bool RequestNextScene();

	/// <summary>
	/// Visualize the previous <see cref="FScenario">scene</see> in the node.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Visualizes the previous Scene in the node"))
	bool RequestPreviousScene();

	/// <summary>
	/// Jump to any exhausted scene.
	/// </summary>
	/// <param name="Scene"><see cref="FScenario">scene</see> to visualize</param>
	/// <returns><c>true</c> if scene was visualized</returns>
	/// \warning Only use this method on <see cref="FScenario">scenes</see> that was already seen by the player.
	bool RequestScene(const FScenario* Scene);

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

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	void RequestFastMove(EVisualControllerDirection::Type Direction = EVisualControllerDirection::Forward);

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	void CancelFastMove();

	/**
	* Construct renderer if necessary and add it to the player screen. Will show currently selected scene.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void Visualize(TSubclassOf<UVisualRenderer> RendererClass, int32 ZOrder = 0);

	/**
	* Destroy underlying renderer.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void Discard();

	/**
	* Change renderer visibility.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void SetVisibility(ESlateVisibility Visibility);

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Async", meta = (DisplayName = "SetNumScenariosToLoad"))
	void SetNumScenesToLoad(int32 Num);

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Async", meta = (DisplayName = "GetNumScenariosToLoad"))
	FORCEINLINE int32 GetNumScenesToLoad() const { return ScenesToLoad; };

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Transition")
	void ShouldPlayTransitions(bool bShouldPlay);

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Transition")
	FORCEINLINE bool PlaysTransitions() const { return bPlayTransitions; }

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow Control")
	bool IsCurrentScenarioHead() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	FORCEINLINE bool IsFastMoving() const { return bIsFastMoving; }
	
	/**
	* Called when Visual Controller has switched to a different scenario.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnSceneStart OnSceneStart;

	DECLARE_MULTICAST_DELEGATE(FOnNativeSceneStart);
	/**
	* Called when Visual Controller has switched to a different scenario.
	*/
	FOnNativeSceneStart OnNativeSceneStart;

	/**
	* Called when Visual Controller begins switching to a different scenario.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnSceneEnd OnSceneEnd;

	DECLARE_MULTICAST_DELEGATE(FOnNativeSceneEnd);
	/**
	* Called when Visual Controller begins switching to a different scenario.
	*/
	FOnNativeSceneEnd OnNativeSceneEnd;

protected:
	/// <summary>
	/// Asynchronously load assets of the <paramref name="Scene"/> into the memory.
	/// </summary>
	/// <param name="Scene">Scenario whose assets should be loaded</param>
	/// <param name="AfterLoadDelegate">Delegate that would be executed after assets are loaded</param>
	TSharedPtr<FStreamableHandle> LoadSceneAsync(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate = nullptr);

	/// <summary>
	/// Load assets of the <paramref name="Scene"/> into the memory.
	/// </summary>
	/// <param name="Scene">Scenario whose assets should be loaded</param>
	/// <param name="AfterLoadDelegate">Delegate that would be executed after assets are loaded</param>
	TSharedPtr<FStreamableHandle> LoadScene(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate = nullptr);

	/*
	* Loads assets of future scenes.
	* @param Direction determines where future scenes are.
	*/
	void PrepareScenes(EVisualControllerDirection::Type Direction = EVisualControllerDirection::Forward);

	/// <summary>
	/// Releases the handle for the assets of the next scene.
	/// </summary>
	/// <remarks>
	/// If no other handles to these assets exist, assets will be unloaded from memory.
	/// </remarks>
	void CancelNextScene();

	/**
	* Requests Renderer to display transition animation.
	*/
	bool TryPlayTransition(const FScenario* From, const FScenario* To);

private:
	/**
	* Switch Visual Controller to the specified scenario, potentially switching node as well.
	*/
	void SetCurrentScene(const FScenario* Scene);

	/**
	* Guarantees that the next requested scenario assets will be loaded.
	* @param Direction determines what is the next scenario e.g. controller going back to the beginning or forward towards the end of the node.
	*/
	void AssertNextSceneLoad(EVisualControllerDirection::Type Direction = EVisualControllerDirection::Forward);

private:
	UPROPERTY()
	TObjectPtr<UVisualRenderer> Renderer;

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
	* How many following scenarios will be loaded asynchronously. Zero means no asynchronous loading.
	* @note Will load remaining scenarios in a node even when their amount is less than this value.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Controller|Async", meta = (DisplayName = "Num Scenarios to Load", AllowPrivateAccess = true))
	int32 ScenesToLoad;

	/*
	* Handles for resources of the following scenarios.
	* @note Number of elements is always less or equal to ScenesToLoad.
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

	/*
	* So far, the deepest scene in Visual Controller.
	*/
	const FScenario* Head;

	TUniquePtr<UE::VisualU::Private::FFastMoveAsyncTask> FastMoveTask;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Controller|Transition", meta = (AllowPrivateAccess = true, ToolTip = "Should Visual Controller attempt to play transition between scenarios."))
	bool bPlayTransitions;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Visual Controller|Flow control", meta = (AllowPrivateAccess = true, ToolTip = "Is Visual Controller in fast move mode."))
	bool bIsFastMoving;

};
