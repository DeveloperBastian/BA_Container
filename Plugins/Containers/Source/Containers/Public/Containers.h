// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FContainersModule : public IModuleInterface
{
public:
	static inline TSharedPtr<FSlateStyleSet> BA_StyleSet = nullptr;
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
