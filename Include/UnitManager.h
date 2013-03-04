#ifndef _UnitManager_
#define _UnitManager_

#include "OgreSceneManager.h"
#include "OgreSingleton.h"

class GameUnit;

class UnitManager : public Ogre::Singleton<UnitManager>
{
public: 
	UnitManager(Ogre::SceneManager*);
	~UnitManager();
	GameUnit* createUnit(int player);
private:
	std::vector<GameUnit*> units;
	Ogre::SceneManager *sceneManager;
	int unitCounter;
};

#endif