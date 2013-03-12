/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#include "TutorialApplication.h"
#include "UnitManager.h"
#include <cstdlib>

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
	currentUnit = NULL;
	finalCell = NULL;
	attacker = NULL;
	target = NULL;
	gameConsole = NULL;
	interfaceBlocked = false;
	gameState = GameState::PlayState;
	currentPlayer = Players::player1;
}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
	//mSceneMgr->destroyQuery(mRaySceneQuery);
}

//-------------------------------------------------------------------------------------

void TutorialApplication::createFrameListener()
{
	BaseApplication::createFrameListener();
	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
    mLmouseDown = false;
	mRmouseDown = false;
    mRotateSpeed = .2;
	walkSpeed = 10.0f;
	direction = Ogre::Vector3::ZERO;
}

void TutorialApplication::createScene(void)
{
	mCamera->setPosition(-80, 80, 30);
	mCamera->lookAt(80, 0, 30);
	setupGUI();
	setupScene();
    // create your scene here :)
}

void TutorialApplication::setupGUI()
{
	MyGUI::Gui *gui;
	MyGUI::OgrePlatform *platform = new MyGUI::OgrePlatform();
	platform->initialise(mWindow, mSceneMgr);
	gui = new MyGUI::Gui;
	gui->initialise();
	button = gui->createWidget<MyGUI::Button>("Button", 10, 10, 300, 26, MyGUI::Align::Default, "Main", "changeGameStateButton");
	button->setCaption("Change game state");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &TutorialApplication::buttonClicked);
	button = gui->createWidget<MyGUI::Button>("Button", 10, 46, 300, 26, MyGUI::Align::Default, "Main", "createUnitButton");
	button->setCaption("Create unit");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &TutorialApplication::buttonClicked);
	button = gui->createWidget<MyGUI::Button>("Button", 10, 82, 300, 26, MyGUI::Align::Default, "Main", "changePlayerButton");
	button->setCaption("Change player");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &TutorialApplication::buttonClicked);
	MyGUI::ListBox *list = gui->createWidget<MyGUI::ListBox>("ListBox", 10, 120, 300, 100, MyGUI::Align::Default, "Main", "unitList");
	list->eventListSelectAccept += MyGUI::newDelegate(this, &TutorialApplication::itemAcceptedCallback);
	list->setVisible(false);
	gameConsole = gui->createWidget<MyGUI::EditBox>("EditBox",500, 780, 500, 150, MyGUI::Align::Default, "Main", "console");
	gameConsole->setEditReadOnly(true);
	gameConsole->setEditMultiLine(true);
	gameConsole->setTextAlign(MyGUI::Align::Bottom | MyGUI::Align::Left);
}

void TutorialApplication::updateUnitListForCurrentPlayer()
{
	std::vector<Ogre::String> names = UnitManager::getSingletonPtr()->getPlayerUnits(currentPlayer);
	MyGUI::ListBox *listBox = MyGUI::Gui::getInstance().findWidget<MyGUI::ListBox>("unitList");
	if(listBox)
	{
		listBox->deleteAllItems();
		std::vector<Ogre::String>::iterator itr;
		for(itr = names.begin(); itr != names.end(); itr++)
			listBox->addItem(*itr);
	}
}

void TutorialApplication::setupScene()
{
	UnitManager *unitManager = new UnitManager(mSceneMgr);
	field = new GameField(mSceneMgr);
	field->setupField();
	currentUnit = UnitManager::getSingletonPtr()->createUnit(currentPlayer);
	field->setUnitOnCell(field->getCellByIndex(0, 0), currentUnit);
	UnitManager::getSingletonPtr()->createSquad(currentPlayer);
	updateUnitListForCurrentPlayer();
	//Ogre::Entity *entity = mSceneMgr->createEntity("tube", "tube.mesh");
	//Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodetube");
	//node->attachObject(entity);
	//node->setScale(2,2,2);
	//node->setPosition(25, 10, 50);
}

void TutorialApplication::changeGameState()
{
	MyGUI::ListBox *listBox = MyGUI::Gui::getInstance().findWidget<MyGUI::ListBox>("unitList");
	if(gameState == GameState::PlayState)
	{
		gameState = GameState::EditState;
		listBox->setVisible(true);
	}
	else
	{
		gameState = GameState::PlayState;
		listBox->setVisible(false);
	}
	currentUnit = NULL;
}

