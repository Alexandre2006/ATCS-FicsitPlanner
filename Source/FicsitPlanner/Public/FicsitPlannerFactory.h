// Mods/FicsitPlanner/Source/FicsitPlanner/Public/FicsitPlannerFactory.h
#pragma once

#include "CoreMinimal.h"
#include "FGRecipe.h"
#include "ItemAmount.h"
#include "FicsitPlannerEngine.h"
#include "FicsitPlannerFactory.generated.h"

// Forward-declaration to avoid issues
class UFicsitPlannerFactoryGroup;

/**
 * Factory class for Ficsit Planner.
 * Contains all the factory-generation algorithm, and visualization tools.
 */
UCLASS()
class FICSITPLANNER_API UFicsitPlannerFactory : public UObject
{
	GENERATED_BODY()

public:
	UFicsitPlannerFactory();
	void Init(FItemAmount TargetProduct, UFGRecipe* Recipe, const TMap<UFGItemDescriptor*, TArray<UFGRecipe*>>& RecipeData);

	UPROPERTY()
	FItemAmount TargetProduct;

	UPROPERTY()
	TArray<FItemAmount> Byproducts;

	UPROPERTY()
	UFGRecipe* Recipe;

	UPROPERTY()
	float Multiplier;

	UPROPERTY()
	TArray<UFicsitPlannerFactoryGroup*> SubFactories;

private:
};

/**
 * Allows factories to split depending on the recipe chosen (e.g. alternate recipes)
 */
UCLASS()
class FICSITPLANNER_API UFicsitPlannerFactoryGroup : public UObject
{
	GENERATED_BODY()

public:
	UFicsitPlannerFactoryGroup();
	void Init(FItemAmount TargetProduct, const TMap<UFGItemDescriptor*, TArray<UFGRecipe*>>& RecipeData);

	UPROPERTY()
	TArray<UFicsitPlannerFactory*> Factories;
};
