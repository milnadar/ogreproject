#include "GameUnit.h"

UnitStats::UnitStats(int _speed, int _armor, int _mele, int _numberAttacks, int _attackPower, int _attackDistance, int _distanceModifier) : movementSpeed(_speed),
	armor(_armor), meleAttack(_mele), numberAttacks(_numberAttacks), attackPower(_attackPower), attackDistance(_attackDistance), distanceModifier(_distanceModifier)
{
	//
}

TrooperStats::TrooperStats(int _movement, int _armor, int _mele, int _numAttacks, int _attackPower, int _attackDistance, int _distModifier)
	: UnitStats(_movement, _armor, _mele, _numAttacks, _attackPower, _attackDistance, _distModifier)
{
	//
}

VehicleStats::VehicleStats(int _movement, int _armor, int _mele, int _numAttacks, int _attackPower, int _attackDistance, int _distModifier, int _ammoCount)
	: UnitStats(_movement, _armor, _mele, _numAttacks, _attackPower, _attackDistance, _distModifier), ammoCount(_ammoCount)
{
	//
}

GameUnit::GameUnit(int id, int player, Ogre::SceneManager *manager) : unitDirection(Ogre::Vector3::ZERO),
	unitCell(NULL), unitNode(NULL), unitEntity(NULL), owner(player), animationState(NULL), sceneManager(manager)
{
	canPerformMovement = true;
	canPerformRangeAttack = true;
	canPerformMeleAttack = true;
	blocked = false;
	alive = true;
	unitID = id;
}

GameUnit::~GameUnit()
{
	//
}

void GameUnit::SetPosition(const Ogre::Vector3& position)
{
	if(unitNode != NULL)
		unitNode->setPosition(position);
}

Ogre::Vector3 const GameUnit::getPosition()
{
	Ogre::Vector3 position;
	if(unitNode != NULL)
	{
		position = unitNode->getPosition();
	}
	return position;
}

bool GameUnit::TranslateUnit(Ogre::Vector3 &vector)
{
	if(unitNode != NULL)
	{
		unitNode->translate(vector);
		return true;
	}
	return false;
}

void GameUnit::moveOneStep()
{
	if(--stepsLeft < 0)
		stepsLeft = 0;
}

void GameUnit::addTime(Ogre::Real deltaTime)
{
	if(animationState != NULL)
		animationState->addTime(deltaTime);
}

void GameUnit::startAnimation(AnimationList animation, bool loop = true)
{
	Ogre::String unitAnimations[] = {"Walk", "idle", "Shoot", "Die"};
	Ogre::String animationName = unitAnimations[animation];
	if(unitEntity->hasAnimationState(animationName))
	{	
		animationState = unitEntity->getAnimationState(animationName);
		animationState->setLoop(loop);
		animationState->setEnabled(true);
	}
}

void GameUnit::stopAnimation()
{
	if(animationState != NULL)
		animationState->setEnabled(false);
}

void GameUnit::resetAnimation()
{
	if(animationState != NULL)
		animationState->setTimePosition(0);
}

void GameUnit::setVisible(bool visible)
{
	if(sceneManager != NULL)
	{
		if(unitEntity != NULL)
			unitEntity->setVisible(visible);
	}
}

Trooper::Trooper(int id, int player, TrooperStats stats, Ogre::SceneManager *manager)
	: GameUnit(id, player, manager), unitStats(stats)
{
	unitName = "trooper" + Ogre::StringConverter::toString(id);
	if(sceneManager)
	{
		unitEntity = sceneManager->createEntity(unitName, "robot.mesh");
		unitNode = sceneManager->getRootSceneNode()->createChildSceneNode(unitName + "node");
		unitNode->setScale(0.05, 0.05, 0.05);
		unitNode->attachObject(unitEntity);
		unitEntity->setUserAny(Ogre::Any(this));
		if(player == 2)
		{
			unitNode->yaw(Ogre::Degree(180));
		}
	}
	unitType = UnitType::TROOPER;
	stepsLeft = stats.movementSpeed;
	numberAttacksLeft = stats.attackPower;
}

void Trooper::makeOneShot()
{
	canPerformRangeAttack = false;
	if(--numberAttacksLeft < 0)
		numberAttacksLeft = 0;
}

void Trooper::resetTurnStats()
{
	canPerformMovement = true;
	canPerformRangeAttack = true;
	canPerformMeleAttack = true;
	stepsLeft = unitStats.movementSpeed;
	numberAttacksLeft = unitStats.attackPower;
}

Vehicle::Vehicle(int id, int player, VehicleStats stats, Ogre::SceneManager *manager)
	: GameUnit(id, 0, manager), unitStats(stats), pilot(NULL)
{
	unitName = "vehicle" + Ogre::StringConverter::toString(id);
	if(sceneManager)
	{
		unitEntity = sceneManager->createEntity(unitName, "car.mesh");
		unitNode = sceneManager->getRootSceneNode()->createChildSceneNode(unitName + "node");
		unitNode->attachObject(unitEntity);
		unitEntity->setUserAny(Ogre::Any(this));
		if(player == 2)
		{
			unitNode->yaw(Ogre::Degree(180));
		}
	}
	unitType = UnitType::VEHICLE;
	stepsLeft = stats.movementSpeed;
	numberAttacksLeft = stats.attackPower;
}

void Vehicle::makeOneShot()
{
	canPerformRangeAttack = false;
	if(--numberAttacksLeft < 0)
		numberAttacksLeft = 0;
	if(--unitStats.ammoCount < 0)
		unitStats.ammoCount = 0;
}

void Vehicle::resetTurnStats()
{
	canPerformMovement = true;
	canPerformRangeAttack = unitStats.ammoCount > 0;
	canPerformMeleAttack = true;
	stepsLeft = unitStats.movementSpeed;
	numberAttacksLeft = unitStats.attackPower;
}

bool Vehicle::setUnitIn(GameUnit* unit)
{
	if(unit != NULL && pilot == NULL)
	{
		pilot = unit;
		owner = unit->getOwner();
		return true;
	}
	return false;
}

bool Vehicle::canEject()
{
	return stepsLeft != 0;
}

GameUnit* Vehicle::ejectPilot()
{
	GameUnit *unit = NULL;
	if(pilot != NULL)
	{
		owner = 0;
		unit = pilot;
		pilot = NULL;
		//after unit enetered vehicle he can not drive it in this turn
		//stepsLeft = 0;
		//canPerformMovement = false;
	}
	return unit;
}