// Copyright (c) 2024 Evgeny Shustov


#include "VisualSocket.h"
#include "Components/PanelSlot.h"
#include "SVisualSocket.h"
#include "Components/ScaleBoxSlot.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "VisualImage.h"
#include "PaperSprite.h"
#include "PaperFlipbook.h"

#define LOCTEXT_NAMESPACE "VisualU"

UVisualSocket::UVisualSocket(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	SocketOffset(ForceInitToZero),
	bAutoPositioning(false),
	ImageDesiredPosition(ForceInitToZero),
	SlateVisualSocket(nullptr)
{
	SetStretch(EStretch::Fill);
}

void UVisualSocket::SetSocketOffset(FVector2D InSocketPosition)
{
	SocketOffset = InSocketPosition;
}

void UVisualSocket::SetAutoPositioning(bool ShouldAutoPosition)
{
	bAutoPositioning = ShouldAutoPosition;
}

void UVisualSocket::SetImageDesiredPosition(FVector2D Position)
{
	ImageDesiredPosition = Position;
}

#if WITH_EDITOR
const FText UVisualSocket::GetPaletteCategory()
{
	return LOCTEXT("VisualSocketCategory", "VisualU");
}
#endif

void UVisualSocket::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (bAutoPositioning && GetChildrenCount() > 0)
	{
		UWidget* ChildWidget = GetChildAt(0);

		if (ChildWidget->IsA<UVisualImage>())
		{
			UVisualImage* ChildImage = Cast<UVisualImage>(ChildWidget);
			if (SlateVisualSocket.IsValid())
			{
				SlateVisualSocket->SetVisualImage(ChildImage);
				SlateVisualSocket->SetImageDesiredPosition(ImageDesiredPosition);
			}
		}
	}

	if (SlateVisualSocket.IsValid())
	{
		SlateVisualSocket->SetSocketOffset(SocketOffset);
	}
}

void UVisualSocket::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	SlateVisualSocket.Reset();
}

TSharedRef<SWidget> UVisualSocket::RebuildWidget()
{
	SlateVisualSocket = SNew(SVisualSocket);
	MyScaleBox = SlateVisualSocket;
    
	if (GetChildrenCount() > 0)
	{
		Cast<UScaleBoxSlot>(GetContentSlot())->BuildSlot(MyScaleBox.ToSharedRef());
	}

	return MyScaleBox.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE