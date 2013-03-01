#ifndef _UnitManager_
#define _UnitManager_

#include "OgreSceneManager.h"
//#include "GameUnit.h"
class GameUnit;

class UnitManager
{
public: 
	UnitManager(Ogre::SceneManager*);
	~UnitManager();
	GameUnit* createUnit();
private:
	std::vector<GameUnit*> units;
	Ogre::SceneManager *sceneManager;
	int unitCounter;
};

#endif