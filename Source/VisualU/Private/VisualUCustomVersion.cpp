// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualUCustomVersion.h"
#include "Serialization/CustomVersion.h"

const FGuid FVisualUCustomVersion::GUID(0xDA06AED2, 0xFF31AE8C, 0x38A2C1BE, 0x6DAA21EC);

FDevVersionRegistration GVisualUCustomVersionRegistration(FVisualUCustomVersion::GUID, FVisualUCustomVersion::LatestVersion, TEXT("VisualUVersion"));
