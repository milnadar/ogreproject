#ifndef _UnitManager_
#define _UnitManager_

#include "OgreSceneManager.h"
#include "OgreSingleton.h"

class GameUnit;
struct Squad;

class UnitManager : public Ogre::Singleton<UnitManager>
{
public: 
	UnitManager(Ogre::SceneManager*);
	~UnitManager();
	GameUnit* createUnit(int player, int type);
	bool createSquad(int player);
	GameUnit* getUnitByName(Ogre::String name);
	void resetUnitsStats();
	std::vector<Ogre::String> getPlayerUnits(int player);
	void addTime(Ogre::Real);
private:
	std::vector<GameUnit*> units;
	std::vector<GameUnit*>::iterator iterator;
	Ogre::SceneManager *sceneManager;
	int unitCounter;
};

#endif