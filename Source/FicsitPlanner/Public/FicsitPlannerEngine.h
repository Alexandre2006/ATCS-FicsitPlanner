#pragma once

#include "CoreMinimal.h"
#include "FGRecipe.h"
#include "FicsitPlannerFactory.h"
#include "Subsystem/ModSubsystem.h"
#include "FicsitPlannerEngine.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogFicsitPlannerEngine, Log, All);

/**
 * Backend for Ficsit Planner!
 * Handles loading data and generating factories.
 * NOTE: Created once per world, like a singleton.
 */
UCLASS()
class FICSITPLANNER_API AFicsitPlannerEngine : public AModSubsystem
{
	GENERATED_BODY()

public:
	// Constructor / Destructor
	AFicsitPlannerEngine();
	virtual ~AFicsitPlannerEngine() override;

	// Initialization
	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner")
	void Initialize();

	// Reload game data
	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner")
	void ReloadGameData();

	// Getters for recipe maps & item lists
	const TMap<TSubclassOf<UFGItemDescriptor>, TArray<TSubclassOf<UFGRecipe>>>& GetUnlockedRecipesMap() const { return UnlockedRecipesMap; }
	const TMap<TSubclassOf<UFGItemDescriptor>, TArray<TSubclassOf<UFGRecipe>>>& GetAllRecipesMap() const { return AllRecipesMap; }
	const TArray<TSubclassOf<UFGItemDescriptor>>& GetUnlockedItems() const { return UnlockedItems; }
	const TArray<TSubclassOf<UFGItemDescriptor>>& GetAllItems() const { return AllItems; }

	// Utilities for calculating factory stats
	static float GetFactoryMultiplier(const TSubclassOf<UFGRecipe> Recipe, FItemAmount Target);
	static float GetFactoryPowerConsumption(const TSubclassOf<UFGRecipe> Recipe, float Multiplier);

	// Factory Generation
	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Create Factory Plan"))
	UFicsitPlannerFactory* CreateFactoryPlan(FItemAmount TargetProduct, bool bAllowLockedRecipes, EFactoryOptimizationType Optimization = EFactoryOptimizationType::EFOT_None);

	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Log Factory Plan"))
	void LogFactoryPlan(UFicsitPlannerFactory* Factory);

	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Factory Plan to String"))
	FString FactoryPlanToString(UFicsitPlannerFactory* Factory);

	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Factory Plan to Header String"))
	FString FactoryPlanToHeaderString(UFicsitPlannerFactory* Factory);

	// Factory Management (CLI Commands)
	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Save Factory"))
	int SaveFactory(UFicsitPlannerFactory* Factory);

	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Get Factory"))
	UFicsitPlannerFactory* GetFactory(int FactoryID);

	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Delete Factory"))
	bool DeleteFactory(int FactoryID);

	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Get All Factories"))
	TArray<UFicsitPlannerFactory*> GetAllFactories();

	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Replace Factory at index"))
	bool ReplaceFactory(int FactoryID, UFicsitPlannerFactory* Factory);

	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Update Factory Group"))
	bool UpdateFactoryGroup(int FactoryID, int nodeID, int newIndex);

	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Optimize Factory"))
	bool OptimizeFactory(int Factory, EFactoryOptimizationType Optimization);

	// Item Search
	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner", meta = (DisplayName = "Find Item"))
	TSubclassOf<UFGItemDescriptor> FindItem(const FString& ItemName);

	
	
	

private:
	// Items / Recipes
	TArray<TSubclassOf<UFGItemDescriptor>> UnlockedItems;
	TArray<TSubclassOf<UFGItemDescriptor>> AllItems;
	TMap<TSubclassOf<UFGItemDescriptor>, TArray<TSubclassOf<UFGRecipe>>> UnlockedRecipesMap;
	TMap<TSubclassOf<UFGItemDescriptor>, TArray<TSubclassOf<UFGRecipe>>> AllRecipesMap;

	// Saved Factories
	UPROPERTY()
	TArray<UFicsitPlannerFactory*> Factories;

	// Helpers
	static bool RecipeProducedInBuilding(TSubclassOf<UFGRecipe> Recipe);
	void RegisterUnlockedRecipe(TSubclassOf<UFGRecipe> Recipe);
	void RegisterRecipe(TSubclassOf<UFGRecipe> Recipe);
};
