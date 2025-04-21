

#pragma once

#include "CoreMinimal.h"
#include "Subsystem/ModSubsystem.h"

#include "FGRecipe.h"
#include "FGRecipeManager.h"

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
	AFicsitPlannerEngine();

	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner")
	void InitEngine();

private:
	TArray<UFGItemDescriptor*> AllItems;
	TMap<UFGItemDescriptor*, TArray<UFGRecipe*>> RecipeGraph;
	
	void LoadRecipeData(bool bLoadUnavailableRecipes);
	void GenerateFactory(TArray<FItemAmount> requestedProducts);
};
