// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VisualU.h"
#include "Engine/DataTable.h"
#include "VisualSprite.h"
#include "VisualDefaults.h"
#include "VisualImage.h"
#include "VisualChoice.h"
#include "Scenario.generated.h"

class UPaperFlipbook;
class UMaterialInterface;
class USoundCue;

/// <summary>
/// Polymorphlic struct that describes which <see cref="UVisualSprite">Visual Sprite</see> to visualize, its appearance and position 
/// in the <see cref="UVisualScene">Visual Scene</see>.
/// </summary>
USTRUCT(BlueprintType)
struct VISUALU_API FSprite
{
	GENERATED_USTRUCT_BODY()

public:
	FSprite() : Anchors(ForceInit), Position(ForceInit), ZOrder(0) {}

	virtual ~FSprite() {}

	/// <summary>
	/// <see cref="UVisualSprite">Visual Sprite</see> class to be constructed and visualized by <see cref="UVisualScene">Visual Scene</see>.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Visual Sprite class to be constructed and visualized by Visual Scene"))
	TSoftClassPtr<UVisualSprite> SpriteClass;

	/// <summary>
	/// <see cref="UVisualSprite">Visual Sprite</see> anchors in Canvas Panel.
	/// </summary>
	/// <seealso cref="FVisualAnchors"/>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Visual Sprite anchors in Canvas Panel"))
	FVisualAnchors Anchors;

	/// <summary>
	/// <see cref="UVisualSprite">Visual Sprite</see> position in Canvas Panel.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Visual Sprite position in Canvas Panel"))
	FVector2D Position;

	/// <summary>
	/// <see cref="UVisualSprite">Visual Sprite</see> Z coordinate (or layer) in Canvas Panel.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Visual Sprite's Z coordinate (a.k.a layer) in Canvas Panel"))
	int32 ZOrder;

	/// <summary>
	/// Information for <see cref="UVisualImage">Visual Images</see> inside <see cref="UVisualSprite">Visual Sprite</see>.
	/// </summary>
	/// <seealso cref="UVisualSprite::AssignVisualImageInfo"/>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Information for Visual Images inside Visual Sprite"))
	TArray<FVisualImageInfo> SpriteInfo;

	/// <summary>
	/// Prints all fields to the VisualU log.
	/// </summary>
	virtual void PrintLog() const
	{
		UE_LOG(LogVisualU, Warning, TEXT("Sprite Class: %s"), !SpriteClass.IsNull() ? *SpriteClass->GetFName().ToString() : TEXT("None"));
		UE_LOG(LogVisualU, Warning, TEXT("Anchors: %s"), *Anchors.ToString());
		UE_LOG(LogVisualU, Warning, TEXT("Position: %s"), *Position.ToString());
		UE_LOG(LogVisualU, Warning, TEXT("Z order: %d"), ZOrder);
		if (!SpriteInfo.IsEmpty())
		{
			int cnt = 0;
			for (const auto& Info : SpriteInfo)
			{
				cnt++;
				UE_LOG(LogVisualU, Warning, TEXT("\tSprite Info %d: %s"), cnt, *Info.ToString());
			}
		}
	}

