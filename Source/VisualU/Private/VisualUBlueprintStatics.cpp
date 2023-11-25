// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualUBlueprintStatics.h"
#include "PaperSprite.h"
#include "Scenario.h"
#include "AssetRegistry/AssetRegistryModule.h"

UTexture2D* UVisualUBlueprintStatics::GetSpriteTexture(UPaperSprite* Sprite)
{
	return Sprite->GetBakedTexture();
}

void UVisualUBlueprintStatics::PrintScenesData()
{
	TArray<FAssetData> ScenesData;
	GetScenesData(ScenesData);

	for (const auto& Asset : ScenesData)
	{
		const UDataTable* DataTable = Cast<UDataTable>(Asset.GetAsset());
		TArray<FScenario*> Rows;
		
		DataTable->GetAllRows(UE_SOURCE_LOCATION, Rows);

		UE_LOG(LogVisualU, Warning, TEXT("%s"), *Asset.AssetName.ToString());

		int cnt = 0;
		for (const auto Row : Rows)
		{
			cnt++;
			UE_LOG(LogVisualU, Warning, TEXT("\tRow %d"), cnt);
			Row->PrintLog();
			UE_LOG(LogVisualU, Warning, TEXT("================================================="));
		}
	}
}

void UVisualUBlueprintStatics::GetScenesData(TArray<FAssetData>& OutData)
{
	FAssetRegistryModule* AssetRegistryModule = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	FARFilter Filter;

	const FName Name = UDataTable::StaticClass()->GetFName();
	const FName Key = TEXT("RowStructure");
	const FString Value = TEXT("Scenario");

	Filter.ClassNames.Add(Name);
	Filter.bIncludeOnlyOnDiskAssets = true;
	Filter.TagsAndValues.AddUnique(Key, Value);

	if (IsInGameThread())
	{
		AssetRegistryModule->Get().GetAssets(Filter, OutData);
	}
}
