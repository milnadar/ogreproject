#ifndef _Gameunit_
#define _GameUnit_

#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

class Cell;

class GameUnit
{
public:
	GameUnit(int id, Ogre::SceneManager *manager);
	~GameUnit() {};
	Cell* getCell() {return unitCell;};
	Ogre::String getUnitName() const {return unitName;};
	void SetPosition(Ogre::Vector3&);
private:
	Ogre::Entity *unitEntity;
	Ogre::SceneNode *unitNode;
	Ogre::String unitName;
	Cell* unitCell;
	Ogre::SceneManager *sceneManager;
};

#endif