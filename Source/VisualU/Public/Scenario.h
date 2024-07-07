// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VisualU.h"
#include "Engine/DataTable.h"
#include "VisualSprite.h"
#include "VisualDefaults.h"
#include "VisualImage.h"
#include "InfoAssignable.h"
#include "Scenario.generated.h"

class UPaperFlipbook;
class UMaterialInterface;
class USoundCue;

/// <summary>
/// Polymorphlic struct that describes which <see cref="UVisualSprite">Visual Sprite</see> to visualize, its appearance and position 
/// in the <see cref="UVisualScene">Visual Scene</see>.
/// </summary>
USTRUCT(BlueprintType)
struct FSprite
{
	GENERATED_BODY()

public:
	FSprite() = default;

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
	FVector2D Position = FVector2D(ForceInitToZero);

	/// <summary>
	/// <see cref="UVisualSprite">Visual Sprite</see> Z coordinate (or layer) in Canvas Panel.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Visual Sprite's Z coordinate (a.k.a layer) in Canvas Panel"))
	int32 ZOrder = 0;

	/// <summary>
	/// Information for <see cref="UVisualImage">Visual Images</see> inside <see cref="UVisualSprite">Visual Sprite</see>.
	/// </summary>
	/// <seealso cref="UVisualSprite::AssignSpriteInfo"/>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Information for Visual Images inside Visual Sprite"))
	TArray<FVisualImageInfo> SpriteInfo;

	FORCEINLINE friend FArchive& operator<< (FArchive& Ar, FSprite& Sprite)
	{
		Ar << Sprite.SpriteClass 
		   << Sprite.Anchors 
		   << Sprite.Position 
		   << Sprite.ZOrder 
		   << Sprite.SpriteInfo;

		return Ar;
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	/// <summary>
	/// Prints all fields to the VisualU log.
	/// </summary>
	inline void PrintLog() const
	{
		UE_LOG(LogVisualU, Warning, TEXT("Sprite Class: %s"), !SpriteClass.IsNull() ? *SpriteClass->GetFName().ToString() : TEXT("None"));
		UE_LOG(LogVisualU, Warning, TEXT("Anchors: %s"), *Anchors.ToString());
		UE_LOG(LogVisualU, Warning, TEXT("Position: %s"), *Position.ToString());
		UE_LOG(LogVisualU, Warning, TEXT("Z order: %d"), ZOrder);
		if (!SpriteInfo.IsEmpty())
		{
			int32 cnt = 0;
			for (const FVisualImageInfo& Info : SpriteInfo)
			{
				cnt++;
				UE_LOG(LogVisualU, Warning, TEXT("\tSprite Info %d: %s"), cnt, *Info.ToString());
			}
		}
	}
#endif

	FORCEINLINE bool operator== (const FSprite& Other) const
	{
		return SpriteClass == Other.SpriteClass
			&& Anchors == Other.Anchors
			&& Position == Other.Position
			&& ZOrder == Other.ZOrder
			&& SpriteInfo == Other.SpriteInfo;
	}

	FORCEINLINE bool operator!= (const FSprite& Other) const
	{
		return !(*this == Other);
	}
};

/// <summary>
/// Polymorphic struct that describes what background to display and what effect to use for transition.
/// </summary>
USTRUCT(BlueprintType)
struct FBackground
{
	GENERATED_BODY()

public:
	FBackground() = default;

	/// <summary>
	/// Background info for the scene.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene", meta = (ToolTip = "Background info for the Scenario"))
	FVisualImageInfo BackgroundArtInfo;

	/// <summary>
	/// Transition to play on background when switching to the next scene.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene", meta = (ToolTip = "Transition to play on background when switching to the next scene"))
	TSoftObjectPtr<UMaterialInterface> TransitionMaterial;

	FORCEINLINE friend FArchive& operator<< (FArchive& Ar, FBackground& Background)
	{
		return Ar << Background.BackgroundArtInfo << Background.TransitionMaterial;
	}

