#include "FicsitPlannerEngine.h"

#include "FGBuildableManufacturer.h"
#include "FGPlayerController.h"
#include "FGRecipeManager.h"
#include "FicsitPlannerFactory.h"
#include "FicsitPlannerFactoryGroup.h"

AFicsitPlannerEngine::AFicsitPlannerEngine()
{
 UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Creating FicsitPlannerEngine..."));

 // Change replication policy to client-only
 ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnClient;

 UE_LOG(LogFicsitPlannerEngine, Display, TEXT("FicsitPlannerEngine created!"));
}

AFicsitPlannerEngine::~AFicsitPlannerEngine()
{
 UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Destroying FicsitPlannerEngine..."));

 // Clear all items and recipes
 AllItems.Empty();
 UnlockedItems.Empty();
 AllRecipesMap.Empty();
 UnlockedRecipesMap.Empty();

 UE_LOG(LogFicsitPlannerEngine, Display, TEXT("FicsitPlannerEngine destroyed!"));
}

void AFicsitPlannerEngine::Initialize()
{
 UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Initializing FicsitPlannerEngine..."));
 ReloadGameData();

 UE_LOG(LogFicsitPlannerEngine, Display, TEXT("FicsitPlannerEngine initialized!"));
}

void AFicsitPlannerEngine::ReloadGameData()
{
 UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Loading RecipeData..."));

 // Get Recipe Manager
 AFGRecipeManager * RecipeManager = AFGRecipeManager::Get(this);

 // Check if RecipeManager is valid
 if (!RecipeManager)
 {
  UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Failed to get RecipeManager!"));
  return;
 }

 // Load recipes
 TArray<TSubclassOf<UFGRecipe>> UnlockedRecipes;
 RecipeManager->GetAllAvailableRecipes(UnlockedRecipes);

 TArray<TSubclassOf<UFGRecipe>> AllRecipes = RecipeManager->GetAllRecipes();

 // Register recipes
 for (const TSubclassOf<UFGRecipe>& Recipe : UnlockedRecipes) {
 	// Get recipe
 	const UFGRecipe* RecipeObj = Recipe->GetDefaultObject<UFGRecipe>();

 	// Get ingredients
 	TArray<FItemAmount> Ingredients = RecipeObj->GetIngredients();

 	// Check if the recipe contains SAM as an ingredient
 	bool bContainsSAM = false;
 	for (const FItemAmount& Ingredient : Ingredients)
 	{
 		if (Ingredient.ItemClass->GetName() == "Desc_SAM_C" || Ingredient.ItemClass->GetName() == "Desc_SAMIngot_C")
 		{
 			bContainsSAM = true;
 			break;
 		}
 	}

 	// If the recipe contains SAM, skip it
 	if (bContainsSAM)
 	{
 		UE_LOG(LogFicsitPlannerEngine, Warning, TEXT("Skipping recipe %s because it contains SAM!"), *Recipe->GetName());
 		continue;
 	}

 	RegisterUnlockedRecipe(Recipe);
 }
	
 	// RegisterUnlockedRecipe(Recipe);
 for (const TSubclassOf<UFGRecipe>& Recipe : AllRecipes)
 {
 	// Get recipe
 	const UFGRecipe* RecipeObj = Recipe->GetDefaultObject<UFGRecipe>();

 	// Get ingredients
 	TArray<FItemAmount> Ingredients = RecipeObj->GetIngredients();

 	// Check if the recipe contains SAM as an ingredient
 	bool bContainsSAM = false;
 	for (const FItemAmount& Ingredient : Ingredients)
 	{
 		if (Ingredient.ItemClass->GetName() == "Desc_SAM_C" || Ingredient.ItemClass->GetName() == "Desc_SAMIngot_C")
 		{
 			bContainsSAM = true;
 			break;
 		}
 	}

 	// If the recipe contains SAM, skip it
 	if (bContainsSAM)
 	{
 		UE_LOG(LogFicsitPlannerEngine, Warning, TEXT("Skipping recipe %s because it contains SAM!"), *Recipe->GetName());
 		continue;
 	}

 	RegisterRecipe(Recipe);
 }

 // Log the number of items and recipes loaded
 UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Loaded %d items and %d recipes (%d items unlocked, %d recipes unlocked)."), AllItems.Num(), AllRecipes.Num(), UnlockedItems.Num(), UnlockedRecipes.Num());
}

