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
	MyGUI::Gui *gui;
	MyGUI::OgrePlatform *platform = new MyGUI::OgrePlatform();
	platform->initialise(mWindow, mSceneMgr);
	gui = new MyGUI::Gui;
	gui->initialise();
	button = gui->createWidget<MyGUI::Button>("Button", 10, 10, 300, 26, MyGUI::Align::Default, "Main");
	button->setCaption("exit");
	button = gui->createWidget<MyGUI::Button>("Button", 10, 46, 300, 26, MyGUI::Align::Default, "Main");
	button->setCaption("Create unit");
	UnitManager *unitManager = new UnitManager(mSceneMgr);
	field = new GameField(mSceneMgr);
	field->setupField();
	Cell *cell1, *cell2;
	cell1 = new Cell(0, 0, "11");
	cell2 = new Cell(4, 8, "48");
	field->findPath(cell1, cell2);
	currentUnit = unitManager->createUnit();
	field->setUnitOnCell(field->getCellByIndex(0, 0), currentUnit);
    // create your scene here :)
}
bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	//
	if(direction == Ogre::Vector3::ZERO)
	{
		nextLocation();
	}
	else
	{
		Ogre::Real move = walkSpeed * evt.timeSinceLastFrame;
		distance -= move;
		if(distance <= 0.00f)
		{
			currentUnit->SetPosition(destination);
			direction = Ogre::Vector3::ZERO;
		}
		else
		{
			currentUnit->getUnitNode()->translate(direction * move);
		}
	}

	return BaseApplication::frameRenderingQueued(evt);
}
bool TutorialApplication::mouseMoved(const OIS::MouseEvent &arg)
{
	MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
	if(mRmouseDown)
	{
		mCamera->yaw(Ogre::Degree(-arg.state.X.rel * mRotateSpeed));
		mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * mRotateSpeed));
	}
	if(currentUnit != NULL)
	{
		MyGUI::IntPoint position = MyGUI::InputManager::getInstance().getMousePosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(position.left/float(arg.state.width),position.top/float(arg.state.height));
        mRaySceneQuery->setRay(mouseRay);
 
        Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator itr = result.begin();
 
		if (itr != result.end() && itr->movable)
		{
			//Ogre::Vector3 position = itr->movable->getParentSceneNode()->getPosition();
			//currentUnit->SetPosition(position);
		}
	}
	return true;
}
bool TutorialApplication::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		MyGUI::IntPoint position = MyGUI::InputManager::getInstance().getMousePosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(position.left/float(arg.state.width),position.top/float(arg.state.height));
        mRaySceneQuery->setRay(mouseRay);
 
        Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator itr;
		for(itr = result.begin(); itr != result.end(); itr++){
			if(currentUnit != NULL)
			{
				if (itr->movable && itr->movable->getName().find("cell") == 0)
				{
					Cell* cell = Ogre::any_cast<Cell*>(itr->movable->getUserAny());
					std::vector<Cell*> path;
					std::vector<Cell*>::iterator pathItr;
					path = field->findPath(currentUnit->getCell(), cell);
					for(pathItr = path.begin(); pathItr != path.end(); pathItr++)
						walkList.push_front((*pathItr)->getEntity()->getParentSceneNode()->getPosition());

					break;
				}
				///for edit state
				/*if (itr->movable && itr->movable->getName().find("cell") == 0)
				{
					Cell* cell = Ogre::any_cast<Cell*>(itr->movable->getUserAny());
					field->setUnitOnCell(cell, currentUnit);
					currentUnit = NULL;
					break;
				}*/
			}
			else
			{
				if (itr->movable && itr->movable->getName().find("unit") == 0)
				{
					currentUnit = Ogre::any_cast<GameUnit*>(itr->movable->getUserAny());
					Cell *cell = currentUnit->getCell();
					break;
				}
			}
		}
		//if (itr != result.end() && itr->movable)
		//	button->setCaption(itr->movable->getName().c_str());
		mLmouseDown = true;
	}
	else if(id == OIS::MB_Right)
	{
		currentUnit = NULL;
		mRmouseDown = true;
		MyGUI::PointerManager::getInstance().setVisible(false);
	}
	return true;
}
bool TutorialApplication::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
		if(id == OIS::MB_Left)
		mLmouseDown = false;
	else if(id == OIS::MB_Right)
	{
		mRmouseDown = false;
		MyGUI::PointerManager::getInstance().setVisible(true);
	}
	return true;
}

bool TutorialApplication::nextLocation()
{
	if(walkList.empty())
		return false;
	destination = walkList.front();
	walkList.pop_front();
	direction = destination - currentUnit->getPosition();
	distance = direction.normalise();
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
