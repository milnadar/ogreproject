#ifndef _Gameunit_
#define _GameUnit_

#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

class Cell;

class GameUnit
{
public:
	enum AnimationList {WALK_ANIMATION, IDLE_ANIMATION, SHOOT_ANIMATION};
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
	void makeOneShot();
	int stepsLeftToMove() {return stepsLeft;};
	int getNumberOfAttacksLeft() {return numberAttacksLeft;};
	void resetTurnStats();
	Ogre::SceneNode* getNode() {return unitNode;};
	void addTime(Ogre::Real);
	void startAnimation(AnimationList, bool loop);
	void stopAnimation();
	Ogre::AnimationState* getAnimationState() {return animationState;};
	bool canPerformAction() {return mCanPerformAction;};
	void setActionAvailable(bool action) {mCanPerformAction = action;};
private:
	Ogre::Entity *unitEntity;
	Ogre::SceneNode *unitNode;
	Ogre::String unitName;
	Ogre::String idName;
	Ogre::Vector3 unitDirection;
	Ogre::AnimationState *animationState;
	Cell* unitCell;
	Ogre::SceneManager *sceneManager;
	int owner;
	//unit stats
	int movementSpeed;
	int stepsLeft;
	int armor;
	int meleAttack;
	int numberAttacks;
	int numberAttacksLeft;
	int attackPower;
	bool canPerformMovement;
	bool canPerformRangeAttack;
	bool canPerformMeleAttack;
	bool mCanPerformAction;
};

#endif