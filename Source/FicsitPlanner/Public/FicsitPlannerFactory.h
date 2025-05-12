#pragma once

#include "CoreMinimal.h"
#include "FGRecipe.h"
#include "ItemAmount.h"
#include "FicsitPlannerFactory.generated.h"


UENUM()
enum EFactoryOptimizationType
{
	EFOT_None,
	EFOT_Complexity,
	EFOT_PowerConsumption,
};

class AFicsitPlannerEngine;

/**
 *
 */
UCLASS(BlueprintType)
class FICSITPLANNER_API UFicsitPlannerFactory : public UObject
{
	GENERATED_BODY()

	// Configuration
	const int RECIPE_COMPLEXITY = 10;
	const int BUILDING_COMPLEXITY = 1;

private:
	// Cached Factory Metrics
	UPROPERTY()
	float Multiplier;

	UPROPERTY()
	float PowerConsumption;

	UPROPERTY()
	int Complexity;

	// Cached Factory Data
	UPROPERTY()
	TSubclassOf<UFGRecipe> Recipe;

	UPROPERTY()
	FItemAmount PrimaryProduct;

	UPROPERTY()
	TArray<FItemAmount> Byproducts;

	// Subfactories
	UPROPERTY()
	TArray<UFicsitPlannerFactory*> Subfactories;


public:
	// Constructor / Destructor
	UFicsitPlannerFactory();
	virtual ~UFicsitPlannerFactory() override;

	// Init
	void Init(FItemAmount Target, TSubclassOf<UFGRecipe> InRecipe, const TArray<TSubclassOf<UFGRecipe>>& VisitedRecipes,
	          bool AllowLockedRecipes, EFactoryOptimizationType Optimization, AFicsitPlannerEngine* Engine);

	// Getters (Factory Metrics - Individual)
	virtual float GetMultiplier() const;
	virtual float GetPowerConsumption() const;
	virtual int GetComplexity() const;

	// Getters (Factory Metrics - Sum)
	virtual float GetTotalPowerConsumption();
	virtual int GetTotalComplexity();

	// Getters (Factory Data)
	virtual TSubclassOf<UFGRecipe> GetRecipe() const;
	virtual FItemAmount GetPrimaryProduct() const;
	virtual TArray<FItemAmount> GetByproducts() const;

	// Getters subfactories
	virtual TArray<UFicsitPlannerFactory*>* GetSubfactories();

	UPROPERTY()
	bool UsesLockedRecipes;
};