float AFicsitPlannerEngine::GetFactoryMultiplier(const TSubclassOf<UFGRecipe> Recipe, const FItemAmount Target)
{
 // Get the default values of the recipe
 const UFGRecipe* RecipeObj = Recipe->GetDefaultObject<UFGRecipe>();

 // Get craft duration from recipe
 float CraftDuration = RecipeObj->GetManufacturingDuration();

 // Find output rate of target product
 for (const FItemAmount& Product : RecipeObj->GetProducts())
 {
  if (Product.ItemClass == Target.ItemClass)
  {
   // Calculate default rate per minute
   const float DefaultRate = (60.0f / CraftDuration) * Product.Amount;

   // Calculate multiplier
   const float Multiplier = Target.Amount / DefaultRate;
   return Multiplier;
  }
 }

 // Return default multiplier if no product found
 return 1.0f;
}

float AFicsitPlannerEngine::GetFactoryPowerConsumption(const TSubclassOf<UFGRecipe> Recipe, const float Multiplier)
{
	// Get the default values of the recipe
	const UFGRecipe* RecipeObj = Recipe->GetDefaultObject<UFGRecipe>();

	// Get producers
	TArray<TSubclassOf<UObject>> ProducedIn;
	RecipeObj->GetProducedIn(ProducedIn);

	// Get the first producer that is a building
	TSubclassOf<UObject> Producer;
	for (const TSubclassOf<UObject>& Building : ProducedIn)
	{
		if (Building->GetName().StartsWith("Build_"))
		{
			Producer = Building;
			break;
		}
	}

	// Get the power consumption of the producer, or return 0 if not selected
	if (!Producer)
	{
		UE_LOG(LogFicsitPlannerEngine, Warning, TEXT("No producer found for recipe %s!"), *Recipe->GetName());
		return 0.0f;
	}

	AFGBuildableManufacturer* ProducerBuild = Cast<AFGBuildableManufacturer>(Producer->GetDefaultObject());
	if (!ProducerBuild)
	{
		UE_LOG(LogFicsitPlannerEngine, Warning, TEXT("Producer %s is not a valid manufacturer!"), *Producer->GetName());
		return 0.0f;
	}
	
	return ProducerBuild->GetDefaultProducingPowerConsumption() * Multiplier;
}

UFicsitPlannerFactory* AFicsitPlannerEngine::CreateFactoryPlan(FItemAmount TargetProduct, bool bAllowLockedRecipes, EFactoryOptimizationType Optimization)
{
	// Check if the target product is valid
	if (!TargetProduct.ItemClass)
	{
		UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Invalid target product!"));
		return nullptr;
	}

	// Check if there are recipes for the target product
	TArray<TSubclassOf<UFGRecipe>> Recipes = bAllowLockedRecipes ?
		AllRecipesMap[TargetProduct.ItemClass] :
		UnlockedRecipesMap[TargetProduct.ItemClass];

	if (Recipes.Num() == 0)
	{
		UE_LOG(LogFicsitPlannerEngine, Error, TEXT("No recipes found for target product %s!"), *TargetProduct.ItemClass->GetName());
		return nullptr;
	}

	// OPTION 1: Single recipe case
	if (Recipes.Num() == 1)
	{
		// Create a new factory
		UFicsitPlannerFactory* Factory = NewObject<UFicsitPlannerFactory>();
		Factory->Init(TargetProduct, Recipes[0], TArray<TSubclassOf<UFGRecipe>>(), bAllowLockedRecipes, Optimization, this);
		return Factory;
	}

	// OPTION 2: Multiple recipes case
	// Create a new factory group
	UFicsitPlannerFactoryGroup* FactoryGroup = NewObject<UFicsitPlannerFactoryGroup>();
	FactoryGroup->Init(TargetProduct, Recipes, TArray<TSubclassOf<UFGRecipe>>(), bAllowLockedRecipes, Optimization, this);
	return FactoryGroup;
}

