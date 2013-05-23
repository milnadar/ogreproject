#include "InputManager.h"

InputManager::InputManager()// : mInputManager(0), mouse(0), keyboard(0)
{
	mouse = 0;
	keyboard = 0;
	mInputManager = 0;
}

InputManager::~InputManager()
{
	release();
}

void InputManager::initialise(OIS::ParamList pl)
{
    mInputManager = OIS::InputManager::createInputSystem( pl );
    keyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));
}
void InputManager::release()
{
	if(mouse)
		mInputManager->destroyInputObject(mouse);
	mouse = 0;
	if(keyboard)
		mInputManager->destroyInputObject(keyboard);
	keyboard = 0;
	if(mInputManager)
		OIS::InputManager::destroyInputSystem(mInputManager);
	mInputManager = 0;
}

void InputManager::capture()
{
	if(mouse)
		mouse->capture();
	if(keyboard)
		keyboard->capture();
}

void InputManager::setEventCallbacks(OIS::KeyListener* keys, OIS::MouseListener *mouse)
{
	this->mouse->setEventCallback(mouse);
	this->keyboard->setEventCallback(keys);
}