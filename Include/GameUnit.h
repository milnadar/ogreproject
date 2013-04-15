#ifndef _Gameunit_
#define _GameUnit_

#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

class Cell;

enum UnitType {TROOPER = 1, VEHICLE};

struct UnitStats{
	UnitStats(int _speed, int _armor, int _mele, int _numberAttacks, int _attackPower, int _attackDistance, int _distanceModifier);
	//UnitStats(const UnitStats& rhs);
	//UnitStats(UnitStats& rhs);
	int movementSpeed;
	int armor;
	int meleAttack;
	int numberAttacks;
	int attackPower;
	int attackDistance;
	int distanceModifier;
};

struct VehicleStats : public UnitStats{
	VehicleStats(int _movement, int _armor, int _mele, int _numAttacks, int _attackPower, int attackDistance, int distModifier, int _ammoCount);
	//VehicleStats(const VehicleStats& rhs);
	//VehicleStats(VehicleStats& rhs);
	int ammoCount;
};

struct TrooperStats : public UnitStats{
	//TrooperStats(const TrooperStats& rhs);
	//TrooperStats(TrooperStats& rhs );
	TrooperStats(int _movement, int _armor, int _mele, int _numAttacks, int _attackPower, int attackDistance, int distModifier);
};

class GameUnit
{
public:
	enum AnimationList {WALK_ANIMATION, IDLE_ANIMATION, SHOOT_ANIMATION, DEATH_ANIMATION};
	GameUnit(int id, int player, Ogre::SceneManager *manager);
	~GameUnit();
	Cell* getCell() const {return unitCell;};
	void setUnitCell(Cell* newCell) {unitCell = newCell;};
	Ogre::String getUnitName() const {return unitName;};
	int getOwner() const {return owner;};
	void SetPosition(const Ogre::Vector3& position);
	Ogre::Vector3 const getPosition();
	bool TranslateUnit(Ogre::Vector3&);
	void moveOneStep();
	virtual void makeOneShot() = 0;
	int stepsLeftToMove() const {return stepsLeft;};
	bool hasMoreShots() const {return numberAttacksLeft > 0;};
	virtual void resetTurnStats() = 0;
	Ogre::SceneNode* getNode() const {return unitNode;};
	//unit methods for performing animation
	void addTime(Ogre::Real);
	void startAnimation(AnimationList, bool loop);
	void stopAnimation();
	void resetAnimation();
	Ogre::AnimationState* getAnimationState() const {return animationState;};
	bool isBlocked() const {return blocked;};
	//returns true if unit has more available shots in current turn
	virtual bool canShoot() const {return canPerformRangeAttack;};
	void setBlocked(bool action) {blocked = action;};
	void kill() {alive = false;};
	bool isAlive() const {return alive;};
	virtual const UnitStats* getUnitStats() const = 0;
	UnitType getType() const {return unitType;};
	void setVisible(bool visible);
	bool playable() const {return unitCell != NULL;};
protected:
	Ogre::Entity *unitEntity;
	Ogre::SceneNode *unitNode;
	Ogre::String unitName;
	Ogre::String idName;
	Ogre::Vector3 unitDirection;
	Ogre::AnimationState *animationState;
	Cell* unitCell;
	Ogre::SceneManager *sceneManager;
	int owner;
	int stepsLeft;
	int numberAttacksLeft;
	bool canPerformMovement;
	bool canPerformRangeAttack;
	bool canPerformMeleAttack;
	bool blocked;
	bool alive;
	UnitType unitType;
};

class Trooper : public GameUnit{
public:
	Trooper(int id, int player, TrooperStats stats, Ogre::SceneManager *manager);
	~Trooper();
	virtual const UnitStats* getUnitStats() const {return &unitStats;};
	virtual void makeOneShot();
	virtual bool canShoot() const {return canPerformRangeAttack;};
	virtual void resetTurnStats();
private:
	TrooperStats unitStats;
};

class Vehicle : public GameUnit{
public:
	Vehicle(int id, int player, VehicleStats stats, Ogre::SceneManager *manager);
	~Vehicle();
	virtual const UnitStats* getUnitStats() const {return &unitStats;};
	virtual void makeOneShot();
	virtual bool canShoot() const {return canPerformRangeAttack && unitStats.ammoCount != 0;};
	virtual void resetTurnStats();
	bool setUnitIn(GameUnit *pilot);
	bool canEject();
	GameUnit* ejectPilot();
private:
	GameUnit *pilot;
	VehicleStats unitStats;
};

#endif