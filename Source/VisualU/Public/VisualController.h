// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Scenario.h"
#include "Templates/SubclassOf.h"
#include "Async/AsyncWork.h"
#include "Containers/Ticker.h"
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#include "Containers/Deque.h"
#endif
#include "VisualController.generated.h"

class UVisualRenderer;
class APlayerController;
class DataTable;
class UWorld;
class UWidgetComponent;
class UVisualVersioningSubsystem;
struct FStreamableHandle;

/**
* !!! DO NOT USE OUTSIDE OF VISUAL CONTROLLER FUNCTIONS !!!
* Describes direction in which scenes move - current scene is expected to
* change either to the next(forward) or previous(backward) scene in the node.
* None is an internal value and must never be used.
* 
* @note Declared using a namespace in order to support negative enum values.
		It will break a lot of things in blueprints if
		this enum is used as a variable or in conversions, etc.
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

/**
* Describes current state of the UVisualController.
* This state is only affected by
* UVisualController::RequestFastMove() and UVisualController::RequestAutoMove()
*/
UENUM(BlueprintType)
enum class EVisualControllerMode : uint8
{
	Idle = 0,
	FastMoving = 1,
	AutoMoving = 2
};

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
					bool PlayedTransitions,
					bool PlayedSound)
					: VisualController(Controller),
					ControllerDirection(Direction),
					bPlayedTransitions(PlayedTransitions),
					bPlayedSound(PlayedSound)
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

				bool bPlayedSound;

			};

			class FFastMoveAsyncTask : public FAsyncTask<FFastMoveAsyncWorker>
			{
			public:
				FFastMoveAsyncTask(UVisualController* Controller,
					EVisualControllerDirection::Type Direction,
					bool PlayedTransitions,
					bool PlayedSound)
					: FAsyncTask(Controller, Direction, PlayedTransitions, PlayedSound)
				{
				};

			};
		}
	}
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneStart, const FScenario&, Scenario);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneEnd, const FScenario&, Scenario);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFastMoveStart, EVisualControllerDirection::Type, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFastMoveEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutoMoveStart, EVisualControllerDirection::Type, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAutoMoveEnd);

