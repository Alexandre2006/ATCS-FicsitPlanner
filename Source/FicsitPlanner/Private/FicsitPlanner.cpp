// Copyright Epic Games, Inc. All Rights Reserved.

#include "FicsitPlanner.h"

#include "GameInstanceModuleManager.h"

#define LOCTEXT_NAMESPACE "FFicsitPlannerModule"

void FFicsitPlannerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// DEBUG: Log "Hello World" to the output log
	UE_LOG(LogTemp, Warning, TEXT("Hello World from FicsitPlanner!"));

	// Get the main root world component
	UGameInstanceModuleManager * module = UGameInstanceModuleManager::FindModule("FicsitPlanner");
	
}

void FFicsitPlannerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFicsitPlannerModule, FicsitPlanner)