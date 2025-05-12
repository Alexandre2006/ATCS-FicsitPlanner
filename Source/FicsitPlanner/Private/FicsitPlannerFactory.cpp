


#include "FicsitPlannerFactory.h"

#include "FicsitPlannerEngine.h"
#include "FicsitPlannerFactoryGroup.h"

UFicsitPlannerFactory::UFicsitPlannerFactory()
{
	// Initialize default values
	Multiplier = 1.0f;
	PowerConsumption = 0.0f;
	Complexity = 0;
	Recipe = nullptr;
	PrimaryProduct = FItemAmount();
	Byproducts.Empty();
	Subfactories.Empty();
}

UFicsitPlannerFactory::~UFicsitPlannerFactory()
{
	// No cleanup needed, should be handled fine by the GC
}

void UFicsitPlannerFactory::Init(const FItemAmount Target, const TSubclassOf<UFGRecipe> InRecipe, const TArray<TSubclassOf<UFGRecipe>>& VisitedRecipes, const bool AllowLockedRecipes, EFactoryOptimizationType Optimization, AFicsitPlannerEngine* Engine)
{
	// PART 0: Calculate basic recipe info
	Recipe = InRecipe;
	Multiplier = AFicsitPlannerEngine::GetFactoryMultiplier(Recipe, Target);
	PowerConsumption = AFicsitPlannerEngine::GetFactoryPowerConsumption(Recipe, Multiplier);
	PrimaryProduct = Target;
	UsesLockedRecipes = AllowLockedRecipes;
	
	// PART 1: Create a copy of visited recipes to avoid impacting other factories
	TArray<TSubclassOf<UFGRecipe>> NewVisitedRecipes = VisitedRecipes;
	NewVisitedRecipes.Add(Recipe);
	
	// PART 2: Calculate complexity
	int BuildingCount = ceil(Multiplier);
	Complexity = RECIPE_COMPLEXITY + (BUILDING_COMPLEXITY * BuildingCount);

	// PART 3: Save byproducts
	for (const FItemAmount& Product : Recipe->GetDefaultObject<UFGRecipe>()->GetProducts())
	{
		if (Product.ItemClass != Target.ItemClass)
		{
			Byproducts.Add(Product);
		}
	}

	// PART 4: Generate subfactories
	for (const FItemAmount& Input : Recipe->GetDefaultObject<UFGRecipe>()->GetIngredients())
	{
		// PART 4.0: Calculate target amount
		FItemAmount TargetInput = Input;
		float CraftDuration = Recipe->GetDefaultObject<UFGRecipe>()->GetManufacturingDuration();
		TargetInput.Amount *= Multiplier * (60.0f / CraftDuration);

		// PART 4.1: Check if there are recipes for the input item
		if (AllowLockedRecipes && Engine->GetAllRecipesMap().Contains(Input.ItemClass) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("No recipes found for input item %s!"), *Input.ItemClass->GetName());
			continue;
		}
		if (!AllowLockedRecipes && Engine->GetUnlockedRecipesMap().Contains(Input.ItemClass) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("No unlocked recipes found for input item %s!"), *Input.ItemClass->GetName());
			continue;
		}
		
		// PART 4.2: Get recipes for input item
		TArray<TSubclassOf<UFGRecipe>> Recipes = AllowLockedRecipes ?
			Engine->GetAllRecipesMap()[Input.ItemClass] :
			Engine->GetUnlockedRecipesMap()[Input.ItemClass];

		// PART 4.3: Remove any visited recipes from the list
		for (const TSubclassOf<UFGRecipe>& VisitedRecipe : NewVisitedRecipes)
		{
			Recipes.Remove(VisitedRecipe);
		}

		// DEBUG: LOG RECIPES
		UE_LOG(LogTemp, Warning, TEXT("Recipes for %s: "), *Input.ItemClass->GetName());
		for (const TSubclassOf<UFGRecipe>& Recipe : Recipes)
		{
			UE_LOG(LogTemp, Warning, TEXT(" - %s"), *Recipe->GetName());
		}
		

		// PART 4.4: Single recipe case
		if (Recipes.Num() == 1) {
		
			// Create a new subfactory
			UFicsitPlannerFactory* Subfactory = NewObject<UFicsitPlannerFactory>();
			Subfactory->Init(TargetInput, Recipes[0], NewVisitedRecipes, AllowLockedRecipes, Optimization, Engine);
			Subfactories.Add(Subfactory);
		}
		// PART 4.5: Multiple recipes case
		else if (Recipes.Num() > 1)
		{
			// Create a new subfactory group
			UFicsitPlannerFactoryGroup* SubfactoryGroup = NewObject<UFicsitPlannerFactoryGroup>();
			SubfactoryGroup->Init(TargetInput, Recipes, NewVisitedRecipes, AllowLockedRecipes, Optimization, Engine);
			Subfactories.Add(SubfactoryGroup);
			
			// Optimization: Select the best subfactory
			if (Optimization == EFactoryOptimizationType::EFOT_PowerConsumption)
			{
				// Get power consumption of all subfactories
				TArray<float> PowerConsumptions = SubfactoryGroup->GetAllPowerConsumptions();
				
				// Find the index of the subfactory with the lowest power consumption
				int MinIndex = 0;
				float MinPowerConsumption = PowerConsumptions[0];
				for (int i = 1; i < PowerConsumptions.Num(); ++i)
				{
					if (PowerConsumptions[i] < MinPowerConsumption)
					{
						MinPowerConsumption = PowerConsumptions[i];
						MinIndex = i;
					}
				}
				
				// Select the subfactory with the lowest power consumption
				SubfactoryGroup->SetSelectedFactoryIndex(MinIndex);
			} else if (Optimization == EFactoryOptimizationType::EFOT_Complexity)
			{
				// Get complexity of all subfactories
				TArray<int> Complexities = SubfactoryGroup->GetAllComplexities();
				
				// Find the index of the subfactory with the lowest complexity
				int MinIndex = 0;
				int MinComplexity = Complexities[0];
				for (int i = 1; i < Complexities.Num(); ++i)
				{
					if (Complexities[i] < MinComplexity)
					{
						MinComplexity = Complexities[i];
						MinIndex = i;
					}
				}
				
				// Select the subfactory with the lowest complexity
				SubfactoryGroup->SetSelectedFactoryIndex(MinIndex);
			}
		}
	}
}

