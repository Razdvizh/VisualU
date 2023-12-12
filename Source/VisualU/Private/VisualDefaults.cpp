// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualDefaults.h"

FAnchors FVisualAnchors::Default = FAnchors();
FAnchors FVisualAnchors::FullScreen = FAnchors(0, 0, 1, 1);
FAnchors FVisualAnchors::BottomLeft = FAnchors(0, 1, 0, 1);
FAnchors FVisualAnchors::TopLeft = FAnchors(0, 0, 0, 0);
FAnchors FVisualAnchors::MiddleLeft = FAnchors(0, 0.5, 0, 0.5);
FAnchors FVisualAnchors::BottomRight = FAnchors(1, 1, 1, 1);
FAnchors FVisualAnchors::TopRight = FAnchors(1, 0, 1, 0);
FAnchors FVisualAnchors::MiddleRight = FAnchors(1, 0.5, 1, 0.5);
FAnchors FVisualAnchors::BottomCenter = FAnchors(0.5, 1, 0.5, 1);
FAnchors FVisualAnchors::TopCenter = FAnchors(0.5, 0, 0.5, 0);
FAnchors FVisualAnchors::Center = FAnchors(0.5, 0.5, 0.5, 0.5);
FAnchors FVisualAnchors::TopHorizontal = FAnchors(0, 0, 1, 0);
FAnchors FVisualAnchors::MiddleHorizontal = FAnchors(0, 0.5, 1, 0.5);
FAnchors FVisualAnchors::BottomHorizontal = FAnchors(0, 1, 1, 1);
FAnchors FVisualAnchors::LeftVertical = FAnchors(0, 0, 0, 1);
FAnchors FVisualAnchors::CenterVertical = FAnchors(0.5, 0, 0.5, 1);
FAnchors FVisualAnchors::RightVertical = FAnchors(1, 0, 1, 1);

FMargin FVisualMargin::Zero = FMargin(0, 0);