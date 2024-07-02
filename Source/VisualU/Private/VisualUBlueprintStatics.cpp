// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualUBlueprintStatics.h"
#include "PaperSprite.h"
#include "Scenario.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "VisualController.h"
#include "Engine/DataTable.h"

UTexture2D* UVisualUBlueprintStatics::GetSpriteTexture(UPaperSprite* Sprite)
{
	return Sprite->GetBakedTexture();
}

void UVisualUBlueprintStatics::PrintScenesData()
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	TArray<FAssetData> ScenesData;
	GetScenesData(ScenesData);

	for (const FAssetData& Asset : ScenesData)
	{
		const UDataTable* DataTable = Cast<UDataTable>(Asset.GetAsset());
		TArray<FScenario*> Rows;
		
		DataTable->GetAllRows(UE_SOURCE_LOCATION, Rows);

		UE_LOG(LogVisualU, Display, TEXT("%s"), *Asset.AssetName.ToString());

		int32 cnt = 0;
		for (const FScenario* Row : Rows)
		{
			cnt++;
			UE_LOG(LogVisualU, Display, TEXT("\tRow %d"), cnt);
			Row->PrintLog();
			UE_LOG(LogVisualU, Display, TEXT("================================================="));
		}
	}
#endif
}

bool UVisualUBlueprintStatics::Choose(UVisualController* Controller, const UDataTable* DataTable)
{
	check(Controller);
	check(DataTable);
	return Controller->RequestNode(DataTable);
}

void UVisualUBlueprintStatics::GetScenesData(TArray<FAssetData>& OutData)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;

	const FName AssetName = UDataTable::StaticClass()->GetFName();
	const FName PackageName = UDataTable::StaticClass()->GetPackage()->GetFName();
	const FName Key = TEXT("RowStructure");
	const FString Value = TEXT("Scenario");

	Filter.ClassPaths.Add(FTopLevelAssetPath(PackageName, AssetName));
	Filter.bIncludeOnlyOnDiskAssets = true;
	Filter.TagsAndValues.AddUnique(Key, Value);

	if (IsInGameThread())
	{
		AssetRegistryModule.Get().GetAssets(Filter, OutData);
	}
}
