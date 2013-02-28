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

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
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
}

void TutorialApplication::createScene(void)
{
	mCamera->setPosition(0, 50, 0);
	mCamera->lookAt(50, 0, 50);
	MyGUI::Gui *gui;
	MyGUI::OgrePlatform *platform = new MyGUI::OgrePlatform();
	platform->initialise(mWindow, mSceneMgr);
	gui = new MyGUI::Gui;
	gui->initialise();
	button = gui->createWidget<MyGUI::Button>("Button", 10, 10, 300, 26, MyGUI::Align::Default, "Main");
	button->setCaption("exit");
	field = new GameField(mSceneMgr);
	field->setupField();
	Cell *cell1, *cell2;
	cell1 = new Cell(0, 0, "11");
	cell2 = new Cell(4, 8, "48");
	field->findPath(cell1, cell2);
    // create your scene here :)
}
bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
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
        Ogre::RaySceneQueryResult::iterator itr = result.begin();
 
		if (itr != result.end() && itr->movable)
			button->setCaption(itr->movable->getName().c_str());
		mLmouseDown = true;
	}
	else if(id == OIS::MB_Right)
	{
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
