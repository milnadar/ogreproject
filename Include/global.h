#ifndef _global_h_
#define _global_h_

#include <OgreRoot.h>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include "InputManager.h"

typedef struct
{
    //Orbits::ConfigManager *config;
    Ogre::Root *ogre;
    Ogre::RenderWindow *rwindow;
    OIS::InputManager *InputMgr;
    OIS::Keyboard *keyboard;
    OIS::Mouse *mouse;
	InputManager *manager;
    //CEGUI::OgreCEGUIRenderer *GUIRenderer;
    //CEGUI::System *GUISystem;
} device_info;

#endif