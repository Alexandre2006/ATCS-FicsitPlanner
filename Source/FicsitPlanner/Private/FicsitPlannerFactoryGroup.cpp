#include "FicsitPlannerFactoryGroup.h"

UFicsitPlannerFactoryGroup::UFicsitPlannerFactoryGroup()
{
	// Initialize default values
	Factories.Empty();
	SelectedFactoryIndex = 0;
}

UFicsitPlannerFactoryGroup::~UFicsitPlannerFactoryGroup()
{
	// No cleanup needed, should be handled fine by the GC
}

void UFicsitPlannerFactoryGroup::Init(const FItemAmount Target, TArray<TSubclassOf<UFGRecipe>> InRecipes,
	const TArray<TSubclassOf<UFGRecipe>>& VisitedRecipes, const bool AllowLockedRecipes, EFactoryOptimizationType Optimization, AFicsitPlannerEngine* Engine)
{
	UsesLockedRecipes = AllowLockedRecipes;
	// Cycle through all recipes
	for (const TSubclassOf<UFGRecipe>& CurrentRecipe : InRecipes)
	{
		// Create a new subfactory
		UFicsitPlannerFactory* Subfactory = NewObject<UFicsitPlannerFactory>();
		Subfactory->Init(Target, CurrentRecipe, VisitedRecipes, AllowLockedRecipes, Optimization, Engine);
		Factories.Add(Subfactory);
	}
}

float UFicsitPlannerFactoryGroup::GetMultiplier() const
{
	// Check if selected factory exists
	if (SelectedFactoryIndex < 0 || SelectedFactoryIndex >= Factories.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected factory index out of bounds!"));
		return 0.0f;
	}

	// Get multiplier of selected factory
	return Factories[SelectedFactoryIndex]->GetMultiplier();
}

float UFicsitPlannerFactoryGroup::GetPowerConsumption() const
{
	// Check if selected factory exists
	if (SelectedFactoryIndex < 0 || SelectedFactoryIndex >= Factories.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected factory index out of bounds!"));
		return 0.0f;
	}

	// Get power consumption of selected factory
	return Factories[SelectedFactoryIndex]->GetPowerConsumption();
}

int UFicsitPlannerFactoryGroup::GetComplexity() const
{
	// Check if selected factory exists
	if (SelectedFactoryIndex < 0 || SelectedFactoryIndex >= Factories.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected factory index out of bounds!"));
		return 0;
	}

	// Get complexity of selected factory
	return Factories[SelectedFactoryIndex]->GetComplexity();
}

float UFicsitPlannerFactoryGroup::GetTotalPowerConsumption()
{
	// Check if selected factory exists
	if (SelectedFactoryIndex < 0 || SelectedFactoryIndex >= Factories.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected factory index out of bounds!"));
		return 0.0f;
	}

	// Get power consumption of selected factory
	return Factories[SelectedFactoryIndex]->GetTotalPowerConsumption();
}

int UFicsitPlannerFactoryGroup::GetTotalComplexity()
{
	// Check if selected factory exists
	if (SelectedFactoryIndex < 0 || SelectedFactoryIndex >= Factories.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected factory index out of bounds!"));
		return 0;
	}

	// Get complexity of selected factory
	return Factories[SelectedFactoryIndex]->GetTotalComplexity();
}

TArray<float> UFicsitPlannerFactoryGroup::GetAllMultipliers() const
{
	// Create an array to store the multipliers
	TArray<float> Multipliers;

	// Iterate through all factories and add their multipliers to the array
	for (const UFicsitPlannerFactory* Factory : Factories)
	{
		if (Factory)
		{
			Multipliers.Add(Factory->GetMultiplier());
		}
	}

	// Return the array of multipliers
	return Multipliers;
}

TArray<float> UFicsitPlannerFactoryGroup::GetAllPowerConsumptions() const
{
	// Create an array to store the power consumption values
	TArray<float> PowerConsumptions;

	// Iterate through all factories and add their power consumption to the array
	for (const UFicsitPlannerFactory* Factory : Factories)
	{
		if (Factory)
		{
			PowerConsumptions.Add(Factory->GetPowerConsumption());
		}
	}

	// Return the array of power consumption values
	return PowerConsumptions;
}

TArray<int> UFicsitPlannerFactoryGroup::GetAllComplexities() const
{
	// Create an array to store the complexity values
	TArray<int> Complexities;

	// Iterate through all factories and add their complexity to the array
	for (const UFicsitPlannerFactory* Factory : Factories)
	{
		if (Factory)
		{
			Complexities.Add(Factory->GetComplexity());
		}
	}

	// Return the array of complexity values
	return Complexities;
}

TArray<float> UFicsitPlannerFactoryGroup::GetAllTotalPowerConsumption()
{
	// Create an array to store the power consumption values
	TArray<float> PowerConsumptions;

	// Iterate through all factories and add their power consumption to the array
	for (UFicsitPlannerFactory* Factory : Factories)
	{
		if (Factory)
		{
			PowerConsumptions.Add(Factory->GetTotalPowerConsumption());
		}
	}

	// Return the array of power consumption values
	return PowerConsumptions;
}

