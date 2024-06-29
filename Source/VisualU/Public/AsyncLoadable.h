// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/StreamableManager.h"
#include "AsyncLoadable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAsyncLoadable : public UInterface
{
	GENERATED_BODY()
};


/// <summary>
/// Indicates use and support for asynchronous asset loading.
/// To be revisited.
/// </summary>
class IAsyncLoadable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/// <summary>
	/// Asynchronously load asset from disk.
	/// </summary>
	/// <param name="ObjectPath">Path to the asset that should be loaded</param>
	/// <param name="AfterLoadDelegate">Delegate to call after asset is loaded. Note: it will be called if asset is already in memory</param>
	/// <param name="Priority">Priority for async loading</param>
	/// <returns>Handle to the loaded asset. When handle gone, asset is released.</returns>
	virtual TSharedPtr<FStreamableHandle> AsyncLoad(const FSoftObjectPath& ObjectPath, FStreamableDelegate AfterLoadDelegate, TAsyncLoadPriority Priority);
};