void AFicsitPlannerEngine::LogFactoryPlan(UFicsitPlannerFactory* Factory)
{
	// Log the factory plan
	if (Factory)
	{
		FString FactoryPlanString = FactoryPlanToString(Factory);
		UE_LOG(LogFicsitPlannerEngine, Display, TEXT("%s"), *FactoryPlanString);
	}
	else
	{
		UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Invalid factory!"));
	}
}

FString AFicsitPlannerEngine::FactoryPlanToString(UFicsitPlannerFactory* Factory)
{
	// Create a string to store the factory plan
	FString FactoryPlanString;

	// Add basic factory info
	FactoryPlanString += FString::Printf(TEXT("Producing: %dx %s / min\n"), Factory->GetPrimaryProduct().Amount, *Factory->GetPrimaryProduct().ItemClass->GetDefaultObject<UFGItemDescriptor>()->mDisplayName.ToString());
	FactoryPlanString += FString::Printf(TEXT("Power Consumption: %.2f MW\n"), Factory->GetTotalPowerConsumption());
	FactoryPlanString += FString::Printf(TEXT("Complexity: %d\n"), Factory->GetTotalComplexity());

	// Log factory plan iteratively (w/ deque)
	TArray<UFicsitPlannerFactory*> FactoryDeque;
	TArray<int> IndentationLevels;
	FactoryDeque.Add(Factory);
	IndentationLevels.Add(0);

	int CurrentNodeIndex = 1;
	while (FactoryDeque.Num() > 0)
	{
		// Get the next factory in the deque
		int LastElement = FactoryDeque.Num() - 1;

		UFicsitPlannerFactory* CurrentFactory = FactoryDeque[LastElement];
		FactoryDeque.RemoveAt(LastElement);

		int IndentationLevel = IndentationLevels[LastElement];
		IndentationLevels.RemoveAt(LastElement);

		// Add indentation
		FString IndentationString;
		for (int i = 0; i < IndentationLevel; i++)
		{
			IndentationString += TEXT("  ");
		}
		IndentationString += TEXT("- ");

		// Add factory info to the string (recipe, building, multiplier)
		FactoryPlanString += IndentationString + FString::Printf(TEXT("%d: Product: %s, Recipe: %s, Multiplier: %.2f\n"),
			CurrentNodeIndex,
			*CurrentFactory->GetPrimaryProduct().ItemClass->GetDefaultObject<UFGItemDescriptor>()->mDisplayName.ToString(),
			*CurrentFactory->GetRecipe()->GetDefaultObject<UFGRecipe>()->GetDisplayName().ToString(),
			CurrentFactory->GetMultiplier());

		// Check if the factory is a factory group
		if (UFicsitPlannerFactoryGroup* FactoryGroup = Cast<UFicsitPlannerFactoryGroup>(CurrentFactory))
		{
			FactoryPlanString += FString::Printf(TEXT("(Option %d of %d)\n"), FactoryGroup->GetSelectedFactoryIndex() + 1, FactoryGroup->GetAllMultipliers().Num());
		}

		// Add subfactories to the deque
		for (UFicsitPlannerFactory* Subfactory : *CurrentFactory->GetSubfactories())
		{
			if (Subfactory)
			{
				FactoryDeque.Add(Subfactory);
				IndentationLevels.Add(IndentationLevel + 1);
			}
		}
		CurrentNodeIndex++;
	}

	return FactoryPlanString;
}

FString AFicsitPlannerEngine::FactoryPlanToHeaderString(UFicsitPlannerFactory* Factory)
{
	// Create a string to store the factory plan
	FString FactoryPlanString;

	// Add basic factory info
	FactoryPlanString += FString::Printf(TEXT("Producing: %dx %s / min (Power Consumption: %.2f MW | Complexity: %d)\n"), Factory->GetPrimaryProduct().Amount, *Factory->GetPrimaryProduct().ItemClass->GetDefaultObject<UFGItemDescriptor>()->mDisplayName.ToString(), Factory->GetTotalPowerConsumption(), Factory->GetTotalComplexity());

	// Return the factory plan string
	return FactoryPlanString;
}

