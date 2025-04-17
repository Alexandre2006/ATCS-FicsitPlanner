#include "FicsitPlanner.h"
#include "AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FFicsitPlannerModule"

void FFicsitPlannerModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Hello World from FicsitPlanner!"));

	// Get the Asset Registry Module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<
		FAssetRegistryModule>("AssetRegistry");

	// Ensure the asset registry is fully loaded
	AssetRegistryModule.Get().SearchAllAssets(true);

	// Retrieve all assets
	TArray<FAssetData> AssetDataArray;
	AssetRegistryModule.Get().GetAllAssets(AssetDataArray, true);

	// Check if any assets were found
	if (AssetDataArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No assets found."));
		return;
	}

	// Iterate through the assets and log their names and types
	for (const FAssetData& AssetData : AssetDataArray)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Asset: %s, Type: %i"), *AssetData.GetObjectPathString(),
		       AssetData.IsAssetLoaded());
	}
}

void FFicsitPlannerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module. For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFicsitPlannerModule, FicsitPlanner)