void TutorialApplication::selectUnit(GameUnit *unit)
{
	if(unit != NULL)
	{
		currentUnit = unit;
		//currentUnit->getNode()->showBoundingBox(true);
		consoleOutput("Selected unit " + currentUnit->getUnitName());
		field->showavailableCellsToMove(currentUnit, true);
	}
}

void TutorialApplication::deselectCurrentUnit()
{
	if(currentUnit != NULL)
	{
		currentUnit->getNode()->showBoundingBox(false);
		field->showavailableCellsToMove(currentUnit, false);
		currentUnit = NULL;
	}
}

void TutorialApplication::endTurn()
{
	deselectCurrentUnit();
	attacker = NULL;
	target = NULL;
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	UnitManager::getSingletonPtr()->addTime(evt.timeSinceLastFrame);
	attackScenario();
	if(direction == Ogre::Vector3::ZERO)
	{
		if(nextLocation())
		{
			Ogre::Vector3 src = currentUnit->getNode()->getOrientation() * Ogre::Vector3::UNIT_X;
			Ogre::Quaternion quat = src.getRotationTo(direction);
			currentUnit->getNode()->rotate(quat);
			currentUnit->startAnimation(GameUnit::AnimationList::WALK_ANIMATION, true);
		}
	}
	else
	{
		Ogre::Real move = walkSpeed * evt.timeSinceLastFrame;
		distance -= move;
		if(distance <= 0.00f)
		{
			currentUnit->SetPosition(destination);
			currentUnit->moveOneStep();
			direction = Ogre::Vector3::ZERO;
			if(!nextLocation())
			{
				currentUnit->stopAnimation();
				field->setUnitOnCell(finalCell, currentUnit);
				field->showavailableCellsToMove(currentUnit, true);
				finalCell = NULL;
			}
			else
			{
				Ogre::Vector3 src = currentUnit->getNode()->getOrientation() * Ogre::Vector3::UNIT_X;
				if ((1.0f + src.dotProduct(direction)) < 0.0001f) 
				{
					currentUnit->getNode()->yaw(Ogre::Degree(180));
				}
				else
				{
					Ogre::Quaternion quat = src.getRotationTo(direction);
					currentUnit->getNode()->rotate(quat);
				} // else
			}
		}
		else
		{
			currentUnit->TranslateUnit(direction * move);
		}
	}

	return BaseApplication::frameRenderingQueued(evt);
}
bool TutorialApplication::mouseMoved(const OIS::MouseEvent &arg)
{
	bool result = true;
	MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
	if(mRmouseDown)
	{
		mCamera->yaw(Ogre::Degree(-arg.state.X.rel * mRotateSpeed));
		mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * mRotateSpeed));
	}
	if(gameState == GameState::EditState)
		result = mouseMovedInEditState(arg);
	else if(gameState == GameState::PlayState)
		result = mouseMovedInPlayState(arg);
	return result;
}

bool TutorialApplication::mouseMovedInEditState(const OIS::MouseEvent &arg)
{
	if(currentUnit != NULL)
	{
		MyGUI::IntPoint position = MyGUI::InputManager::getInstance().getMousePosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(position.left/float(arg.state.width),position.top/float(arg.state.height));
        mRaySceneQuery->setRay(mouseRay);
 
        Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator itr = result.begin();
 
		if (itr != result.end() && itr->movable)
		{
			Ogre::Vector3 position = itr->movable->getParentSceneNode()->getPosition();
			currentUnit->SetPosition(position);
		}
	}
	return true;
}

bool TutorialApplication::mouseMovedInPlayState(const OIS::MouseEvent &arg)
{
	return true;
}

bool TutorialApplication::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	MyGUI::InputManager::getInstance().injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
	bool result = true;
	if(id == OIS::MB_Left)
	{
		mLmouseDown = true;
	}
	else if(id == OIS::MB_Right)
	{
		mRmouseDown = true;
		MyGUI::PointerManager::getInstance().setVisible(false);
		deselectCurrentUnit();
	}
	if(gameState == GameState::EditState)
		result = mousePressedInEditState(arg, id);
	else if(gameState == GameState::PlayState)
		result = mousePressedInPlayState(arg, id);
	return result;
}