int AFicsitPlannerEngine::SaveFactory(UFicsitPlannerFactory* Factory)
{
	// Add the factory to the list of factories, and return its ID
	int FactoryID = Factories.Num();
	Factories.Add(Factory);
	return FactoryID;
}

UFicsitPlannerFactory* AFicsitPlannerEngine::GetFactory(int FactoryID)
{
	// Check if the factory ID is valid
	if (Factories.Num() > FactoryID)
	{
		return Factories[FactoryID];
	}
	else
	{
		UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Invalid factory ID: %d"), FactoryID);
		return nullptr;
	}
}

bool AFicsitPlannerEngine::DeleteFactory(int FactoryID)
{
	// Check if the factory ID is valid
	if (Factories.Num() > FactoryID)
	{
		// Remove the factory from the list
		Factories.RemoveAt(FactoryID);
		return true;
	}
	else
	{
		UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Invalid factory ID: %d"), FactoryID);
		return false;
	}
}

TArray<UFicsitPlannerFactory*> AFicsitPlannerEngine::GetAllFactories()
{
	// Return all factories
	TArray<UFicsitPlannerFactory*> AllFactories;
	for (const auto& Factory : Factories)
	{
		if (Factory)
		{
			AllFactories.Add(Factory);
		}
	}
	return AllFactories;
}

bool AFicsitPlannerEngine::ReplaceFactory(int FactoryID, UFicsitPlannerFactory* Factory)
{
	// Check if index is valid
	if (Factories.Num() > FactoryID)
	{
		// Replace the factory at the given index
		Factories[FactoryID] = Factory;
		return true;
	}
	else
	{
		UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Invalid factory ID: %d"), FactoryID);
		return false;
	}
}

bool AFicsitPlannerEngine::UpdateFactoryGroup(int FactoryID, int nodeID, int newIndex)
{
	// Check if the factory ID is valid
	if (Factories.Num() <= FactoryID)
	{
		UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Invalid factory ID: %d"), FactoryID);
		return false;
	}

	// Get the factory
	UFicsitPlannerFactory* Factory = Factories[FactoryID];

	// Get all nodes, as deque, until index is reached
	TArray<UFicsitPlannerFactory*> FactoryDeque;
	FactoryDeque.Add(Factory);

	int CurrentNodeIndex = 1;

	while (FactoryDeque.Num() > 0)
	{
		// Get the next factory in the deque
		int LastElement = FactoryDeque.Num() - 1;

		UFicsitPlannerFactory* CurrentFactory = FactoryDeque[LastElement];
		FactoryDeque.RemoveAt(LastElement);

		// Check if the current node is the one we want to update
		if (CurrentNodeIndex == nodeID)
		{
			// Update the selected factory index
			if (UFicsitPlannerFactoryGroup* FactoryGroup = Cast<UFicsitPlannerFactoryGroup>(CurrentFactory))
			{
				FactoryGroup->SetSelectedFactoryIndex(newIndex - 1);
				return true;
			}
			else
			{
				UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Current node is not a factory group!"));
				return false;
			}
		}

		// Add subfactories to the deque
		for (UFicsitPlannerFactory* Subfactory : *CurrentFactory->GetSubfactories())
		{
			if (Subfactory)
			{
				FactoryDeque.Add(Subfactory);
			}
		}
		
		CurrentNodeIndex++;
	}

	UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Node ID %d not found in factory group!"), nodeID);
	return false;
}

bool AFicsitPlannerEngine::OptimizeFactory(int Factory, EFactoryOptimizationType Optimization)
{
	// Check if the factory ID is valid
	if (Factories.Num() <= Factory)
	{
		UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Invalid factory ID: %d"), Factory);
		return false;
	}

	// Get the factory
	UFicsitPlannerFactory* FactoryObj = Factories[Factory];

	// Check if optimization is valid
	if (Optimization == EFactoryOptimizationType::EFOT_None)
	{
		UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Invalid optimization type!"));
		return false;
	}

	// Optimize the factory
	UFicsitPlannerFactory* NewFactory = AFicsitPlannerEngine::CreateFactoryPlan(FactoryObj->GetPrimaryProduct(), FactoryObj->UsesLockedRecipes, Optimization);

	// Replace the factory
	if (ReplaceFactory(Factory, NewFactory))
	{
		UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Factory %d optimized successfully!"), Factory);
		return true;
	}
	else
	{
		UE_LOG(LogFicsitPlannerEngine, Error, TEXT("Failed to optimize factory %d!"), Factory);
		return false;
	}
}