/**
 * Organizes scenes described by FScenario in a meaningful way.
 * Must be created within APlayerController.
 * Requires UVisualUSettings::FirstDataTable to be set at compile/editor time.
 * \image html Tree_structure.png
 * 
 * @see UVisualUSettings
 *		FScenario
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, Within = PlayerController)
class VISUALU_API UVisualController : public UObject
{
	GENERATED_BODY()

public:
	UVisualController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	* Brings controller to idle state before destruction.
	*/
	virtual void BeginDestroy() override;

	/**
	* Brings controller to idle state before serialization.
	* 
	* @param SaveContext save parameters
	*/
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

	/**
	* Setups initial values to make controller operational.
	*/
	virtual void PostInitProperties() override;

	/**
	* Serializes controller to the provided archive.
	* Uses FVisualUCustomVersion.
	*
	* @param Ar archive to serialize this controller
	*/
	virtual void SerializeController(FArchive& Ar);

	/**
	* Visualizes the next scene in the node.
	* 
	* @return result of the request
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Visualizes the next scene in the node"))
	bool RequestNextScene();

	/**
	* Visualizes the previous scene in the node.
	* Can also display last exhausted scene from previous node.
	* 
	* @return result of the request
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Visualizes the previous scene in the node"))
	bool RequestPreviousScene();

	/**
	* Visualizes a scene adjacent to the current scene in specified direction.
	* 
	* @param Direction describes position of the adjacent scene to request
	* @return result of the request
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Visualizes the adjacent scene in specified direction"))
	bool RequestAdjacentScene(EVisualControllerDirection::Type Direction = EVisualControllerDirection::Forward);

	/**
	* Visualize any exhausted scene.
	* Will fail for any other scene.
	* 
	* @note only use this for past scenes (e.g. already seen by the player)
	* 
	* @param Scene exhausted scenario to visualize
	* @return result of the request
	*/
	bool RequestScene(const FScenario* Scene);

	/**
	* @see UVisualController::RequestScene()
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Visualize any exhausted scene"))
	bool RequestScenario(const FScenario& Scenario);

	/**
	* Gets scene from the node at given position.
	* 
	* @note will trigger assertion for invalid index.
	* 
	* @param Index position of the scene
	* @return valid scene
	*/
	const FScenario* GetSceneAt(int32 Index);

	/**
	* Sets provided node as active and visualizes the first scene in it.
	* Will trigger assertion when:
	* -# Provided data table is invalid
	* -# Data table is empty
	* -# Data table is not based on {@code FScenario}
	* -# Data table was previously set to this controller
	* Use this to request choice options (data tables) when
	* choice is introduced.
	* 
	* @note UVisualController::RequestScene can be used for
	*		requesting previous nodes
	* 
	* @param NewNode data table with at least one scene
	* @return result of the request
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Sets provided node as active. Data table can't be empty. Will visualize the first scene of the node."))
	bool RequestNode(const UDataTable* NewNode);

	/**
	* Tries to activate fast move mode.
	* In this mode, controller will request scenes
	* in the specified direction as fast as possible.
	* This mode will end when UVisualController::Head is reached or
	* scene with EScenarioMetaFlags::Choice is encountered.
	* 
	* @note scene transitions are disabled in this mode
	* 
	* @param Direction decides whether to request next or previous scenes
	* @return result of the request
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	bool RequestFastMove(EVisualControllerDirection::Type Direction = EVisualControllerDirection::Forward);

	/**
	* Tries to activate auto move.
	* In this mode, controller will request scenes
	* at the pace of UVisualController::AutoMoveDelay.
	* This mode will end when scene with
	* EScenarioMetaFlags::Choice is encountered.
	* 
	* @note ends when UVisualController::AutoMoveDelay is lower than
	*		UVisualUSettings::TransitionDuration
	* 
	* @param Direction decides whether to request next or previous scenes
	* @return result of the request
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	bool RequestAutoMove(EVisualControllerDirection::Type Direction = EVisualControllerDirection::Forward);

	/**
	* Ends fast move mode when it is active.
	* controller becomes idle.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	void CancelFastMove();

	/**
	* Ends auto move mode when it is active.
	* controller becomes idle.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	void CancelAutoMove();

	/**
	* Reconstruct renderer to a new class if necessary and
	* add it to the player screen.
	* 
	* @note renderer will show currently selected scene
	* 
	* @param RendererClass Renderer class, 
	*        has no effect when current and provided classes are the same.
	* @param ZOrder renderer layer on the screen
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void VisualizeToScreen(TSubclassOf<UVisualRenderer> RendererClass, int32 ZOrder = 0);

	/**
	* Reconstructs renderer to a new class if necessary and
	* add it to the widget component.
	* 
	* @note renderer will show currently selected scene
	* 
	* @param RendererClass Renderer class, 
	*        has no effect when current and provided classes are the same.
	* @param Component widget component that should display the renderer
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void VisualizeToComponent(TSubclassOf<UVisualRenderer> RendererClass, UWidgetComponent* Component);

	/**
	* Removes renderer visualized to screen.
	* Has no effect on widget component or not visualized renderer.
	* 
	* @note renderer will remain valid after removal
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void RemoveFromScreen() const;

	/**
	* Sets renderer visibility.
	* 
	* @param Visibility new renderer visibility
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	void SetRendererVisibility(ESlateVisibility Visibility);

	/**
	* @return current renderer visibility
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Widget")
	ESlateVisibility GetRendererVisibility() const;

	/**
	* Setter for UVisualController::NumScenesToLoad
	* 
	* @param Num Number of scenes to load. Triggers a warning
	*		 when larger than ScenesToLoadLargeNum
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Async", meta = (DisplayName = "SetNumScenariosToLoad"))
	void SetNumScenesToLoad(int32 Num);

	/**
	* @return number of scenes to load
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Async", meta = (DisplayName = "GetNumScenariosToLoad"))
	FORCEINLINE int32 GetNumScenesToLoad() const { return ScenesToLoad; };

	/**
	* Setter for UVisualController::bPlayTransitions.
	* Calling this during fast moving mode is discouraged because
	* bPlayTransitions may change after fast moving mode is over.
	* 
	* @param bShouldPlay {@code true} to play transitions when available
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Transition")
	void ShouldPlayTransitions(bool bShouldPlay);

	/**
	* @return decision of this controller to play transitions between scenes
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Transition")
	FORCEINLINE bool PlaysTransitions() const { return bPlayTransitions; }

	/**
	* Setter for UVisualController::bPlaySound.
	* Calling this during fast moving mode is discouraged because
	* bPlaySound may change after fast moving mode is over.
	* 
	* @param bShouldPlay {@code true} to play sound when available
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Sound")
	void ShouldPlaySound(bool bShouldPlay);

	/**
	* @return decision of this controller to play sound when scene starts
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Sound")
	FORCEINLINE bool PlaysSound() const { return bPlaySound; }

	/**
	* Setter for UVisualController::AutoMoveDelay.
	* 
	* @param Delay new delay, in seconds, between scenes in auto move mode
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	void SetAutoMoveDelay(float Delay);
	
	/**
	* @return delay between scenes when controller is in auto move mode
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	FORCEINLINE float GetAutoMoveDelay() const { return AutoMoveDelay; }

	/**
	* @return currently visualized scene.
	*/
	const FScenario* GetCurrentScene() const;

	/**
	* @return currently visualized scene.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Scenario", meta = (ToolTip = "Currently visualized scenario"))
	const FScenario& GetCurrentScenario() const;

	/**
	* @return {@code true} when there is a scene in front of the current one
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Is there a scene in front of the current one"))
	bool CanAdvanceScene() const;

	/**
	* @return {@code true} when there is a scene behind the current one
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control", meta = (ToolTip = "Is there a scene behind the current one"))
	bool CanRetractScene() const;

	/**
	* @return is currently visualized scene has EScenarioMetaFlags::Choice set
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Scenario", meta = (ToolTip = "Whether or not currently visualized scene contains a choice"))
	bool IsWithChoice() const;

	/**
	* Is provided scene exhausted.
	*
	* @note scene is considered exhausted when it was already seen by player
	*
	* @param Scene scene to check for exhaustion
	* @return is scene considered exhausted
	*
	* @see UVisualController::ExhaustedScenes
	*/
	bool IsSceneExhausted(const FScenario* Scene) const;

	/**
	* @see UVisualController::IsSceneExhausted()
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Visual Controller|Scenario", meta = (ToolTip = "is provided scenario exhausted"))
	bool IsScenarioExhausted(const FScenario& Scenario) const;

	/**
	* Is renderer has transition ongoing.
	* 
	* @see UVisualRenderer::IsTransitionInProgress()
	* @return {@code true} for active scene transition
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	bool IsTransitioning() const;

	/**
	* @return {@code true} when current scene is UVisualController::Head
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	bool IsCurrentScenarioHead() const;

	/**
	* @return current mode of the controller
	* 
	* @see UVisualController::IsFastMoving()
	*		   UVisualController::IsAutoMoving()
	*		   UVisualController::IsIdle()
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	FORCEINLINE EVisualControllerMode GetMode() const { return Mode; }

	/**
	* @return {@code true} when controller is in fast move mode
	* 
	* @see UVisualController::GetMode()
	*		   UVisualController::IsAutoMoving()
	*		   UVisualController::IsIdle()
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	FORCEINLINE bool IsFastMoving() const { return Mode == EVisualControllerMode::FastMoving; }

	/**
	* @return {@code true} when controller is in auto move mode
	* 
	* @see UVisualController::GetMode()
	*		   UVisualController::IsFastMoving()
	*          UVisualController::IsIdle()
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	FORCEINLINE bool IsAutoMoving() const { return Mode == EVisualControllerMode::AutoMoving; }
	
	/**
	* @return {@code true} when controller is idle
	* 
	* @see UVisualController::GetMode()
	*		   UVisualController::IsFastMoving()
	*		   UVisualController::IsAutoMoving()
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Flow control")
	FORCEINLINE bool IsIdle() const { return Mode == EVisualControllerMode::Idle; }

	/**
	* Development only.
	* 
	* @return debug information of the UVisualController::Head
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Debug", meta = (DevelopmentOnly))
	const FString GetHeadDebugString() const;

	/**
	* Development only.
	* 
	* @return debug information about asynchronous scene preparation
	* 
	* @see UVisualController::DebugSceneHandles
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Debug", meta = (DevelopmentOnly))
	const FString GetAsyncQueueDebugString() const;

	/**
	* Development only.
	* 
	* @return debug information about UVisualController::ExhaustedScenes
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Controller|Debug", meta = (DevelopmentOnly))
	const FString GetExhaustedScenesDebugString() const;

	/**
	* Called when controller has switched to a different scenario.
	* 
	* @param Scenario scenario to which controller has swiched to
	*/
	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnSceneStart OnSceneStart;

	/**
	* Called when controller is about to switch to a different scenario.
	* 
	* @param Scenario active scenario before controller switches
	*/
	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnSceneEnd OnSceneEnd;

	/**
	* Called when controller enters Fast Move mode.
	* 
	* @param Direction direction in which controller moves
	*/
	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnFastMoveStart OnFastMoveStart;

	/**
	* Called when controller exits Fast Move mode.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnFastMoveEnd OnFastMoveEnd;

	/**
	* Called when controller enters Auto Move mode.
	* 
	* @param Direction direction in which controller moves
	*/
	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnAutoMoveStart OnAutoMoveStart;

	/**
	* Called when controller exits Auto Move mode.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Visual Controller|Events")
	FOnAutoMoveEnd OnAutoMoveEnd;

protected:
	/**
	* Asynchronously loads assets of the scene into the memory.
	* 
	* @param Scene scenario that provides assets to stream in
	* @param AfterLoadDelegate delegate to execute after assets are loaded
	* @return handle to manage lifetime of streamed assets
	* 
	* @see UVisualController::LoadScene()
	*/
	TSharedPtr<FStreamableHandle> LoadSceneAsync(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate = nullptr);

	/**
	* Synchronously loads assets of the scene into the memory.
	*
	* @param Scene scenario that provides assets to load
	* @param AfterLoadDelegate delegate to execute after assets are loaded
	* @return handle to manage lifetime of streamed assets
	* 
	* @see UVisualController::LoadSceneAsync()
	*/
	TSharedPtr<FStreamableHandle> LoadScene(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate = nullptr);

	/**
	* Asynchronously loads assets of future scenes after controller moves.
	* Has no effect when UVisualController::NumScenesToLoad is less than zero.
	* 
	* @param Direction determines where future scenes are
	*/
	void PrepareScenes(EVisualControllerDirection::Type Direction = EVisualControllerDirection::Forward);

	/**
	* Plays the scene sound when available.
	* Will fail when scene sound is invalid or controller doesn't play sound.
	* 
	* @param SceneSound audio to play
	* 
	* @see UVisualController::bPlaySound
	*/
	void TryPlaySceneSound(TSoftObjectPtr<USoundBase> SceneSound) const;

	/**
	* Releases the handle for the assets of the next scene.
	* 
	* @see UVisualController::NextSceneHandle
	*/
	void CancelNextScene();

	/**
	* Requests renderer to display transition animation.
	* 
	* @param From Scene that will be displayed at the start and then blended
	*        as transition progresses. Usually the currently displayed scene.
	* @param To Scene that will be visualized after transition ends
	* @return result of the request
	*/
	bool TryPlayTransition(const FScenario* From, const FScenario* To);

	/**
	* Switches controller to the previously seen scene.
	* 
	* @param Scene scene to be visualized by this controller
	*/
	void RollbackTo(const FScenario* Scene);

	/**
	* @return valid visual versioning subsystem or nullptr.
	*/
	UVisualVersioningSubsystem* TryGetVisualVersioningSubsystem() const;

	/**
	* Guarantees that the next requested scene assets will be loaded.
	* 
	* @param Direction determines what is the next scene 
	*		 e.g. controller going back to the
	*		 beginning or forward towards the end of the node
	*/
	void AssertNextSceneLoad(EVisualControllerDirection::Type Direction = EVisualControllerDirection::Forward);