bool TutorialApplication::mousePressedInEditState(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	if(mLmouseDown)
	{
		MyGUI::IntPoint position = MyGUI::InputManager::getInstance().getMousePosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(position.left/float(arg.state.width),position.top/float(arg.state.height));
        mRaySceneQuery->setRay(mouseRay);
 
        Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator itr;
		for(itr = result.begin(); itr != result.end(); itr++)
		{
			if(currentUnit != NULL)
			{
				if (itr->movable && itr->movable->getName().find("cell") == 0)
				{
					Cell* cell = Ogre::any_cast<Cell*>(itr->movable->getUserAny());
					field->setUnitOnCell(cell, currentUnit);
					currentUnit = NULL;
					break;
				}
			}
			else
			{
				if (itr->movable && itr->movable->getName().find("unit") == 0)
				{
					selectUnit(Ogre::any_cast<GameUnit*>(itr->movable->getUserAny()));
					break;
				}
			}
		}
	}
	else if(mRmouseDown)
	{
		//
	}
	return true;
}

bool TutorialApplication::mousePressedInPlayState(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	if(mLmouseDown)
	{
		MyGUI::IntPoint position = MyGUI::InputManager::getInstance().getMousePosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(position.left/float(arg.state.width),position.top/float(arg.state.height));
        mRaySceneQuery->setRay(mouseRay);
		mRaySceneQuery->setSortByDistance(true);
 
        Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator itr;
		for(itr = result.begin(); itr != result.end(); itr++)
		{
			if(currentUnit != NULL)
			{
				if(itr->movable && itr->movable->getName().find("unit") == 0)
				{
					GameUnit* pointedUnit = Ogre::any_cast<GameUnit*>(itr->movable->getUserAny());
					if(pointedUnit->getOwner() != currentPlayer)
					{
						if(currentUnit->canShoot())
						{
							GameUnit *targetUnit = Ogre::any_cast<GameUnit*>(itr->movable->getUserAny());
							performRangeAttack(currentUnit, targetUnit);
						}
					}
					else
					{
						//deselectCurrentUnit();
						currentUnit = pointedUnit;
						selectUnit(currentUnit);
					}
					break;
				}
				else if (itr->movable && itr->movable->getName().find("cell") == 0)
				{
					Cell* cell = Ogre::any_cast<Cell*>(itr->movable->getUserAny());
					moveUnitToCell(currentUnit, cell);
					break;
				}
			}
			else
			{
				if (itr->movable && itr->movable->getName().find("unit") == 0)
				{
					selectUnit(Ogre::any_cast<GameUnit*>(itr->movable->getUserAny()));
					break;
				}
			}
		}
	}
	else if(mRmouseDown)
	{
		//
	}
	return true;
}

bool TutorialApplication::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	MyGUI::InputManager::getInstance().injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
		if(id == OIS::MB_Left)
		mLmouseDown = false;
	else if(id == OIS::MB_Right)
	{
		mRmouseDown = false;
		MyGUI::PointerManager::getInstance().setVisible(true);
	}
	return true;
}

void TutorialApplication::buttonClicked(MyGUI::Widget* _widget)
{
	if(_widget != NULL)
	{
		if(_widget->getName() == "createUnitButton")
		{
			if(gameState == GameState::EditState)
				currentUnit = UnitManager::getSingletonPtr()->createUnit(currentPlayer);
		}
		else if(_widget->getName() == "changeGameStateButton")
		{
			changeGameState();
		}
		else if(_widget->getName() == "changePlayerButton")
		{
			if(currentPlayer == Players::player1)
			{
				currentPlayer = Players::player2;
				//mRaySceneQuery->setQueryMask(QueryMask::PLAYER_2);
			}
			else
			{
				currentPlayer = Players::player1;
				//mRaySceneQuery->setQueryMask(QueryMask::PLAYER_1);
			}
			updateUnitListForCurrentPlayer();
			UnitManager::getSingletonPtr()->resetUnitsStats();
			endTurn();
		}
	}
}

