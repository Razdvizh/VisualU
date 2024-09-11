// Copyright (c) 2024 Evgeny Shustov

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

/**
* Describes UVisualSprite for UVisualRenderer. 
*/
USTRUCT(BlueprintType)
struct FSprite
{
	GENERATED_BODY()

public:
	FSprite();

	/**
	* UVisualSprite class to be visualized by UVisualRenderer.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Sprite class to be visualized by Visual Renderer"))
	TSoftClassPtr<UVisualSprite> SpriteClass;

	/**
	* UVisualSprite anchors in Canvas Panel.
	* 
	* @see FVisualAnchors
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Sprite anchors in Canvas Panel"))
	FVisualAnchors Anchors;

	/**
	* UVisualSprite position in Canvas Panel.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Sprite position in Canvas Panel"))
	FVector2D Position;

	/**
	* UVisualSprite Z coordinate (or layer) in Canvas Panel.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Sprite Z coordinate (or layer) in Canvas Panel"))
	int32 ZOrder;

	/**
	* Information for UVisualImages inside UVisualSprite.
	* 
	* @see UVisualSprite::AssignSpriteInfo()
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite", meta = (ToolTip = "Information for Visual Images inside sprite"))
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

	/**
	* String representation of FSprite.
	*/
	inline FString ToString() const
	{
		FString String;
		String = FString::Printf(TEXT("Sprite Class: %s\n"), SpriteClass.IsNull() ? TEXT("None") : *SpriteClass.GetAssetName());
		String += FString::Printf(TEXT("Anchors: %s\n"), *Anchors.ToString());
		String += FString::Printf(TEXT("Position: %s\n"), *Position.ToString());
		String += FString::Printf(TEXT("Z order: %d\n"), ZOrder);
		if (!SpriteInfo.IsEmpty())
		{
			int32 cnt = 0;
			for (const FVisualImageInfo& Info : SpriteInfo)
			{
				cnt++;
				String += FString::Printf(TEXT("Sprite Info %d: %s\n"), cnt, *Info.ToString());
			}
		}

		return String;
	}

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

/**
* Describes what background to display and what effect to use for transition.
*/
USTRUCT(BlueprintType)
struct FBackground
{
	GENERATED_BODY()

public:
	FBackground();

	/**
	* Background info for the scene.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background")
	FVisualImageInfo BackgroundArtInfo;

	/**
	* Transition to play on background when switching to the next scene.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background")
	TSoftObjectPtr<UMaterialInterface> TransitionMaterial;

	/**
	* String representation of FBackground.
	*/
	inline FString ToString() const
	{
		const FString String = FString::Printf(TEXT("%s\nTransition Material: %s"),
			*BackgroundArtInfo.ToString(),
			TransitionMaterial.IsNull() ? TEXT("None") : *TransitionMaterial.GetAssetName());

		return String;
	}

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

/**
* Metadata to describe FScenario.
* 
* @note Not enforced
*/
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

/**
* Data that describes FScenario.
*/
USTRUCT(BlueprintType)
struct FVisualScenarioInfo : public FVisualInfo
{
	GENERATED_BODY()

public:
	FVisualScenarioInfo();
	
	/**
	* An author of the FVisualScenarioInfo::Line.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info", meta = (ToolTip = "An author of the line"))
	FText Author;

	/**
	* A dialog line.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info")
	FText Line;

	/**
	* Audio to play while the scene is displayed.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info")
	TSoftObjectPtr<USoundBase> Sound;

	/**
	* Background to display.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info")
	FBackground Background;

	/**
	* FSprites that the scene has.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info", meta = (ToolTip = "Sprites that the scene has"))
	TArray<FSprite> SpritesParams;

	/**
	* Describes the role of UVisualSprites of the scene.
	* 
	* @note Not enforced
	* 
	* @see EScenarioMetaFlags
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Visual Info", meta = (Bitmask, BitmaskEnum = "/Script/VisualU.EScenarioMetaFlags", ToolTip = "Describes the role of sprites of the scene"))
	uint8 Flags = 0;

	/**
	* Implements Visitor pattern for this info.
	* 
	* @see FVisualInfo::Accept()
	*/
	virtual void Accept(IInfoAssignable* Visitor) const override
	{
		Visitor->AssignScenarioVisualInfo(*this);
	}

