#include "UnitManager.h"
#include "GameUnit.h"

template<> UnitManager* Ogre::Singleton<UnitManager>::msSingleton = 0;

UnitManager::UnitManager(Ogre::SceneManager* manager)
{
	sceneManager = manager;
	unitCounter = 0;
}

UnitManager::~UnitManager()
{
	//if(!units.empty()) //delete units
}

GameUnit* UnitManager::createUnit(int player)
{
	GameUnit *unit = NULL;
	if(sceneManager != NULL)
	{
		TrooperStats stats(6, 3, 3, 3, 6, 4, 1);
		unit = new Trooper(unitCounter++, player, stats, sceneManager);
		units.push_back(unit);
	}
	return unit;
}

bool UnitManager::createSquad(int player)
{
	for(int i = 0; i < 5; i++)
		createUnit(player);
	return true;
}

GameUnit* UnitManager::getUnitByName(Ogre::String name)
{
	GameUnit *unit = NULL;
	for(iterator = units.begin(); iterator != units.end(); iterator++)
		if((*iterator)->getUnitName() == name)
		{
			unit = *iterator;
			break;
		}
	return unit;
}

void UnitManager::resetUnitsStats()
{
	for(iterator = units.begin(); iterator != units.end(); iterator++)
	{
		if(*iterator != NULL)
			(*iterator)->resetTurnStats();
	}
}

std::vector<Ogre::String> UnitManager::getPlayerUnits(int player)
{
	std::vector<Ogre::String> vector;
	for(iterator = units.begin(); iterator != units.end(); iterator++)
		if((*iterator)->getOwner() == player)
			vector.push_back((*iterator)->getUnitName());
	return vector;
}

void UnitManager::addTime(Ogre::Real deltaTime)
{
	for(iterator = units.begin(); iterator != units.end(); iterator++)
		if(*iterator != NULL)
			(*iterator)->addTime(deltaTime);
}
