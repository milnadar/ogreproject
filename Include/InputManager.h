#ifndef _InputManager_
#define _InputManager_

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class InputManager{
public:
	InputManager();
	~InputManager();
	void initialise(OIS::ParamList pl);
	void release();
	void capture();
	void setEventCallbacks(OIS::KeyListener*, OIS::MouseListener*);
	OIS::Mouse* getMouse() {return mouse;}
	OIS::Keyboard* getKeyBoard() {return keyboard;}
private:
	OIS::InputManager* mInputManager;
    OIS::Mouse*    mouse;
    OIS::Keyboard* keyboard;
};

#endif