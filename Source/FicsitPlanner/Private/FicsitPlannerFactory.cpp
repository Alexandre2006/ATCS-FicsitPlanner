// Mods/FicsitPlanner/Source/FicsitPlanner/Private/FicsitPlannerFactory.cpp
#include "FicsitPlannerFactory.h"

#include "FicsitPlannerEngine.h"

UFicsitPlannerFactory::UFicsitPlannerFactory(): Recipe(nullptr), Multiplier(0)
{
}

void UFicsitPlannerFactory::Init(FItemAmount TargetProduct, UFGRecipe* Recipe,
                                 const TMap<UFGItemDescriptor*, TArray<UFGRecipe*>>& RecipeData)
{
	
	// Save basic info
	this->TargetProduct = TargetProduct;
	this->Recipe = Recipe;

	// Calculate ingredients & products
	TArray<FItemAmount> Products = Recipe->GetProducts();
	TArray<FItemAmount> Ingredients = Recipe->GetIngredients();
	
	// Calculate multiplier
	Multiplier = 1.0f;
	for (FItemAmount Product : Products)
	{
		if (Product.ItemClass == TargetProduct.ItemClass)
		{
			Multiplier = TargetProduct.Amount / Product.Amount;
			break;
		}
	}

	// Modify products and ingredients by multiplier
	for (FItemAmount& Product : Products)
	{
		Product.Amount *= Multiplier;
	}
	for (FItemAmount& Ingredient : Ingredients)
	{
		Ingredient.Amount *= Multiplier;
	}

	// Remove target product and save byproducts
	Byproducts = Products;
	for (int i = Byproducts.Num() - 1; i >= 0; i--)
	{
		if (Byproducts[i].ItemClass == TargetProduct.ItemClass)
		{
			Byproducts.RemoveAt(i);
			break;
		}
	}

	// Find non-raw ingredients (e.g. ingredients that have a recipe)
	for (FItemAmount Ingredient : Ingredients)
	{
		// Check if ingredient has a recipe
		if (RecipeData.Contains(Ingredient.ItemClass->GetDefaultObject<UFGItemDescriptor>()))
		{
			// Create sub-factory
			UFicsitPlannerFactoryGroup* SubFactory = NewObject<UFicsitPlannerFactoryGroup>();
			SubFactory->Init(Ingredient, RecipeData);

			// Add sub-factory to list
			SubFactories.Add(SubFactory);
		}
	}
}

UFicsitPlannerFactoryGroup::UFicsitPlannerFactoryGroup()
{
}

void UFicsitPlannerFactoryGroup::Init(FItemAmount TargetProduct,
                                      const TMap<UFGItemDescriptor*, TArray<UFGRecipe*>>& RecipeData)
{
	// Find recipes which can be used to produce the target product
	TArray<UFGRecipe*> Recipes = RecipeData.FindRef(TargetProduct.ItemClass->GetDefaultObject<UFGItemDescriptor>());

	// Create a factory for each recipe
	for (UFGRecipe* Recipe : Recipes)
	{
		// Create factory
		UFicsitPlannerFactory* Factory = NewObject<UFicsitPlannerFactory>();
		Factory->Init(TargetProduct, Recipe, RecipeData);

		// Add the factory to the group
		Factories.Add(Factory);
	}
}
