// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualImage.h"
#include "SVisualImage.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"

#define LOCTEXT_NAMESPACE "Visual U"

UVisualImage::UVisualImage(const FObjectInitializer& ObjectInitializer) 
	: UVisualImageBase(ObjectInitializer),
	Flipbook(nullptr)
{
	ColorAndOpacity = FLinearColor::White;
	DesiredScale = FVector2D::One();
	MirrorScale = FVector2D::One();
	bAnimate = false;
	FrameIndex = 0;
}

void UVisualImage::ReleaseSlateResources(bool bReleaseChildren)
{
	UVisualImageBase::ReleaseSlateResources(bReleaseChildren);
	
	Flipbook = nullptr;
	CancelAsyncLoad();
	VisualImageSlate.Reset();
}

void UVisualImage::SynchronizeProperties()
{
	UVisualImageBase::SynchronizeProperties();

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

TSharedPtr<FStreamableHandle> UVisualImage::AsyncLoadFlipbook(TSoftObjectPtr<UPaperFlipbook> FlipbookToLoad, FStreamableDelegate AfterLoadDelegate, TAsyncLoadPriority Priority)
{
	check(!FlipbookToLoad.IsNull());

	return UAssetManager::GetStreamableManager().RequestAsyncLoad(FlipbookToLoad.ToSoftObjectPath(), AfterLoadDelegate, Priority);
}

void UVisualImage::AssignVisualImageInfo(const FVisualImageInfo& InInfo)
{
	SetFlipbookAsync(InInfo.Expression);
	SetColorAndOpacity(InInfo.ColorAndOpacity);
	SetDesiredScale(InInfo.DesiredScale);
	SetMirrorScale(InInfo.MirrorScale);
	SetAnimate(InInfo.bAnimate);
	SetFrameIndex(InInfo.FrameIndex);
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

void UVisualImage::SetFlipbook(UPaperFlipbook* InFlipbook)
{
	Flipbook = InFlipbook;

	if (VisualImageSlate.IsValid())
	{
		VisualImageSlate->SetFlipbook(Flipbook.Get());
	}
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
	if (ensureMsgf(!InFlipbook.IsNull(), TEXT("Failed to load flipbook, soft pointer is pointing to null.")))
	{
		FStreamableDelegate OnFlipbookLoaded;

		OnFlipbookLoaded.BindWeakLambda(this, [this, InFlipbook]()
		{
			SetFlipbook(InFlipbook.Get());
		});

		FlipbookHandle = AsyncLoadFlipbook(InFlipbook, OnFlipbookLoaded, FStreamableManager::AsyncLoadHighPriority);
	}
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
	return LOCTEXT("VisualUCategory", "Visual U");
}
#endif

#undef LOCTEXT_NAMESPACE
