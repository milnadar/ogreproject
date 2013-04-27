#ifndef __MainGameState_h_
#define __MainGameState_h_

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
#include "GameHelper.h"
#include "GameState.h"
//#include "Network.h"
#include "Game.h"
#include <SdkCameraMan.h>

class Network;

class MainGameState : public GameState
{
public:
	static void Create(GameStateListener *parent, const Ogre::String& name);
	virtual void enter(void);
	virtual void exit(void);
    virtual void createScene(void);
	//virtual void createFrameListener(void);
    //frame listener    
    virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	virtual bool frameStarted(const Ogre::FrameEvent &evt);
	virtual bool frameEnded(const Ogre::FrameEvent &evt);
	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg ){return true;};
    //OIS::MouseListener
    virtual bool mouseMoved(const OIS::MouseEvent &arg);
    virtual bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseMovedInEditState(const OIS::MouseEvent &arg);
	bool mouseMovedInPlayState(const OIS::MouseEvent &arg);
	bool mousePressedInEditState(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mousePressedInPlayState(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	//MyGUI
	void buttonClicked(MyGUI::Widget* _widget);
	void itemAcceptedCallback(MyGUI::ListBox* _sender, size_t _index);
	void consoleOutput(Ogre::String);
	void setupGUI();
	void updateUnitListForCurrentPlayer();
	void updateUnitInformation() {};
private:
	MainGameState(const device_info* device);
	~MainGameState();
	Ogre::RaySceneQuery *mRaySceneQuery;// The ray scene query pointer
	MyGUI::ButtonPtr button;
	MyGUI::EditBox *gameConsole;
    bool mLmouseDown, mRmouseDown;		// True if the mouse buttons are down
	bool allowedToMove;
	bool interfaceBlocked;
	Network *network;
	GameHelper helper;
	GameManager* game;
	Ogre::SceneManager *sceneManager;
	Ogre::Camera *mCamera;
	float mRotateSpeed;
	GameUnit *currentUnit;
	bool isServer;
	OgreBites::SdkCameraMan *mCameraMan;
	bool loop;
};

#endif