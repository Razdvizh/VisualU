// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "VisualSprite.h"
#include "VisualChoice.h"
#include "Scenario.generated.h"

class UPaperFlipbook;
class UWidgetAnimation;
class USoundCue;

USTRUCT(BlueprintType)
struct VISUALU_API FSprite
{
	GENERATED_USTRUCT_BODY()

public:
	FSprite() : Position(ForceInit), ZOrder(0) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	TSubclassOf<UVisualSprite> SpriteClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	FVector2D Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	int32 ZOrder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	TArray<TSoftObjectPtr<UPaperFlipbook>> Expressions;

	FORCEINLINE bool operator== (const FSprite& Other)
	{
		if (SpriteClass == Other.SpriteClass
			&& Position == Other.Position)
		{
			for (int i = 0; i < Expressions.Num(); i++)
			{
				if (!Other.Expressions.IsValidIndex(i) || Expressions[i] != Other.Expressions[i])
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

USTRUCT(BlueprintType)
struct VISUALU_API FScenario : public FTableRowBase
{

	GENERATED_USTRUCT_BODY()

public:
	FScenario() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	FText Author;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	FText Line;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	TSoftObjectPtr<USoundBase> SceneStartAudioEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	FString SceneStartAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	FString SceneEndAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	TSoftObjectPtr<USoundBase> SceneEndAudioEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	TSoftObjectPtr<USoundBase> Music;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Scene")
	TSoftObjectPtr<UPaperFlipbook> BackgroundArt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	TArray<FSprite> SpritesParams;

	const UDataTable* Owner;

	int32 Index;

	/*TODO: Add assertions that the last Scenario in the data table has at least one Visual Sprite at the end.
	* Visual Choice is intended to be used as link between branches, however it is possible to change branch without any sprite - call method after some event, etc.
	* The last data table doesn't need any links? (Probably link to the game ending related content?)
	*/
	virtual void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems) override
	{
		Intrusive(InDataTable);
	}

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Intrusive(InDataTable);
	}

	FORCEINLINE bool operator== (const FScenario& Other)
	{
		if (Author == Other.Author
			&& Line.CompareTo(Other.Line)
			&& BackgroundArt.GetAssetName() == Other.BackgroundArt.GetAssetName()
			&& Music.GetAssetName() == Other.Music.GetAssetName()
			&& SceneStartAudioEffect == Other.SceneStartAudioEffect
			&& SceneEndAudioEffect == Other.SceneEndAudioEffect
			&& SceneStartAnimation == Other.SceneStartAnimation
			&& SceneEndAnimation == Other.SceneEndAnimation)
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

	virtual void GetDataToLoad(TArray<FSoftObjectPath>& Out) const
	{
		Out.Empty();
		Out.Emplace(BackgroundArt.ToSoftObjectPath());
		if (!Music.IsNull())
		{
			Out.Emplace(Music.ToSoftObjectPath());
		}
		if (!SceneStartAudioEffect.IsNull())
		{
			Out.Emplace(SceneStartAudioEffect.ToSoftObjectPath());
		}
		if (!SceneEndAudioEffect.IsNull())
		{
			Out.Emplace(SceneEndAudioEffect.ToSoftObjectPath());
		}
		for (const auto& SpriteParam : SpritesParams)
		{
			for (const auto& Expression : SpriteParam.Expressions)
			{
				Out.Emplace(Expression.ToSoftObjectPath());
			}
		}
	}

	virtual void ToString() const
	{
		UE_LOG(LogTemp, Warning, TEXT("\tAuthor: %s"), !Author.IsEmpty() ? *Author : TEXT("None"));
		UE_LOG(LogTemp, Warning, TEXT("\tLine: %s"), !Line.IsEmpty() ? *Line.ToString() : TEXT("None"));
		UE_LOG(LogTemp, Warning, TEXT("\tMusic: %s"), !Music.IsNull() ? *Music.GetAssetName() : TEXT("None"));
		UE_LOG(LogTemp, Warning, TEXT("\tBackground Art: %s"), *BackgroundArt.GetAssetName());
		UE_LOG(LogTemp, Warning, TEXT("\tScene Start Audio: %s"), !SceneStartAudioEffect.IsNull() ? *SceneStartAudioEffect.GetAssetName() : TEXT("None"));
		UE_LOG(LogTemp, Warning, TEXT("\tScene End Audio: %s"), !SceneEndAudioEffect.IsNull() ? *SceneEndAudioEffect.GetAssetName() : TEXT("None"));
		UE_LOG(LogTemp, Warning, TEXT("\tScene Start Animation: %s"), !SceneStartAnimation.IsEmpty() ? *SceneStartAnimation : TEXT("None"));
		UE_LOG(LogTemp, Warning, TEXT("\tScene End Animation: %s"), !SceneEndAnimation.IsEmpty() ? *SceneEndAnimation : TEXT("None"));

		if (!SpritesParams.IsEmpty())
		{
			for (const auto& SpriteParam : SpritesParams)
			{
				UE_LOG(LogTemp, Warning, TEXT("\tSprite Class: %s"), SpriteParam.SpriteClass ? *SpriteParam.SpriteClass->GetFName().ToString() : TEXT("None"));
				UE_LOG(LogTemp, Warning, TEXT("\tPosition: %s"), *SpriteParam.Position.ToString());
				if (!SpriteParam.Expressions.IsEmpty())
				{
					for (const auto& Expression : SpriteParam.Expressions)
					{
						UE_LOG(LogTemp, Warning, TEXT("\tExpression: %s"), !Expression.IsNull() ? *Expression.GetAssetName() : TEXT("None"));
					}
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("================================================="));
	}

	inline bool hasChoice() const
	{
		if (!SpritesParams.IsEmpty())
		{
			for (const auto& SpriteParam : SpritesParams)
			{
				if (SpriteParam.SpriteClass->IsChildOf<UVisualChoice>())
				{
					return true;
				}
			}
		}
		
		return false;
	}

private:
	inline void Intrusive(const UDataTable* InDataTable)
	{
		Owner = InDataTable;
		TArray<FScenario*> Rows;
		InDataTable->GetAllRows(TEXT("Scenario.h(219)"), Rows);
		Rows.Find(this, Index);
	}
};
