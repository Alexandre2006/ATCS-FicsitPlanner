


#include "FicsitPlannerEngine.h"

#include "FGRecipe.h"
#include "FGRecipeManager.h"

DECLARE_LOG_CATEGORY_CLASS(LogFicsitPlannerEngine, Log, All);

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
	LoadRecipeData();
	
	UE_LOG(LogFicsitPlannerEngine, Display, TEXT("FicsitPlannerEngine initialized!"));
}

void AFicsitPlannerEngine::LoadRecipeData()
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
	// To-Do: Adapt mod so it is compatible with both locked/unlocked recipes
	TArray<TSubclassOf<UFGRecipe>> Recipes = RecipeManager->GetAllRecipes();

	// DEBUG: Log # of recipes
	UE_LOG(LogFicsitPlannerEngine, Display, TEXT("Number of recipes: %d"), Recipes.Num());

	// Filter recipes by type
	for (TSubclassOf<UFGRecipe> Recipe : Recipes)
	{
		// Get "Produced In" property
		TArray<TSubclassOf<UObject>> ProducedIn;
		Recipe->GetDefaultObject<UFGRecipe>()->GetProducedIn(ProducedIn);
		// Check if the recipe contains any building producers
		bool bContainsBuildingProducer = false;
		for (TSubclassOf<UObject> Producer : ProducedIn)
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
	
}
