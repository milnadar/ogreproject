/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
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
#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include "BaseApplication.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
#include "GameField.h"

class TutorialApplication : public BaseApplication
{
public:
    TutorialApplication(void);
    virtual ~TutorialApplication(void);

protected:
	enum GameState {PlayState = 0, EditState};
    virtual void createScene(void);
	virtual void createFrameListener(void);
    //frame listener    
    virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
    //mouse listener
    virtual bool mouseMoved(const OIS::MouseEvent &arg);
	bool mouseMovedInEditState(const OIS::MouseEvent &arg);
	bool mouseMovedInPlayState(const OIS::MouseEvent &arg);
    virtual bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mousePressedInEditState(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mousePressedInPlayState(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	void buttonClicked(MyGUI::Widget* _widget);
	void itemAcceptedCallback(MyGUI::ListBox* _sender, size_t _index);
	void setupGUI();
	void updateUnitListForCurrentPlayer();
	void setupScene();
	void changeGameState();
private:
	enum Players {player1 = 1, player2};
	bool moveUnitToCell(GameUnit*, Cell*);
	bool nextLocation();
	Ogre::RaySceneQuery *mRaySceneQuery;// The ray scene query pointer
    bool mLmouseDown, mRmouseDown;		// True if the mouse buttons are down
    float mRotateSpeed;
	MyGUI::ButtonPtr button;
	GameField *field;
	GameUnit *currentUnit;
	Ogre::Real distance;
	Ogre::Vector3 destination;
	Ogre::Vector3 direction;
	Ogre::Real walkSpeed;
	bool allowedToMove;
	std::deque<Ogre::Vector3> walkList;
	GameState gameState;
	Players currentPlayer;
	Cell* finalCell;
};

#endif // #ifndef __TutorialApplication_h_
