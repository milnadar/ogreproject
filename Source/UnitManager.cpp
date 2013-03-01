#include "UnitManager.h"
#include "GameUnit.h"

UnitManager::UnitManager(Ogre::SceneManager *manager) : sceneManager(manager)
{
	unitCounter = 0;
}

UnitManager::~UnitManager()
{
	//if(!units.empty()) //delete units
}

GameUnit* UnitManager::createUnit()
{
	GameUnit *unit = NULL;
	if(sceneManager != NULL)
	{
		unit = new GameUnit(unitCounter++, sceneManager);
		units.push_back(unit);
	}
	return unit;
}