TArray<int> UFicsitPlannerFactoryGroup::GetAllTotalComplexity()
{
	// Create an array to store the complexity values
	TArray<int> Complexities;

	// Iterate through all factories and add their complexity to the array
	for (UFicsitPlannerFactory* Factory : Factories)
	{
		if (Factory)
		{
			Complexities.Add(Factory->GetTotalComplexity());
		}
	}

	// Return the array of complexity values
	return Complexities;
}

TSubclassOf<UFGRecipe> UFicsitPlannerFactoryGroup::GetRecipe() const
{
	// Check if selected factory exists
	if (SelectedFactoryIndex < 0 || SelectedFactoryIndex >= Factories.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected factory index out of bounds!"));
		return nullptr;
	}

	// Get recipe of selected factory
	return Factories[SelectedFactoryIndex]->GetRecipe();
}

FItemAmount UFicsitPlannerFactoryGroup::GetPrimaryProduct() const
{
	// Check if selected factory exists
	if (SelectedFactoryIndex < 0 || SelectedFactoryIndex >= Factories.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected factory index out of bounds!"));
		return FItemAmount();
	}

	// Get primary product of selected factory
	return Factories[SelectedFactoryIndex]->GetPrimaryProduct();
}

TArray<FItemAmount> UFicsitPlannerFactoryGroup::GetByproducts() const
{
	// Check if selected factory exists
	if (SelectedFactoryIndex < 0 || SelectedFactoryIndex >= Factories.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected factory index out of bounds!"));
		return TArray<FItemAmount>();
	}

	// Get byproducts of selected factory
	return Factories[SelectedFactoryIndex]->GetByproducts();
}

TArray<TSubclassOf<UFGRecipe>> UFicsitPlannerFactoryGroup::GetAllRecipes() const
{
	// Create an array to store the recipes
	TArray<TSubclassOf<UFGRecipe>> AllRecipesResult;

	// Iterate through all factories and add their recipes to the array
	for (const UFicsitPlannerFactory* Factory : Factories)
	{
		if (Factory)
		{
			TSubclassOf<UFGRecipe> FactoryRecipe = Factory->GetRecipe();
			if (FactoryRecipe)
			{
				AllRecipesResult.Add(FactoryRecipe);
			}
		}
	}

	// Return the array of recipes
	return AllRecipesResult;
}

TArray<FItemAmount> UFicsitPlannerFactoryGroup::GetAllPrimaryProducts() const
{
	// Create an array to store the primary products
	TArray<FItemAmount> AllPrimaryProductsResult;

	// Iterate through all factories and add their primary products to the array
	for (const UFicsitPlannerFactory* Factory : Factories)
	{
		if (Factory)
		{
			FItemAmount FactoryPrimaryProduct = Factory->GetPrimaryProduct();
			if (FactoryPrimaryProduct.ItemClass)
			{
				AllPrimaryProductsResult.Add(FactoryPrimaryProduct);
			}
		}
	}

	// Return the array of primary products
	return AllPrimaryProductsResult;
}

TArray<TArray<FItemAmount>> UFicsitPlannerFactoryGroup::GetAllByproducts() const
{
	// Create an array to store the byproducts
	TArray<TArray<FItemAmount>> AllByproductsResult;

	// Iterate through all factories and add their byproducts to the array
	for (const UFicsitPlannerFactory* Factory : Factories)
	{
		if (Factory)
		{
			TArray<FItemAmount> FactoryByproducts = Factory->GetByproducts();
			if (FactoryByproducts.Num() > 0)
			{
				AllByproductsResult.Add(FactoryByproducts);
			}
		}
	}

	// Return the array of byproducts
	return AllByproductsResult;
}

TArray<UFicsitPlannerFactory*>* UFicsitPlannerFactoryGroup::GetSubfactories()
{
	// Check if selected factory exists
	if (SelectedFactoryIndex < 0 || SelectedFactoryIndex >= Factories.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected factory index out of bounds!"));
		return nullptr;
	}

	// Get subfactories of selected factory
	return Factories[SelectedFactoryIndex]->GetSubfactories();
}

TArray<TArray<UFicsitPlannerFactory*>> UFicsitPlannerFactoryGroup::GetAllSubfactories() const
{
	// Create an array to store the subfactories
	TArray<TArray<UFicsitPlannerFactory*>> AllSubfactoriesResult;

	// Iterate through all factories and add their subfactories to the array
	for (UFicsitPlannerFactory* Factory : Factories)
	{
		if (Factory)
		{
			TArray<UFicsitPlannerFactory*>* FactorySubfactories = Factory->GetSubfactories();
			if (FactorySubfactories && FactorySubfactories->Num() > 0)
			{
				AllSubfactoriesResult.Add(*FactorySubfactories);
			}
		}
	}

	// Return the array of subfactories
	return AllSubfactoriesResult;
}