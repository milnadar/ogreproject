#include "GameUnit.h"

GameUnit::GameUnit(int id, int player, Ogre::SceneManager *manager) : sceneManager(manager), unitDirection(Ogre::Vector3::ZERO),
	unitCell(NULL), unitNode(NULL), unitEntity(NULL), owner(player), animationState(NULL)
{
	movementSpeed = 4;
	stepsLeft = movementSpeed;
	armor = 5;
	meleAttack = 3;
	numberAttacks = 3;
	numberAttacksLeft = numberAttacks;
	attackPower = 6;
	canPerformMovement = true;
	canPerformRangeAttack = true;
	canPerformMeleAttack = true;
	mCanPerformAction = true;
	unitName = "unit" + Ogre::StringConverter::toString(id);
	if(sceneManager)
	{
		unitEntity = sceneManager->createEntity(unitName, "robot.mesh");
		unitNode = sceneManager->getRootSceneNode()->createChildSceneNode(unitName + "node");
		unitNode->setScale(0.07, 0.07, 0.07);
		unitNode->attachObject(unitEntity);
		unitEntity->setUserAny(Ogre::Any(this));
		if(player == 2)
		{
			unitNode->yaw(Ogre::Degree(180));
		}
	}
}

void GameUnit::SetPosition(Ogre::Vector3 &position)
{
	if(unitNode != NULL)
		unitNode->setPosition(position);
}

Ogre::Vector3 GameUnit::getPosition()
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

void GameUnit::makeOneShot()
{
	if(--numberAttacksLeft < 0)
		numberAttacksLeft = 0;
}

void GameUnit::resetTurnStats()
{
	canPerformMovement = true;
	canPerformRangeAttack = true;
	canPerformMeleAttack = true;
	stepsLeft = movementSpeed;
	numberAttacksLeft = numberAttacks;
}

void GameUnit::addTime(Ogre::Real deltaTime)
{
	if(animationState != NULL)
		animationState->addTime(deltaTime);
}

void GameUnit::startAnimation(AnimationList animation, bool loop = true)
{
	Ogre::String unitAnimations[] = {"Walk", "idle", "Shoot"};
	animationState = unitEntity->getAnimationState(unitAnimations[animation]);
	animationState->setLoop(loop);
	animationState->setEnabled(true);
}

void GameUnit::stopAnimation()
{
	animationState->setEnabled(false);
}