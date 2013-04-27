#ifndef __GameState_h__
#define __GameState_h__
 
#include "global.h"
 
class GameState;

class GameStateListener
{
public:
    GameStateListener(void) {};
    virtual ~GameStateListener(void) {};
    virtual void ManageGameState(Ogre::String state_name,GameState*state)=0;
    virtual GameState *findByName(Ogre::String state_name)=0;
    virtual void changeGameState(GameState *state)=0;
    virtual bool pushGameState(GameState* state)=0;
    virtual void popGameState()=0;
    virtual void Shutdown()=0;
 
};

class GameState: public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
public:
    static void Create(GameStateListener *parent, const Ogre::String name) {};
    void destroy(void) {delete this;}
	void init(device_info *devices) {mDevice = devices;};
    virtual void enter(void)=0;
    virtual void exit(void)=0;
    virtual bool pause(void) {return false;}
    virtual void resume(void) {};
protected:
    GameState(void) {};
    virtual ~GameState(void) {};
    GameState *findByName(Ogre::String state_name) {return parent->findByName(state_name);}
    void changeGameState(GameState *state) {parent->changeGameState(state);}
    bool pushGameState(GameState* state) {return parent->pushGameState(state);}
    void popGameState(void) {parent->popGameState();}
    void Shutdown(void) {parent->Shutdown();}
    GameStateListener *parent;
    device_info *mDevice;
};
 
/** Create the game state. Inherit, Create your class, and have it managed.
    Example:
    \code
    static void MyGameStateClass::Create(GameStateListener *parent,
            const Ogre::String name)
    {
        myGameStateClass myGameState=new myGameStateClass(); 
        myGameState->parent=parent;
        parent->ManageGameState(name,myGameState);
    }
    \endcode
*/
#define DECLARE_GAMESTATE_CLASS(T) static void Create(GameStateListener *parent,const Ogre::String name)    {                        T *myGameState=new T();                    myGameState->parent=parent;                parent->ManageGameState(name,myGameState);        }
 
#endif