void TutorialApplication::itemAcceptedCallback(MyGUI::ListBox* _sender, size_t _index)
{
	Ogre::String str = _sender->getItem(_index);
	currentUnit = UnitManager::getSingletonPtr()->getUnitByName(str);
}

bool TutorialApplication::nextLocation()
{
	if(walkList.empty())
		return false;
	destination = walkList.front();
	walkList.pop_front();
	direction = destination - currentUnit->getPosition();
	distance = direction.normalise();
	return true;
}

bool TutorialApplication::moveUnitToCell(GameUnit *unit, Cell* cell)
{
	std::vector<Cell*> path;
	std::vector<Cell*>::iterator pathItr;
	path = field->findPath(unit->getCell(), cell);
	if(path.size() > unit->stepsLeftToMove() || path.empty())
		return false;
	finalCell = cell;
	for(pathItr = path.begin(); pathItr != path.end(); pathItr++)
		{walkList.push_front((*pathItr)->getEntity()->getParentSceneNode()->getPosition());
	}
	consoleOutput("Unit " + unit->getUnitName() + " moved to cell " + cell->getName());
	return true;
}

void TutorialApplication::performRangeAttack(GameUnit* attacker, GameUnit *target)
{
	if(attacker != target)
	{
		//calculate distance between two units
		int distance = getDistance(attacker->getPosition(), target->getPosition());
		consoleOutput("distance = " + Ogre::StringConverter::toString(distance));
		//calculate distance check for attacker unit
		int distanceResult = calculateDistance(attacker->getUnitStats().attackDistance, attacker->getUnitStats().distanceModifier);
		if(distanceResult < distance)
			return;
		this->attacker = attacker;
		this->target = target;
	}
}

bool TutorialApplication::calculateRangeAttack(const UnitStats &attacker, const UnitStats &target)
{
	int shot = rand() % attacker.attackPower + 1;
	if(shot > target.armor)
	{
		Ogre::String log("Hit (" + Ogre::StringConverter::toString(shot) + " > " + Ogre::StringConverter::toString(target.armor) + ')');
		consoleOutput(log);
		return true;
	}
	else
	{
		Ogre::String log("Miss (" + Ogre::StringConverter::toString(shot) + " < " + Ogre::StringConverter::toString(target.armor)+ ')');
		consoleOutput(log);
	}
	return false;
}

int TutorialApplication::calculateDistance(int distance, int modifier)
{
	int value = rand() % distance;
	value += modifier;
	consoleOutput("Distance check result: " + Ogre::StringConverter::toString(value) + " + " + Ogre::StringConverter::toString(modifier) +
		+ " = " + Ogre::StringConverter::toString(value + modifier));
	return value;
}

int TutorialApplication::getDistance(const GameUnit *attacker, const GameUnit *target)
{
	//Ogre::Vector3 position1 = _attacker->getPosition();
	//Ogre::Vector3 position2 = _target->getPosition();
	return 0;
}

int TutorialApplication::getDistance(const Ogre::Vector3 &position1, const Ogre::Vector3 &position2)
{
	Ogre::Vector3 vector = position2 - position1;
	int distance = std::floor((vector.normalise() / 5) + 0.5);
	return distance;
}

void TutorialApplication::attackScenario()
{
	if(attacker != NULL && target != NULL)
	{
		if(attacker->hasMoreShots())
		{
			if(!attacker->isBlocked())
			{
				attacker->setBlocked(true);
				attacker->makeOneShot();
				attacker->startAnimation(GameUnit::AnimationList::SHOOT_ANIMATION, false);
				if(bool result = calculateRangeAttack(attacker->getUnitStats(), target->getUnitStats()))
					target->kill();
			}
			if(attacker->getAnimationState()->hasEnded())
			{
				attacker->resetAnimation();
				attacker->setBlocked(false);
				if(!target->isAlive())
					attacker = NULL;
			}
		}
	}
	if(target != NULL && !target->isAlive())
	{
		target->startAnimation(GameUnit::AnimationList::DEATH_ANIMATION, false);
		target = NULL;
	}
}

void TutorialApplication::consoleOutput(Ogre::String string)
{
	if(gameConsole)
	{
		gameConsole->addText(string + '\n');
	}
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBoxA( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif