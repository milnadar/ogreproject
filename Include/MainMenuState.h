#include "GameState.h"
#include <SdkTrays.h>

class MainMenuState : public GameState, public OgreBites::SdkTrayListener
{
public:
	MainMenuState();
	static void Create(GameStateManager *parent, const Ogre::String &name);
	void enter(void);
	void exit(void);
private:
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
	trayManager = new OgreBites::SdkTrayManager("newTray", mDevice->rwindow, mDevice->mouse, this);
}

void MainMenuState::exit(void)
{
	delete trayManager;
	trayManager = 0;
}

void MainMenuState::Create(GameStateManager *parent, const Ogre::String &name)
{
	MainMenuState *newState = new MainMenuState();
	newState->parent = parent;
	parent->ManageGameState(name, newState);
}

bool MainMenuState::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	if(mShutdown) return false;
	return true;
}

bool MainMenuState::keyPressed(const OIS::KeyEvent &arg)
{
    if (arg.key == OIS::KC_ESCAPE)
    {
		mShutdown = true;
    }
	return true;
}

bool MainMenuState::mouseMoved(const OIS::MouseEvent &arg)
{
	if(trayManager->injectMouseMove(arg)) return true;;
	return true;
}