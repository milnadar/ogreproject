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

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
	currentUnit = NULL;
	finalCell = NULL;
	attacker = NULL;
	target = NULL;
	gameConsole = NULL;
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
		if(currentUnit != NULL)
			currentUnit->getNode()->showBoundingBox(false);
		currentUnit = NULL;
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
					currentUnit = Ogre::any_cast<GameUnit*>(itr->movable->getUserAny());
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
				if (itr->movable && itr->movable->getName().find("cell") == 0)
				{
					Cell* cell = Ogre::any_cast<Cell*>(itr->movable->getUserAny());
					moveUnitToCell(currentUnit, cell);
					break;
				}
				else if(itr->movable && itr->movable->getName().find("unit") == 0)
				{
					attacker = currentUnit;
					target = Ogre::any_cast<GameUnit*>(itr->movable->getUserAny());
					break;
				}
			}
			else
			{
				if (itr->movable && itr->movable->getName().find("unit") == 0)
				{
					currentUnit = Ogre::any_cast<GameUnit*>(itr->movable->getUserAny());
					itr->movable->getParentSceneNode()->showBoundingBox(true);
					consoleOutput("Selected unit " + currentUnit->getUnitName());
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
				currentPlayer = Players::player2;
			else
				currentPlayer = Players::player1;
			updateUnitListForCurrentPlayer();
			UnitManager::getSingletonPtr()->resetUnitsStats();
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
	if(path.size() > unit->stepsLeftToMove())
		return false;
	finalCell = cell;
	for(pathItr = path.begin(); pathItr != path.end(); pathItr++)
		{walkList.push_front((*pathItr)->getEntity()->getParentSceneNode()->getPosition());
	}
	consoleOutput("Unit " + unit->getUnitName() + " moved to cell " + cell->getName());
	return true;
}

void TutorialApplication::attackScenario()
{
	if(attacker != NULL && target != NULL)
	{
		if(attacker->canShoot())
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
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
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