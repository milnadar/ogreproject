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
	void setUnitCell(Cell* newCell) {unitCell = newCell;};
	Ogre::String getUnitName() const {return unitName;};
	void SetPosition(Ogre::Vector3&);
	Ogre::Vector3 getPosition();
	bool TranslateUnit(Ogre::Vector3&);
private:
	Ogre::Entity *unitEntity;
	Ogre::SceneNode *unitNode;
	Ogre::String unitName;
	Ogre::Vector3 unitDirection;
	Cell* unitCell;
	Ogre::SceneManager *sceneManager;
};

#endif