	/**
	* String representation of the scene.
	*/
	virtual FString ToString() const override
	{
		FString String;
		String = FString::Printf(TEXT("Author: %s\n"), Author.IsEmpty() ? TEXT("") : *Author.ToString());
		String += FString::Printf(TEXT("Line: %s\n"), Line.IsEmpty() ? TEXT("") : *Line.ToString());
		String += FString::Printf(TEXT("Music: %s\n"), Sound.IsNull() ? TEXT("None") : *Sound.GetAssetName());
		String += (Background.ToString() + TEXT("\n"));

		if (!SpritesParams.IsEmpty())
		{
			int32 cnt = 0;
			for (const FSprite& SpriteParam : SpritesParams)
			{
				cnt++;
				String += FString::Printf(TEXT("Sprite Parameter %d\n%s"), cnt, *SpriteParam.ToString());
			}
		}

		return String;
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

/**
* Describes a single scene/entry/"frame" of UVisualController.
* Struct with data that describes one scene handled by UVisualController.
* Can be imported from CSV or JSON file or added directly 
* as a row to the {@code Data Table} that has {@code FScenario} row struct.
* Terms "Scene" and "Scenario" in this plugin all refer to {@code FScenario}.
* Grouped together under {@code Data Table}, scenarios represent a node in 
* the tree structure, granting ability to choose different narrative paths.
* The first scene, or root, together with its node in the tree is specified
* by UVisualUSettings::FirstDataTable.
* Connections between nodes can be placed in any scene in the current node,
* but make most sense in the last scene of the current node.
* Connections can be created by calling UVisualController::RequestNode().
* Created connections are managed by UVisualController. That said,
* FScenario is a somewhat-intrusive data structure, that is well aware in
* what node it resides. That implies that FScenario must be a part of a node.
* 
* @note FScenario is a polymorphic struct and can be extended if needed.
* 
* @see FSprite
*		   UVisualController
*/
USTRUCT(BlueprintType, meta = (ToolTip = " Describes a single scene/entry/\"frame\" of Visual Controller"))
struct VISUALU_API FScenario : public FTableRowBase, public IInfoAssignable
{
	GENERATED_BODY()

public:
	FScenario();

	/**
	* Actual data of this scenario.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario")
	FVisualScenarioInfo Info;

protected:
	/**
	* Node that owns this scenario. Can be nullptr.
	* 
	* @note Left undiscoverable by reflection to avoid circular references
	*		and intricacies of TWeakObjectPtr.
	*/
	const UDataTable* Owner;

	/**
	* Position of this scenario in the node.
	*/
	int32 Index;

public:
	/*
	* Matches provided scene to its data stored in the data table.
	* 
	* @note Used during serialization
	*/
	static FScenario* ResolveScene(const FScenario& Scene)
	{
		if (ensure(Scene.GetOwner()))
		{
			TArray<FScenario*> Scenes;
			Scene.GetOwner()->GetAllRows(UE_SOURCE_LOCATION, Scenes);
			const int32 Idx = Scene.GetIndex();

			if (ensure(Scenes.IsValidIndex(Idx)))
			{
				return Scenes[Idx];
			}
		}

		return nullptr;
	}

	/*
	* @return Node of this scenario, can be nullptr
	*/
	FORCEINLINE const UDataTable* GetOwner() const { return Owner; }

	/*
	* @return Position of this scenario
	*/
	FORCEINLINE int32 GetIndex() const { return Index; }

	/*
	* @see FScenario::Intrude()
	* 
	* @param InDataTable Data table that imported this row
	* @param InRowName Name of this this row
	* @param OutCollectedImportProblems issues found during import
	*/
	virtual void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems) override
	{
		Intrude(InDataTable);
	}

	/*
	* @see FScenario::Intrude()
	* 
	* @param InDataTable Data table that imported this row
	* @param InRowName Name of this row
	*/
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Intrude(InDataTable);
	}

	/**
	* Provides assets that should be loaded into the memory.
	* 
	* @note said assets might be already loaded. Will empty Out.
	* 
	* @param Array to be filled with data that should be loaded
	*/
	virtual void GetDataToLoad(TArray<FSoftObjectPath>& Out) const
	{
		Out.Reserve(3 + Info.SpritesParams.Num());
		if (!Info.Background.BackgroundArtInfo.Expression.IsNull())
		{
			Out.Emplace(Info.Background.BackgroundArtInfo.Expression.ToSoftObjectPath());
		}
		if (!Info.Background.TransitionMaterial.IsNull())
		{
			Out.Emplace(Info.Background.TransitionMaterial.ToSoftObjectPath());
		}
		if (!Info.Sound.IsNull())
		{
			Out.Emplace(Info.Sound.ToSoftObjectPath());
		}
		for (const FSprite& SpriteParam : Info.SpritesParams)
		{
			if (!SpriteParam.SpriteClass.IsNull())
			{
				Out.Emplace(SpriteParam.SpriteClass.ToSoftObjectPath());
			}
			for (const FVisualImageInfo& ImageInfo : SpriteParam.SpriteInfo)
			{
				if (!ImageInfo.Expression.IsNull())
				{
					Out.Emplace(ImageInfo.Expression.ToSoftObjectPath());
				}
			}
		}
	}

	/**
	* String representation of scenario data.
	*/
	virtual FString ToString() const
	{
		return Info.ToString();
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	/**
	* String representation of this scenario for debugging purposes.
	*/
	FORCEINLINE FString GetDebugString() const
	{
		check(Owner);
		return FString::Printf(TEXT("DataTable: %s, index: %i"), *Owner->GetFName().ToString(), Index);
	}
#endif

	/**
	* @return {@code true} when this scenario has EScenarioMetaFlags::Choice flag.
	*/
	inline bool HasChoice() const
	{
		const uint8 Choice = StaticCast<uint8>(EScenarioMetaFlags::Choice);
		return (Info.Flags & Choice) == Choice;
	}

	/**
	* @return {@code true} when this scenario has valid transition material.
	*/
	inline bool HasTransition() const
	{
		return !Info.Background.TransitionMaterial.IsNull();
	}

	/**
	* Setter for FScenario::Info.
	* 
	* @param InInfo New information for this scenario
	*/
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
		TSoftObjectPtr<UDataTable> SoftOwner;
		if (Ar.IsSaving())
		{
			SoftOwner = Scenario.Owner;
		}

		Ar << SoftOwner;

		if (Ar.IsLoading() && !SoftOwner.IsNull())
		{
			Scenario.Owner = SoftOwner.LoadSynchronous();
		}

		return Ar << Scenario.Index;
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
	/**
	* Makes this scenario aware of its owner and its position.
	* 
	* @param InDataTable the owner of this scenario
	*/
	inline void Intrude(const UDataTable* InDataTable)
	{
		Owner = InDataTable;
		TArray<FScenario*> Rows;
		InDataTable->GetAllRows(UE_SOURCE_LOCATION, Rows);
		Rows.Find(this, Index);
	}
};
