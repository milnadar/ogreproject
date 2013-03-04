#include "GameUnit.h"

GameUnit::GameUnit(int id, int player, Ogre::SceneManager *manager) : sceneManager(manager), unitDirection(Ogre::Vector3::ZERO),
	unitCell(NULL), unitNode(NULL), unitEntity(NULL), owner(player)
{
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