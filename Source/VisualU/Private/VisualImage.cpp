// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualImage.h"
#include "SVisualImage.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"

#define LOCTEXT_NAMESPACE "Visual U"

UVisualImage::UVisualImage(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ColorAndOpacity = FLinearColor::White;
	DesiredScale = FVector2D::One();
	MirrorScale = FVector2D::One();
	bAnimate = false;
	FrameIndex = 0;
}

void UVisualImage::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	VisualImageSlate.Reset();
}

void UVisualImage::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	TAttribute<FSlateColor> ColorAndOpacityAttribute = PROPERTY_BINDING(FSlateColor, ColorAndOpacity);
	TAttribute<const UPaperFlipbook*> FlipbookAttribute = OPTIONAL_BINDING_CONVERT(UPaperFlipbook*, Flipbook, const UPaperFlipbook*, ToFlipbook);

	if (VisualImageSlate.IsValid())
	{
		VisualImageSlate->SetFlipbook(FlipbookAttribute);
		VisualImageSlate->SetColorAndOpacity(ColorAndOpacityAttribute);
		VisualImageSlate->SetAnimate(bAnimate);
		VisualImageSlate->SetSpriteIndex(FrameIndex);
		VisualImageSlate->SetDesiredScale(DesiredScale);
		VisualImageSlate->SetMirrorScale(MirrorScale);
	}
}

TSharedRef<SWidget> UVisualImage::RebuildWidget()
{
	VisualImageSlate = SNew(SVisualImage);

	return VisualImageSlate.ToSharedRef();
}

void UVisualImage::SetAnimate(bool IsAnimated)
{
	bAnimate = IsAnimated;

	if (VisualImageSlate.IsValid())
	{
		VisualImageSlate->SetAnimate(IsAnimated);
	}
}

void UVisualImage::SetFrameIndex(int Index)
{
	FrameIndex = Index;

	if (VisualImageSlate.IsValid())
	{
		VisualImageSlate->SetSpriteIndex(Index);
	}
}

void UVisualImage::SetFlipbook(TObjectPtr<UPaperFlipbook> InFlipbook)
{
	Flipbook = InFlipbook;

	if (VisualImageSlate.IsValid())
	{
		VisualImageSlate->SetFlipbook(Flipbook);
	}
}

void UVisualImage::SetFlipbook(UPaperFlipbook* InFlipbook)
{
	Flipbook = InFlipbook;

	if (VisualImageSlate.IsValid())
	{
		VisualImageSlate->SetFlipbook(Flipbook);
	}
}

void UVisualImage::AsyncLoad(TSoftObjectPtr<UPaperFlipbook> SoftFlipbook, FStreamableDelegate AfterLoadDelegate)
{
	CancelAsyncLoad();

	if (SoftFlipbook.IsValid())
	{
		AfterLoadDelegate.ExecuteIfBound();
		return;
	}

	TWeakObjectPtr<UVisualImage> WeakThis = TWeakObjectPtr<UVisualImage>(this);
	const FSoftObjectPath FlipbookPath = SoftFlipbook.ToSoftObjectPath();

	FlipbookHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(FlipbookPath, [WeakThis, AfterLoadDelegate]()
	{
		if (WeakThis.IsValid())
		{	
			AfterLoadDelegate.ExecuteIfBound();
		}

	}, FStreamableManager::AsyncLoadHighPriority);
}

void UVisualImage::CancelAsyncLoad()
{
	if (FlipbookHandle.IsValid())
	{
		FlipbookHandle->CancelHandle();
		FlipbookHandle.Reset();
	}
}

bool UVisualImage::IsFlipbookLoading() const
{
	if (FlipbookHandle.IsValid())
	{
		return FlipbookHandle->IsLoadingInProgress();
	}

	return false;
}

bool UVisualImage::IsFlipbookLoaded() const
{
	if (FlipbookHandle.IsValid())
	{
		return FlipbookHandle->HasLoadCompleted();
	}
	
	return false;
}

void UVisualImage::SetFlipbookAsync(TSoftObjectPtr<UPaperFlipbook> InFlipbook)
{
	TWeakObjectPtr<UVisualImage> WeakThis = TWeakObjectPtr<UVisualImage>(this);
	FStreamableDelegate OnFlipbookLoaded{};

	OnFlipbookLoaded.BindLambda([WeakThis, InFlipbook]()
	{
		if (UVisualImage* Pinned = WeakThis.Get())
		{
			Pinned->SetFlipbook(InFlipbook.Get());
		}
	});

	AsyncLoad(InFlipbook, OnFlipbookLoaded);
}

void UVisualImage::SetColorAndOpacity(const FLinearColor& InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;

	if (VisualImageSlate.IsValid())
	{
		VisualImageSlate->SetColorAndOpacity(InColorAndOpacity);
	}
}

void UVisualImage::SetDesiredScale(const FVector2D& InDesiredScale)
{
	DesiredScale = InDesiredScale;

	if (VisualImageSlate.IsValid())
	{
		VisualImageSlate->SetDesiredScale(InDesiredScale);
	}
}

void UVisualImage::SetMirrorScale(const FVector2D& InMirrorScale)
{
	MirrorScale = InMirrorScale;

	if (VisualImageSlate.IsValid())
	{
		VisualImageSlate->SetMirrorScale(InMirrorScale);
	}
}

const UPaperFlipbook* UVisualImage::ToFlipbook(TAttribute<UPaperFlipbook*> InFlipbook) const
{	
	Flipbook = InFlipbook.Get();

	return Flipbook;
}

UPaperSprite* UVisualImage::GetCurrentSprite() const
{
	if (VisualImageSlate.IsValid())
	{
		return VisualImageSlate->GetCurrentSprite();
	}

	return nullptr;
}

#if WITH_EDITOR
const FText UVisualImage::GetPaletteCategory()
{
	return LOCTEXT("Visual U", "Visual U");
}
#endif

#undef LOCTEXT_NAMESPACE