	FORCEINLINE bool operator== (const FBackground& Other) const
	{
		return (BackgroundArtInfo == Other.BackgroundArtInfo && TransitionMaterial == Other.TransitionMaterial);
	}

	FORCEINLINE bool operator!= (const FBackground& Other) const
	{
		return !(*this == Other);
	}
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class EScenarioMetaFlags : uint8
{
	None = 0,
	Character = 1 << 0,
	Choice = 1 << 1,
	Custom_1 = 1 << 2,
	Custom_2 = 1 << 3,
	Custom_3 = 1 << 4,
	Custom_4 = 1 << 5,
	Custom_5 = 1 << 6
};
ENUM_CLASS_FLAGS(EScenarioMetaFlags)

USTRUCT(BlueprintType)
struct FVisualScenarioInfo : public FVisualInfo
{
	GENERATED_BODY()

public:
	FVisualScenarioInfo() = default;

	/// <summary>
	/// An author of the <see cref="FScenario::Line">Line</see>.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info", meta = (ToolTip = "An author of the Line"))
	FText Author;

	/// <summary>
	/// A dialog line.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info", meta = (ToolTip = "A dialog line"))
	FText Line;

	/// <summary>
	/// Audio to play while the scene is displayed.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info", meta = (ToolTip = "Audio to play while this Scenario is displayed"))
	TSoftObjectPtr<USoundBase> Sound;

	/// <summary>
	/// Background to display.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info", meta = (ToolTip = "Background to display"))
	FBackground Background;

	/// <summary>
	/// Sprites that this scene has.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info", meta = (ToolTip = "Sprites that this Scenario has"))
	TArray<FSprite> SpritesParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info", meta = (Bitmask, BitmaskEnum = "/Script/VisualU.EScenarioMetaFlags", ToolTip = "What kinds of Visual sprites this scene has"))
	uint8 Flags = 0;

	virtual void Accept(IInfoAssignable* Visitor) const override
	{
		Visitor->AssignScenarioVisualInfo(*this);
	}

	FORCEINLINE void Serialize(FArchive& Ar)
	{
		Ar << *this;
	}

	FORCEINLINE friend FArchive& operator<< (FArchive& Ar, FVisualScenarioInfo& ScenarioInfo)
	{
		Ar << ScenarioInfo.Author
			<< ScenarioInfo.Line
			<< ScenarioInfo.Sound
			<< ScenarioInfo.Background
			<< ScenarioInfo.SpritesParams
			<< ScenarioInfo.Flags;

		return Ar;
	}

	FORCEINLINE bool operator== (const FVisualScenarioInfo& Other) const
	{
		return Author.CompareTo(Other.Author)
			&& Line.CompareTo(Other.Line)
			&& Sound == Other.Sound
			&& Background == Other.Background
			&& SpritesParams == Other.SpritesParams
			&& Flags == Other.Flags;
	}

	FORCEINLINE bool operator!= (const FVisualScenarioInfo& Other) const
	{
		return !(*this == Other);
	}
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
struct VISUALU_API FScenario : public FTableRowBase, public IInfoAssignable
{
	GENERATED_BODY()

public:
	FScenario() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario")
	FVisualScenarioInfo Info;

