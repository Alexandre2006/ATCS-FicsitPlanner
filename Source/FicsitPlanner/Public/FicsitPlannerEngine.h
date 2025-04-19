

#pragma once

#include "CoreMinimal.h"
#include "Subsystem/ModSubsystem.h"
#include "FicsitPlannerEngine.generated.h"

/**
 * Backend for Ficsit Planner!
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
	TArray<TSubclassOf<UFGRecipe>> RecipeData;
	
	void LoadRecipeData();
	
};
