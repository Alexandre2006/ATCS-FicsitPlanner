

#pragma once

#include "CoreMinimal.h"
#include "FicsitPlannerFactory.h"
#include "FicsitPlannerFactoryGroup.generated.h"

/**
 * 
 */
UCLASS()
class FICSITPLANNER_API UFicsitPlannerFactoryGroup : public UFicsitPlannerFactory
{
	GENERATED_BODY()

private:
	// Factories
	UPROPERTY()
	TArray<UFicsitPlannerFactory*> Factories;

	UPROPERTY()
	int SelectedFactoryIndex;

public:
	// Constructor / Destructor
	UFicsitPlannerFactoryGroup();
	virtual ~UFicsitPlannerFactoryGroup() override;

	// Init
	void Init(FItemAmount Target, TArray<TSubclassOf<UFGRecipe>> Recipes, const TArray<TSubclassOf<UFGRecipe>>& VisitedRecipes, bool AllowLockedRecipes, EFactoryOptimizationType Optimization, AFicsitPlannerEngine* Engine);

	// Getters (Factory Metrics - Selected Factory - Individual)
	virtual float GetMultiplier() const override;
	virtual float GetPowerConsumption() const override;
	virtual int GetComplexity() const override;

	// Getters (Factory Metrics - Selected Factory - Sum)
	virtual float GetTotalPowerConsumption() override;
	virtual int GetTotalComplexity() override;

	// Getters (Factory Metrics - All Factories - Individual)
	TArray<float> GetAllMultipliers() const;
	TArray<float> GetAllPowerConsumptions() const;
	TArray<int> GetAllComplexities() const;

	// Getters (Factory Metrics - All Factories - Sum)
	TArray<float> GetAllTotalPowerConsumption();
	TArray<int> GetAllTotalComplexity();

	// Getters (Factory Data - Selected Factory)
	virtual TSubclassOf<UFGRecipe> GetRecipe() const override;
	virtual FItemAmount GetPrimaryProduct() const override;
	virtual TArray<FItemAmount> GetByproducts() const override;

	// Getters (Factory Data - All Factories)
	TArray<TSubclassOf<UFGRecipe>> GetAllRecipes() const;
	TArray<FItemAmount> GetAllPrimaryProducts() const;
	TArray<TArray<FItemAmount>> GetAllByproducts() const;

	// Getters (Subfactories - Selected Factory)
	virtual TArray<UFicsitPlannerFactory*>* GetSubfactories() override;

	// Getters (Subfactories - All Factories)
	TArray<TArray<UFicsitPlannerFactory*>> GetAllSubfactories() const;

	// Getter / Setters (Selected Factory)
	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner")
	void SetSelectedFactoryIndex(int Index) { SelectedFactoryIndex = Index; }
	
	UFUNCTION(BlueprintCallable, Category = "Ficsit Planner")
	int GetSelectedFactoryIndex() const { return SelectedFactoryIndex; }
};
