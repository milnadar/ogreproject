#include "GameUnit.h"

GameUnit::GameUnit(int id, Ogre::SceneManager *manager) : sceneManager(manager)
{
	unitName = "unit" + Ogre::StringConverter::toString(id);
	if(sceneManager)
	{
		unitEntity = sceneManager->createEntity(unitName, "robot.mesh");
		unitNode = sceneManager->getRootSceneNode()->createChildSceneNode(unitName + "node");
		unitNode->setScale(0.05, 0.05, 0.05);
		unitNode->attachObject(unitEntity);
		unitEntity->setUserAny(Ogre::Any(this));
	}
}

void GameUnit::SetPosition(Ogre::Vector3 &position)
{
	if(unitNode != NULL)
		unitNode->setPosition(position);
}