// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncLoadable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

// Add default functionality here for any IAsyncLoadable functions that are not pure virtual.

TSharedPtr<FStreamableHandle> IAsyncLoadable::AsyncLoad(const FSoftObjectPath& ObjectPath, FStreamableDelegate AfterLoadDelegate, TAsyncLoadPriority Priority)
{
	if (ObjectPath == nullptr || ObjectPath.ResolveObject())
	{
		AfterLoadDelegate.ExecuteIfBound();
		return nullptr;
	}

	TWeakObjectPtr<UObject> WeakThis = TWeakObjectPtr<UObject>(this->_getUObject());
	const auto Callback = [WeakThis, AfterLoadDelegate]()
	{
		if (WeakThis.IsValid())
		{
			AfterLoadDelegate.ExecuteIfBound();
		}
	};

	return UAssetManager::GetStreamableManager().RequestAsyncLoad(ObjectPath, Callback, Priority);
}
