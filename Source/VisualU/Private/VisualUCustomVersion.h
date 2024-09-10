// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

struct FVisualUCustomVersion
{
public:
	enum Type
	{
		//--<add new versions above this line>------------------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	const static FGuid GUID;
};
