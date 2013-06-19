#ifndef _MainMenuState_h_
#define _MainMenuState_h_

#include "GameState.h"
#include <SdkTrays.h>

class MainMenuState : public GameState, public OgreBites::SdkTrayListener
{
public:
	static void Create(GameStateListener *parent, const Ogre::String &name);
	void enter(void);
	void exit(void);
private:
	MainMenuState();
	//Ogre::FrameListener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg ){return true;};
    // OIS::MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){return true;};
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){return true;};
	bool mShutdown;
	OgreBites::SdkTrayManager *trayManager;
};

MainMenuState::MainMenuState() : mShutdown(false)
{
	trayManager = 0;
}

void MainMenuState::enter(void)
{
	//trayManager = new OgreBites::SdkTrayManager("newTray", mDevice->rwindow, mDevice->mouse, this);
	trayManager = new OgreBites::SdkTrayManager("newTray", mDevice->rwindow, mDevice->manager->getMouse(), this);
}

void MainMenuState::exit(void)
{
	delete trayManager;
	trayManager = 0;
}

void MainMenuState::Create(GameStateListener *parent, const Ogre::String &name)
{
	MainMenuState *newState = new MainMenuState();
	newState->parent = parent;
	parent->ManageGameState(name, newState);
}

bool MainMenuState::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	if(mShutdown) return false;
	mDevice->manager->capture();
	return true;
}

bool MainMenuState::keyPressed(const OIS::KeyEvent &arg)
{
    if (arg.key == OIS::KC_ESCAPE)
    {
		parent->Shutdown();
		mShutdown = true;
    }
	else if(arg.key == OIS::KC_RETURN)
		parent->changeGameState(parent->findByName("mainGameState"));
	return true;
}

bool MainMenuState::mouseMoved(const OIS::MouseEvent &arg)
{
	if(trayManager->injectMouseMove(arg)) return true;
	return true;
}

#endif