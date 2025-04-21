


#include "FicsitPlannerEngine.h"

#include "FicsitPlannerFactory.h"

AFicsitPlannerEngine::AFicsitPlannerEngine()
	: Super()
{
	UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Creating FicsitPlannerEngine..."));
	
	// Change replication policy to client-only
	ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnClient;

	UE_LOG(LogFicsitPlannerEngine, Display, TEXT("FicsitPlannerEngine created!"));
}

void AFicsitPlannerEngine::InitEngine()
{
	UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Initializing FicsitPlannerEngine..."));
	
	// Load recipe data
	LoadRecipeData(false);
	
	UE_LOG(LogFicsitPlannerEngine, Display, TEXT("FicsitPlannerEngine initialized!"));
}

void AFicsitPlannerEngine::LoadRecipeData(const bool bLoadUnavailableRecipes)
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

	// Load all recipes
	TArray<TSubclassOf<UFGRecipe>> Recipes;
	if (bLoadUnavailableRecipes)
	{
		Recipes = RecipeManager->GetAllRecipes();
	} else
	{
		RecipeManager->GetAllAvailableRecipes(Recipes);
	}

	// Filter recipes (remove any recipes which aren't produced in a building)
	TArray<TSubclassOf<UFGRecipe>> RecipeData;
	for (TSubclassOf Recipe : Recipes)
	{
		// Get "Produced In" property
		TArray<TSubclassOf<UObject>> ProducedIn;
		Recipe->GetDefaultObject<UFGRecipe>()->GetProducedIn(ProducedIn);
		// Check if the recipe contains any building producers
		bool bContainsBuildingProducer = false;
		for (const TSubclassOf Producer : ProducedIn)
		{
			if (Producer->GetName().StartsWith("Build_"))
			{
				bContainsBuildingProducer = true;
				break;
			}
		}

		if (bContainsBuildingProducer)
		{
			// Add recipe to filtered list
			RecipeData.Add(Recipe);
		}
	}

	// Process all items (products) for the searchable item list and graph
	for (const TSubclassOf Recipe : RecipeData)
	{
		// Get products from recipe
		TArray<FItemAmount> Products = Recipe->GetDefaultObject<UFGRecipe>()->GetProducts();
		for (const FItemAmount Product : Products)
		{
			// Check if item already exists in graph
			if (RecipeGraph.Contains(Product.ItemClass->GetDefaultObject<UFGItemDescriptor>()))
			{
				// Add recipe to existing item
				RecipeGraph[Product.ItemClass->GetDefaultObject<UFGItemDescriptor>()].Add(Recipe->GetDefaultObject<UFGRecipe>());
			}
			else
			{
				// Create new entry for item
				TArray<UFGRecipe*> RecipesForItem;
				RecipesForItem.Add(Recipe->GetDefaultObject<UFGRecipe>());
				RecipeGraph.Add(Product.ItemClass->GetDefaultObject<UFGItemDescriptor>(), RecipesForItem);

				// Add item to all items list
				AllItems.Add(Product.ItemClass->GetDefaultObject<UFGItemDescriptor>());
			}
		}
	}

	// Log the number of items and recipes loaded
	UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Loaded %d items and %d recipes."), AllItems.Num(), RecipeGraph.Num());

	// DEBUG: Attempt to create a factory
	FItemAmount TestProduct = FItemAmount(AllItems[2]->GetClass(), 100);
	UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Attempting to create factory for %s"), *TestProduct.ItemClass->GetName());
	GenerateFactory({TestProduct});
}

void AFicsitPlannerEngine::GenerateFactory(TArray<FItemAmount> requestedProducts)
{
	// Create a factory group for each of the requested products
	TArray<UFicsitPlannerFactoryGroup*> FactoryGroups;

	for (FItemAmount& Product : requestedProducts)
	{
		// Check if product has a recipe
		if (RecipeGraph.Contains(Product.ItemClass->GetDefaultObject<UFGItemDescriptor>()))
		{
			// Create factory group
			UFicsitPlannerFactoryGroup* FactoryGroup = NewObject<UFicsitPlannerFactoryGroup>();
			FactoryGroup->Init(Product, RecipeGraph);

			// Add factory group to list
			FactoryGroups.Add(FactoryGroup);
		}
	}
}