	FORCEINLINE bool operator== (const FSprite& Other)
	{
		if (SpriteClass == Other.SpriteClass
			&& Anchors == Other.Anchors
			&& Position == Other.Position
			&& ZOrder == Other.ZOrder)
		{
			for (int i = 0; i < SpriteInfo.Num(); i++)
			{
				if (!Other.SpriteInfo.IsValidIndex(i) || SpriteInfo[i] != Other.SpriteInfo[i])
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}

	FORCEINLINE bool operator!= (const FSprite& Other)
	{
		return !(*this == Other);
	}
};

/// <summary>
/// Polymorphic struct that describes what background to display and what effect to use for transition.
/// </summary>
USTRUCT(BlueprintType)
struct VISUALU_API FBackground
{
	GENERATED_USTRUCT_BODY()

public:
	FBackground() {}

	virtual ~FBackground() {}

	/// <summary>
	/// Background Art of the scene.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene", meta = (ToolTip = "Background Art of this Scenario"))
	TSoftObjectPtr<UPaperFlipbook> BackgroundArt;

	/// <summary>
	/// Transition to play on background when switching to the next scene.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene", meta = (ToolTip = "Transition to play on background when switching to the next scene"))
	TSoftObjectPtr<UMaterialInterface> TransitionMaterial;
};

/// <summary>
/// A single scene or "frame" of the Visual Novel game.
/// </summary>
/// <remarks>
/// Struct with data that describe one scene handled by <see cref="UVisualScene">Visual Scene</see>.
/// Can be imported from CSV or JSON file or added directly as a row to the <c>Data Table</c> that has <c>FScenario</c> row struct.
/// Terms "Scene", "Row" and "Scenario" all refer to <c>FScenario</c>. <see cref="UVisualScene">Visual Scene</see> and "Scene" are distinguished
/// by word "Visual" in front of the class one.
/// Grouped together under <c>Data Table</c>, they represent a node of the tree structure that allows to create multiple ways to complete the game. 
/// The first scene in the game is specified by <see cref="UVisualUSettings::FirstDataTable"/> field.
/// The last scene in the node **can** have <see cref="UVisualChoice">Visual Choice sprite</see> that links nodees together.
/// Connections between scenes are managed by <see cref="UVisualScene">Visual Scene</see>.
/// That being said, <c>FScenario</c> is an intrusive data structure, 
/// meaning that <c>FScenario</c> is well aware in what node and where in the node it resides.
/// That implies that <c>FScenario</c> must be a part of a node and cannot be owned by some other entity.
/// <c>FScenario</c> is a polymorphic struct and can be extended if needed.
/// </remarks>
/// <seealso cref="FSprite"/>
/// <seealso cref="UVisualScene"/>
USTRUCT(BlueprintType)
struct VISUALU_API FScenario : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FScenario() {}

	/// <summary>
	/// An author of the <see cref="FScenario::Line">Line</see>.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene", meta = (ToolTip = "An author of the Line"))
	FText Author;

	/// <summary>
	/// A dialog line.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene", meta = (ToolTip = "A dialog line"))
	FText Line;
	
	/// <summary>
	/// Audio to play while the scene is displayed.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene", meta = (ToolTip = "Audio to play while this Scenario is displayed"))
	TSoftObjectPtr<USoundBase> Music;
	
	/// <summary>
	/// Background to display.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene", meta = (ToolTip = "Background to display"))
	FBackground Background;

	/// <summary>
	/// Sprites that this scene has.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene", meta = (ToolTip = "Sprites that this Scenario has"))
	TArray<FSprite> SpritesParams;

	const UDataTable* Owner;

	int32 Index;

	virtual void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems) override
	{
		Intrusive(InDataTable);
	}

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Intrusive(InDataTable);
	}

	/// <summary>
	/// Supplies assets that should be loaded into the memory.
	/// Note: these assets might be already loaded.
	/// </summary>
	/// <param name="Out">Array to be filled with data that should be loaded</param>
	/// <remarks>
	/// Method will empty <paramref name="Out"/> array.
	/// </remarks>
	virtual void GetDataToLoad(TArray<FSoftObjectPath>& Out) const
	{
		if (!Background.BackgroundArt.IsNull())
		{
			Out.Emplace(Background.BackgroundArt.ToSoftObjectPath());
		}
		if (!Music.IsNull())
		{
			Out.Emplace(Music.ToSoftObjectPath());
		}
		if (!Background.TransitionMaterial.IsNull())
		{
			Out.Emplace(Background.TransitionMaterial.ToSoftObjectPath());
		}
		for (const auto& SpriteParam : SpritesParams)
		{
			Out.Emplace(SpriteParam.SpriteClass.ToSoftObjectPath());
			for (const auto& Info : SpriteParam.SpriteInfo)
			{
				Out.Emplace(Info.Expression.ToSoftObjectPath());
			}
		}
	}

	FORCEINLINE bool operator== (const FScenario& Other)
	{
		if (Author.CompareTo(Other.Author)
			&& Line.CompareTo(Other.Line)
			&& Background.BackgroundArt.GetAssetName() == Other.Background.BackgroundArt.GetAssetName()
			&& Background.TransitionMaterial.GetAssetName() == Other.Background.TransitionMaterial.GetAssetName()
			&& Music.GetAssetName() == Other.Music.GetAssetName())
		{
			int i = 0;
			for (i = 0; i < SpritesParams.Num(); i++)
			{
				if (!Other.SpritesParams.IsValidIndex(i) || SpritesParams[i] != Other.SpritesParams[i])
				{
					return false;
				}
			}

			return true;
		}

		return false;
	}

	FORCEINLINE bool operator!= (const FScenario& Other)
	{
		return !(*this == Other);
	}

	/// <summary>
	/// Prints all content of the scene to VisualU log.
	/// </summary>
	virtual void PrintLog() const
	{
		UE_LOG(LogVisualU, Warning, TEXT("\tAuthor: %s"), !Author.IsEmpty() ? *Author.ToString() : TEXT("None"));
		UE_LOG(LogVisualU, Warning, TEXT("\tLine: %s"), !Line.IsEmpty() ? *Line.ToString() : TEXT("None"));
		UE_LOG(LogVisualU, Warning, TEXT("\tMusic: %s"), !Music.IsNull() ? *Music.GetAssetName() : TEXT("None"));
		UE_LOG(LogVisualU, Warning, TEXT("\tBackground Art: %s"), !Background.BackgroundArt.IsNull() ? *Background.BackgroundArt.GetAssetName() : TEXT("None"));
		UE_LOG(LogVisualU, Warning, TEXT("\tTransition Material: %s"), !Background.TransitionMaterial.IsNull() ? *Background.TransitionMaterial.GetAssetName() : TEXT("None"));

		if (!SpritesParams.IsEmpty())
		{
			int cnt = 0;
			for (const auto& SpriteParam : SpritesParams)
			{
				cnt++;
				UE_LOG(LogVisualU, Warning, TEXT("\tSprite Parameter %d"), cnt);
				SpriteParam.PrintLog();
			}
		}
	}

	/// <summary>
	/// Checks whether or not this scene has at least one <see cref="UVisualChoice">Visual Choice</see>.
	/// </summary>
	/// <returns><c>true</c> if scene has <see cref="UVisualChoice">Visual Choice</see></returns>
	inline bool hasChoice() const
	{
		if (!SpritesParams.IsEmpty())
		{
			for (const auto& SpriteParam : SpritesParams)
			{
				if (!SpriteParam.SpriteClass.IsNull() && SpriteParam.SpriteClass->IsChildOf<UVisualChoice>())
				{
					return true;
				}
			}
		}
		
		return false;
	}

private:
	/// \internal
	/// <summary>
	/// Embeds the owning <c>Data Table</c> and index at which this scene resides.
	/// </summary>
	/// <param name="InDataTable">Owner of this scene</param>
	inline void Intrusive(const UDataTable* InDataTable)
	{
		Owner = InDataTable;
		TArray<FScenario*> Rows;
		InDataTable->GetAllRows(TEXT("Scenario.h(318)"), Rows);
		Rows.Find(this, Index);
	}
};