	//Left undiscoverable by reflection to avoid circular references and intricacies of TWeakObjectPtr.
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
		Out.Reserve(3 + Info.SpritesParams.Num());
		if (!Info.Background.BackgroundArtInfo.Expression.IsNull())
		{
			Out.Emplace(Info.Background.BackgroundArtInfo.Expression.ToSoftObjectPath());
		}
		if (!Info.Sound.IsNull())
		{
			Out.Emplace(Info.Sound.ToSoftObjectPath());
		}
		if (!Info.Background.TransitionMaterial.IsNull())
		{
			Out.Emplace(Info.Background.TransitionMaterial.ToSoftObjectPath());
		}
		for (const FSprite& SpriteParam : Info.SpritesParams)
		{
			Out.Emplace(SpriteParam.SpriteClass.ToSoftObjectPath());
			for (const FVisualImageInfo& ImageInfo : SpriteParam.SpriteInfo)
			{
				Out.Emplace(ImageInfo.Expression.ToSoftObjectPath());
			}
		}
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	/// <summary>
	/// Prints all content of the scene to VisualU log.
	/// </summary>
	virtual void PrintLog() const
	{
		UE_LOG(LogVisualU, Display, TEXT("\tAuthor: %s"), !Info.Author.IsEmpty() ? *Info.Author.ToString() : TEXT("None"));
		UE_LOG(LogVisualU, Display, TEXT("\tLine: %s"), !Info.Line.IsEmpty() ? *Info.Line.ToString() : TEXT("None"));
		UE_LOG(LogVisualU, Display, TEXT("\tMusic: %s"), !Info.Sound.IsNull() ? *Info.Sound.GetAssetName() : TEXT("None"));
		UE_LOG(LogVisualU, Display, TEXT("\tBackground Art: %s"), !Info.Background.BackgroundArtInfo.Expression.IsNull() ? *Info.Background.BackgroundArtInfo.Expression.GetAssetName() : TEXT("None"));
		UE_LOG(LogVisualU, Display, TEXT("\tTransition Material: %s"), !Info.Background.TransitionMaterial.IsNull() ? *Info.Background.TransitionMaterial.GetAssetName() : TEXT("None"));

		if (!Info.SpritesParams.IsEmpty())
		{
			int32 cnt = 0;
			for (const FSprite& SpriteParam : Info.SpritesParams)
			{
				cnt++;
				UE_LOG(LogVisualU, Display, TEXT("\tSprite Parameter %d"), cnt);
				SpriteParam.PrintLog();
			}
		}
	}

	FORCEINLINE FString GetDebugString() const
	{
		check(Owner);
		return FString::Printf(TEXT("DataTable: %s, index: %i"), *Owner->GetFName().ToString(), Index);
	}
#endif

	/// <summary>
	/// Checks whether or not this scene has at least one <see cref="UVisualChoice">Visual Choice</see>.
	/// </summary>
	/// <returns><c>true</c> if scene has <see cref="UVisualChoice">Visual Choice</see></returns>
	inline bool HasChoice() const
	{
		const uint8 AsInt = StaticCast<uint8>(EScenarioMetaFlags::Choice);
		return (Info.Flags & AsInt) == AsInt;
	}

	/// <summary>
	/// Checks whether or not this scene background has valid <see cref="FBackground::TransitionMaterial"/> assigned.
	/// </summary>
	/// <returns><c>true</c> if scene background has valid <see cref="FBackground::TransitionMaterial"/></returns>
	inline bool HasTransition() const
	{
		return !Info.Background.TransitionMaterial.IsNull();
	}

	virtual void AssignScenarioVisualInfo(const FVisualScenarioInfo& InInfo) override
	{
		Info.Author = InInfo.Author;
		Info.Line = InInfo.Line;
		Info.Sound = InInfo.Sound;
		Info.Background = InInfo.Background;
		Info.SpritesParams = InInfo.SpritesParams;
	}

	FORCEINLINE void Serialize(FArchive& Ar)
	{
		Ar << *this;
	}

	FORCEINLINE friend FArchive& operator<< (FArchive& Ar, FScenario& Scenario)
	{
		return Ar << Scenario.Info;
	}

	FORCEINLINE bool operator== (const FScenario& Other) const
	{
		return Info == Other.Info;
	}

	FORCEINLINE bool operator!= (const FScenario& Other) const
	{
		return !(*this == Other);
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
		InDataTable->GetAllRows(UE_SOURCE_LOCATION, Rows);
		Rows.Find(this, Index);
	}
};
