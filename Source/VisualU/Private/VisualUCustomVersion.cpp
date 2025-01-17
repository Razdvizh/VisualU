// Copyright (c) 2024 Evgeny Shustov


#include "VisualUCustomVersion.h"
#include "UObject/DevObjectVersion.h"

const FGuid FVisualUCustomVersion::GUID(0xDA06AED2, 0xFF31AE8C, 0x38A2C1BE, 0x6DAA21EC);

FDevVersionRegistration GVisualUCustomVersionRegistration(FVisualUCustomVersion::GUID, FVisualUCustomVersion::LatestVersion, TEXT("VisualUVersion"));
