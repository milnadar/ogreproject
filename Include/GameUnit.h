#ifndef _Gameunit_
#define _GameUnit_

#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

class Cell;

struct UnitStats{
	explicit UnitStats(int _speed, int _armor, int _mele, int _numberAttacks, int _attackPower);
	UnitStats() {};
	int movementSpeed;
	int armor;
	int meleAttack;
	int numberAttacks;
	int attackPower;
};

class GameUnit
{
public:
	enum AnimationList {WALK_ANIMATION, IDLE_ANIMATION, SHOOT_ANIMATION, DEATH_ANIMATION};
	GameUnit(int id, int player, Ogre::SceneManager *manager);
	~GameUnit() {};
	Cell* getCell() {return unitCell;};
	void setUnitCell(Cell* newCell) {unitCell = newCell;};
	Ogre::String getUnitName() const {return unitName;};
	int getOwner() const {return owner;};
	void SetPosition(Ogre::Vector3&);
	Ogre::Vector3 getPosition();
	bool TranslateUnit(Ogre::Vector3&);
	void moveOneStep();
	void makeOneShot();
	int stepsLeftToMove() {return stepsLeft;};
	int hasMoreShots() const {return numberAttacksLeft > 0;};
	void resetTurnStats();
	Ogre::SceneNode* getNode() const {return unitNode;};
	//unit methods for performing animation
	void addTime(Ogre::Real);
	void startAnimation(AnimationList, bool loop);
	void stopAnimation();
	void resetAnimation();
	Ogre::AnimationState* getAnimationState() const {return animationState;};
	bool isBlocked() const {return blocked;};
	//returns true if unit has more available shots in current turn
	bool canShoot() const {return canPerformRangeAttack;};
	void setBlocked(bool action) {blocked = action;};
	void kill() {alive = false;};
	bool isAlive() const {return alive;};
	const UnitStats& getUnitStats() const {return unitStats;};
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
	UnitStats unitStats;
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
	bool blocked;
	bool alive;
};

#endif