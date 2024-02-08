// Copyright Epic Games, Inc. All Rights Reserved.

#include "Containers.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FContainersModule"

void FContainersModule::StartupModule()
{
	// find Icon path
	auto ProjectPluginDir = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(FPaths::ProjectPluginsDir() + "Containers/Resources/Icons/"));
	if (!FPaths::DirectoryExists(ProjectPluginDir))
	{
		UE_LOG(LogTemp, Error, TEXT("Containers.cpp - Slate: Icon directory not found at '%s'"), *ProjectPluginDir);
	}
	else
	{
		// name the style set
		BA_StyleSet = MakeShareable(new FSlateStyleSet("BastianDev_Container"));
		// Assign the icon dir 
		BA_StyleSet->SetContentRoot(ProjectPluginDir);
		UE_LOG(LogTemp, Log, TEXT("Containers.cpp - Slate: Icon directory found at '%s'"), *ProjectPluginDir);
		// repeat the below region for each icon you want to register for your classes
#pragma region class Icons
		const FVector2D Icon64x64(64.f, 64.f);
		const FVector2D Icon16x16(16.0f, 16.0f);

		// TArray
		BA_StyleSet->Set("ClassIcon.TArray"
			, new FSlateImageBrush(BA_StyleSet->RootToContentDir("Array.png"), Icon16x16));
		BA_StyleSet->Set("ClassThumbnail.TArray"
			, new FSlateImageBrush(BA_StyleSet->RootToContentDir("Array.png"), Icon64x64));
		// TMap
		BA_StyleSet->Set("ClassIcon.TMap"
			, new FSlateImageBrush(BA_StyleSet->RootToContentDir("Map.png"), Icon16x16));
		BA_StyleSet->Set("ClassThumbnail.TMap"
			, new FSlateImageBrush(BA_StyleSet->RootToContentDir("Map.png"), Icon64x64));
		// TMultiMap
		BA_StyleSet->Set("ClassIcon.TMultiMap"
			, new FSlateImageBrush(BA_StyleSet->RootToContentDir("MultiMap.png"), Icon16x16));
		BA_StyleSet->Set("ClassThumbnail.TMultiMap"
			, new FSlateImageBrush(BA_StyleSet->RootToContentDir("MultiMap.png"), Icon64x64));
		// TQueue
		BA_StyleSet->Set("ClassIcon.TQueue"
			, new FSlateImageBrush(BA_StyleSet->RootToContentDir("Queue.png"), Icon16x16));
		BA_StyleSet->Set("ClassThumbnail.TQueue"
			, new FSlateImageBrush(BA_StyleSet->RootToContentDir("Queue.png"), Icon64x64));
		// TSet
		BA_StyleSet->Set("ClassIcon.TSet"
			, new FSlateImageBrush(BA_StyleSet->RootToContentDir("Set.png"), Icon16x16));
		BA_StyleSet->Set("ClassThumbnail.TSet"
			, new FSlateImageBrush(BA_StyleSet->RootToContentDir("Set.png"), Icon64x64));
#pragma endregion

		// register
		FSlateStyleRegistry::RegisterSlateStyle(*BA_StyleSet);
	}
}

void FContainersModule::ShutdownModule()
{
	// unregister
	FSlateStyleRegistry::UnRegisterSlateStyle(*BA_StyleSet);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FContainersModule, Containers)