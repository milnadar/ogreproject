#ifndef _Gameunit_
#define _GameUnit_

#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

class Cell;

class GameUnit
{
public:
	GameUnit(int id, int player, Ogre::SceneManager *manager);
	~GameUnit() {};
	Cell* getCell() {return unitCell;};
	void setUnitCell(Cell* newCell) {unitCell = newCell;};
	Ogre::String getUnitName() const {return unitName;};
	int getOwner() {return owner;};
	void SetPosition(Ogre::Vector3&);
	Ogre::Vector3 getPosition();
	bool TranslateUnit(Ogre::Vector3&);
	void moveOneStep();
	int stepsLeftToMove() {return stepsLeft;};
	void resetTurnStats();
	Ogre::SceneNode* getNode() {return unitNode;};
private:
	Ogre::Entity *unitEntity;
	Ogre::SceneNode *unitNode;
	Ogre::String unitName;
	Ogre::String idName;
	Ogre::Vector3 unitDirection;
	Cell* unitCell;
	Ogre::SceneManager *sceneManager;
	int owner;
	//unit stats
	int movementSpeed;
	int stepsLeft;
	int armor;
	int meleAttack;
	int numberAttacks;
	int attackPower;
	bool canPerformMovement;
	bool canPerformRangeAttack;
	bool canPerformMeleAttack;
};

#endif