float UFicsitPlannerFactory::GetMultiplier() const
{
	return Multiplier;
}

float UFicsitPlannerFactory::GetPowerConsumption() const
{
	return PowerConsumption;
}

int UFicsitPlannerFactory::GetComplexity() const
{
	return Complexity;
}

float UFicsitPlannerFactory::GetTotalPowerConsumption()
{
	int TotalPowerConsumption = PowerConsumption;

	// Add power consumption of subfactories
	for (UFicsitPlannerFactory* Subfactory : Subfactories)
	{
		if (Subfactory)
		{
			TotalPowerConsumption += Subfactory->GetTotalPowerConsumption();
		}
	}

	return TotalPowerConsumption;
}

int UFicsitPlannerFactory::GetTotalComplexity()
{
	int TotalComplexity = Complexity;

	// Add complexity of subfactories
	for (UFicsitPlannerFactory* Subfactory : Subfactories)
	{
		if (Subfactory)
		{
			TotalComplexity += Subfactory->GetTotalComplexity();
		}
	}

	return TotalComplexity;
}

TSubclassOf<UFGRecipe> UFicsitPlannerFactory::GetRecipe() const
{
	return Recipe;
}

FItemAmount UFicsitPlannerFactory::GetPrimaryProduct() const
{
	return PrimaryProduct;
}

TArray<FItemAmount> UFicsitPlannerFactory::GetByproducts() const
{
	// Return copy to avoid modifying the orginal array
	return Byproducts;
}

TArray<UFicsitPlannerFactory*>* UFicsitPlannerFactory::GetSubfactories()
{
	// Return a pointer to the subfactories array, that way we can modify it!
	// Will be helpful when updating the subfactories, and we want to recalculate everything from the top!
	return &Subfactories;
}