private:
	/**
	* Responsible for visualizing scenes as widgets.
	* 
	* @note Transient, it will not be serialized.
	*/
	UPROPERTY(Transient)
	TObjectPtr<UVisualRenderer> Renderer;

	/**
	* Handle for assets of the scene that are loaded into the memory.
	*/
	TSharedPtr<FStreamableHandle> NextSceneHandle;

	/**
	* Currently active data table with scenes.
	* 
	* @see FScenario
	*/
	TArray<FScenario*> Node;

	/**
	* Position of the current scene.
	*/
	int32 SceneIndex;

	/**
	* Handles for resources of the following scenes.
	* 
	* @note number of elements is always
	*		less or equal to UVisualController::ScenesToLoad.
	*/
	TQueue<TSharedPtr<FStreamableHandle>> SceneHandles;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	/**
	* Additional debug queue that gathers weak references to scenes
	* that are meant to be loaded asynchronously.
	* 
	* @note isn't present in shipping or test builds
	* 
	* @see UVisualController::SceneHandles
	*		   UVisualController::PrepareScenes()
	*/
	TDeque<TWeakPtr<FStreamableHandle>> DebugSceneHandles{};
#endif

	/**
	* Maintains references to all data tables that
	* currently own scenes that are referenced by controller.
	*/
	UPROPERTY()
	TSet<const UDataTable*> NodeReferenceKeeper;

	/**
	* Last scenes of already processed nodes which could be
	* made current again in the future. In other words,
	* when controller returns back to the exhausted scene, that scene
	* is not exhausted anymore untill the controller advances forward again.
	*/
	TArray<FScenario*> ExhaustedScenes;

	/**
	* So far, the deepest scene in Visual Controller.
	* It does not account for parallel branches.
	* That means that when the new node is introduced, 
	* its first scene will become the head even if it
	* has less scenes than parallel node (e.g. other choice option)
	*/
	const FScenario* Head;

	/**
	* Task to be dispatched asynchronously to perform fast move.
	* 
	* @see UVisualController::RequestFastMove()
	*/
	TUniquePtr<UE::VisualU::Private::FFastMoveAsyncTask> FastMoveTask;

	/**
	* Handle to the ticker that performs auto move.
	* 
	* @see UVisualController::RequestAutoMove()
	*/
	FTSTicker::FDelegateHandle AutoMoveHandle;

	/**
	* How many following scenes will be loaded asynchronously.
	* Zero means no asynchronous loading.
	*
	* @note will load remaining scenes in a node even
	*		when their amount is less than this value
	*/
	UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = "Visual Controller|Async", meta = (AllowPrivateAccess = true, UIMin = 0.f, ClampMin = 0.f, ToolTip = "How many following scenes will be loaded asynchronously. Zero means no asynchronous loading."))
	int32 ScenesToLoad;

	/**
	* Should controller attempt to play transitions between scenes.
	* Changes to this value during fast moving mode
	* may be reverted when said mode is over.
	*/
	UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = "Visual Controller|Transition", meta = (AllowPrivateAccess = true, ToolTip = "Should Visual Controller attempt to play transition between scenarios"))
	bool bPlayTransitions;

	/**
	* Will controller attempt to play scene sound.
	* Changes to this value during fast moving mode
	* may be reverted when said mode is over.
	* 
	* @note sound will be disabled when engine is launched with no sound
	*/
	UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = "Visual Controller|Sound", meta = (AllowPrivateAccess = true, ToolTip = "Will Visual Controller play scene sound"))
	bool bPlaySound;

	/**
	* How long, in seconds, controller should wait
	* before moving to the next scene in auto move mode.
	* Must be larger than transition duration to not stop on transitions.
	* 
	* @note avoid a zero to simulate fast forwarding, use fast move instead
	* 
	* @see UVisualController::RequestAutoMove()
	*	   UVisualController::RequestFastMove()
	*/
	UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = "Visual Controller|Flow control", meta = (AllowPrivateAccess = true, UIMin = 0.f, ClampMin = 0.f, ToolTip = "How long, in seconds, Visual Controller should wait before moving to the next scene in Auto Move mode. Must be larger than transition duration to not stop on transitions. Warning: don't put a zero to simulate fast forwarding, use FastMove instead."))
	float AutoMoveDelay;

	/**
	* Current state of this controller.
	* Controller can be fast moving, auto moving, or idle.
	*/
	UPROPERTY(VisibleInstanceOnly, Transient, BlueprintReadOnly, Category = "Visual Controller|Flow control", meta = (AllowPrivateAccess = true, ToolTip = "Current state of the Visual Controller"))
	EVisualControllerMode Mode;

};
