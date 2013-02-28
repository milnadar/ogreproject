#include "GameUnit.h"

GameUnit::GameUnit(int id, Ogre::SceneManager *manager) : sceneManager(manager)
{
	unitName = "unit" + Ogre::StringConverter::toString(id);
	if(sceneManager)
	{
		unitEntity = sceneManager->createEntity(unitName, "robot.mesh");
		unitNode = sceneManager->getRootSceneNode()->createChildSceneNode(unitName + "node");
		unitNode->attachObject(unitEntity);
	}
}