// Copyright (c) 2024 Evgeny Shustov


#include "VisualUBlueprintStatics.h"
#include "PaperSprite.h"
#include "Scenario.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/ARFilter.h"
#include "VisualController.h"
#include "Engine/DataTable.h"
#include "Misc/CoreMiscDefines.h"
#include "VisualVersioningSubsystem.h"
#include "PlatformFeatures.h"
#include "SaveGameSystem.h"
#include "Engine/LocalPlayer.h"
#include "Serialization/Archive.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

UTexture2D* UVisualUBlueprintStatics::GetSpriteTexture(UPaperSprite* Sprite)
{
	if (Sprite)
	{
		return Sprite->GetBakedTexture();
	}

	return nullptr;
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
			UE_LOG(LogVisualU, Display, TEXT("%s"), *Row->ToString());
			UE_LOG(LogVisualU, Display, TEXT("================================================="));
		}
	}
#endif
}

bool UVisualUBlueprintStatics::LoadVisualU(UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController, int32 UserIndex, const FString& Filename)
{
	check(!Filename.IsEmpty());

	ISaveGameSystem* SaveGameSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	check(SaveGameSystem);

	FPlatformUserId UserId = FPlatformMisc::GetPlatformUserForUserIndex(UserIndex);
	TArray<uint8> Data;

	const bool bAttemptToUseNativeUI = true;
	if (SaveGameSystem->LoadGame(
		bAttemptToUseNativeUI,
		*Filename,
		UserId,
		Data))
	{
		FMemoryReader MemoryReader = FMemoryReader(Data);
		SerializeVisualU(MemoryReader, VersioningSubsystem, VisualController);

		return true;
	}

	return false;
}

bool UVisualUBlueprintStatics::SaveVisualU(UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController, int32 UserIndex, const FString& Filename)
{
	check(!Filename.IsEmpty());

	ISaveGameSystem* SaveGameSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	check(SaveGameSystem);

	FPlatformUserId UserId = FPlatformMisc::GetPlatformUserForUserIndex(UserIndex);

	TArray<uint8> Data;
	FMemoryWriter MemoryWriter = FMemoryWriter(Data);
	SerializeVisualU(MemoryWriter, VersioningSubsystem, VisualController);
	FPlatformMisc::GetPlatformUserForUserIndex(UserIndex);

	const bool bAttemptToUseNativeUI = true;
	return SaveGameSystem->SaveGame(
		bAttemptToUseNativeUI,
		*Filename,
		UserId,
		Data);
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

void UVisualUBlueprintStatics::SerializeVisualU(FArchive& Ar, UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController)
{
	check(VersioningSubsystem);
	check(VisualController);

	FObjectAndNameAsStringProxyArchive ProxyAr(Ar, /*bInLoadIfFindFails=*/false);
	VersioningSubsystem->SerializeSubsystem(ProxyAr);
	VisualController->SerializeController(ProxyAr);
}
