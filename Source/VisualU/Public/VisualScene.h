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
struct FAssetData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneStartEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneEndEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneLoadedEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneConstructedEvent);

/**
 * Class that loads, displays and connects scenes in the game
 */
UCLASS()
class VISUALU_API UVisualScene : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Visual Scene")
	static UVisualScene* Get();

	UVisualScene(const FObjectInitializer& ObjectInitializer);

	void CancelSceneLoading();

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario")
	const FText GetLine() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario")
	const FText GetAuthor() const;

	const FScenario* GetCurrentScene() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario")
	const FScenario& GetCurrentScenario() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario")
	bool IsWithChoice() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario|Instantiation")
	bool IsSceneLoading() const;

	bool IsSceneExhausted(const FScenario* Scene) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Visual Scene|Scenario")
	bool IsScenarioExhausted(const FScenario& Scenario) const;

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control")
	bool CanAdvanceScene() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control")
	bool CanRetractScene() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Scenario|Instantiation")
	bool IsSceneLoaded() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control")
	void ToNextScene();

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control")
	void ToPreviousScene();

	bool ToScene(const FScenario* Scene);

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control")
	bool ToScenario(const FScenario& Scenario);

	void NativeToBranch(const TArray<FScenario*>& Rows);

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Flow control")
	void ToBranch(UPARAM(ref) TArray<FScenario>& Rows);

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

	UPROPERTY(BlueprintAssignable, Category = "Visual Scene|Events")
	FOnSceneConstructedEvent OnSceneConstructed;

	DECLARE_MULTICAST_DELEGATE(FOnNativeSceneConstructedEvent);
	FOnNativeSceneConstructedEvent OnNativeSceneConstructed;

	/*
	* 
	* It is unclear how to include functionality of these methods inside Visual Scene.
	* Implementation of these behaviors seems reasonable on the higher game levels of abstraction.
	* Despite this, according to the setted purpose of Visual Scene and already implemented methods of control flow,
	* methods bellow should and probably would be realized later if/when the sufficient solution is found.
	* Visual Scene Component is also obsolete now because its application where transeferred to another game management authorities.
	* 
	* 
	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Components")
	void AutoProceedScenes(UVisualSceneComponent* Component);

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Components")
	void FastForward(UVisualSceneComponent* Component, float speedMultiplier = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Visual Scene|Components")
	void Rewind(UVisualSceneComponent* Component, float speedMultiplier = 1.0f);
	* 
	*/

protected:
	UVisualImage* Background;

	UCanvasPanel* Canvas;

	const UWidgetBlueprintGeneratedClass* BPScene;

	TSharedPtr<FStreamableHandle> ActiveSceneHandle;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	/*Visual Scene*/
	virtual void ConstructScene(const FScenario* Scene);

	virtual void LoadScene(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate, bool UnloadScene = true);
	/*~Visual Scene*/

	bool ClearSprites();

	/*
	* Outputs a friendly representation of scene's data to the log
	* @param InScenesData asset data of Scenario Data Tables
	* @see GetScenesData
	*/
	void PrintScenesData(const TArray<FAssetData>& InScenesData) const;

	/*
	* Gathers asset data of all Scenario Data Tables using Asset Registry
	* @param OutData gathered asset data
	*/
	void GetScenesData(TArray<FAssetData>& OutData) const;

private:
	TArray<FScenario*> Branch;

	int32 SceneIndex;

	TArray<FScenario*> ExhaustedScenes;

private:
	void ConstructScene();

	void LoadAndConstruct();

	void SetCurrentScene(const FScenario* Scene);
};