TSubclassOf<UFGItemDescriptor> AFicsitPlannerEngine::FindItem(const FString& ItemName)
{
	// Search for the item in the unlocked items
	for (const TSubclassOf<UFGItemDescriptor>& Item : UnlockedItems)
	{
		if (Item->GetDefaultObject<UFGItemDescriptor>()->mDisplayName.ToString().Equals(ItemName, ESearchCase::IgnoreCase))
		{
			return Item;
		}
	}
	return nullptr;
}

bool AFicsitPlannerEngine::RecipeProducedInBuilding(const TSubclassOf<UFGRecipe> Recipe)
{
	// Get the default values of the recipe
	const UFGRecipe* RecipeObj = Recipe->GetDefaultObject<UFGRecipe>();

	// Get "Produced In" property
	TArray<TSubclassOf<UObject>> ProducedIn;
	RecipeObj->GetProducedIn(ProducedIn);

	// Check if any of the producers is a building
	for (const TSubclassOf<UObject>& Producer : ProducedIn)
	{
		if (Producer->GetName().StartsWith("Build_"))
		{
			// If produced in a building, return true
			return true;
		}
	}

	// If not produced in a building, return false
	return false;
}

void AFicsitPlannerEngine::RegisterUnlockedRecipe(const TSubclassOf<UFGRecipe> Recipe)
{
	// Ensure the recipe is produced in a building
	if (!RecipeProducedInBuilding(Recipe)) return;

	// Get the recipe's products
	const UFGRecipe* RecipeObj = Recipe->GetDefaultObject<UFGRecipe>();

	// Add products to unlocked items list
	for (TArray<FItemAmount> Products = RecipeObj->GetProducts(); const FItemAmount& Product : Products)
	{
		TSubclassOf<UFGItemDescriptor> Item = Product.ItemClass;

		// Add item to all items list
		if (!UnlockedItems.Contains(Item)) UnlockedItems.Add(Item);

		// If the item doesn't exist in the map, create a new entry
		if (!UnlockedRecipesMap.Contains(Item)) UnlockedRecipesMap.Add(Item, TArray<TSubclassOf<UFGRecipe>>());

		// Add the recipe to the item's list of recipes
		if (TArray<TSubclassOf<UFGRecipe>>& Recipes = UnlockedRecipesMap[Item]; !Recipes.Contains(Recipe))
		{
			Recipes.Add(Recipe);
		}
	}
}

void AFicsitPlannerEngine::RegisterRecipe(const TSubclassOf<UFGRecipe> Recipe)
{
	// Ensure the recipe is produced in a building
	if (!RecipeProducedInBuilding(Recipe)) return;

	// Get the recipe's products
	const UFGRecipe* RecipeObj = Recipe->GetDefaultObject<UFGRecipe>();

	// Add products to recipe map
	for (TArray<FItemAmount> Products = RecipeObj->GetProducts(); const FItemAmount& Product : Products)
	{
		TSubclassOf<UFGItemDescriptor> Item = Product.ItemClass;

		// Add item to all items list
		if (!AllItems.Contains(Item)) AllItems.Add(Item);

		// If the item doesn't exist in the map, create a new entry
		if (!AllRecipesMap.Contains(Item)) AllRecipesMap.Add(Item, TArray<TSubclassOf<UFGRecipe>>());

		// Add the recipe to the item's list of recipes

		if (TArray<TSubclassOf<UFGRecipe>>& Recipes = AllRecipesMap[Item]; !Recipes.Contains(Recipe))
		{
			Recipes.Add(Recipe);
		}
	}
}