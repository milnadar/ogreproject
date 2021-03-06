#ifndef _Game_H_
#define _Game_H_

#include "GameField.h"
#include "OISMouse.h"

//class Network;
class GameHelper;

enum NetworkGameState {
	GSEcho = 0,
	GSSystemEvent,
	GSGameEvent
};

enum NetworkSystemEvent {
	SEInitialise,
	SEGetUnitID,
	SEGetRangeCount,
	SEGetMeleCount
};

enum NetworkGameEvent {
	GEEndTurn,
	GEMoveUnit,
	GEEnterVehicle,
	GELeaveVehicle,
	GERangeAttack,
	GEMeleAttack
};

enum State {
	PlayState,
	EditState,
	InitialState
};

class GameManager
{
public:
	GameManager(Ogre::SceneManager *manager, GameHelper *helper);
	~GameManager();
	enum Players {player1 = 1, player2};
	void setupScene();
	void changeGameState();
	void selectUnit(GameUnit *unit, bool showSelection = true);
	void deselectCurrentUnit();
	void deselectUnit(GameUnit *unit);
	void endTurn();
	//returns true if current unit was able to shoot target
	bool shootUnit(GameUnit* target) {};
	void performRangeAttack(GameUnit* attacker, GameUnit* target);
	bool moveUnitToCell(GameUnit*, Cell*);
	bool setUnitInVehicle(GameUnit *unit, GameUnit *vehicle){return false;};
	bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	const State& getGameState() const {return gameState;}
	const Players& getCurrentPlayer() const {return currentPlayer;}
	const Players& getActivePlayer() const {return activePlayer;}
	bool hasSelectedUnit() const {return currentUnit != 0;}
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseMoved(const OIS::MouseEvent &arg);
	Ogre::Camera* getCamera() {return mCamera;}
	bool prepareUnitToBeEjected();
	bool putUnitInVehicle(GameUnit *unit, Vehicle *vehicle);
private:
	enum MovingType {MTWalk, MTEnterVehicle, MTMeleAttack};
	bool mouseMovedInEditState(const OIS::MouseEvent &arg);
	bool mouseMovedInPlayState(const OIS::MouseEvent &arg);
	bool mousePressedInEditState(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mousePressedInPlayState(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	void setUnits(std::vector<int> ids);
	void parseData(char* data, int size);
	bool nextLocation();
	void attackScenario();
	void movingScenario(const Ogre::FrameEvent &evt);
	//return whether unit hit the target
	bool calculateRangeAttack(const UnitStats *attacker, const UnitStats *target);
	//returns units distance check result
	int calculateDistance(int distance, int modifier);
	//returns distance between two units
	int getDistance(const GameUnit* attacker, const GameUnit* target);
	//returns distance between two points
	int getDistance(const Ogre::Vector3 &position1, const Ogre::Vector3 &position2);
	void processUnitMoving(GameUnit *unit, std::vector<Cell*>& path, MovingType type = MovingType::MTWalk);
	GameField *field;
	GameUnit *currentUnit;
	//pointer to unit that will perform moving. Only one unit can be moved at a time
	GameUnit *unitToWalk;
	//pointers to units that will perform attack scenario
	GameUnit *attacker;
	GameUnit *target;
	//pointer to unit that requires to be ejected from vehicle
	GameUnit *ejectedUnit;
	//set to true only when unit need to be ejected. 
	bool needToEject;
	Vehicle *vehicleToPutUnitInside;
	//what action to perform after unit reached destination point
	//perform mele attack, enter vehicle, just stop
	MovingType currentMovingType;
	Ogre::Real distance;
	Ogre::Vector3 destination;
	Ogre::Vector3 direction;
	Ogre::Real walkSpeed;
	std::deque<Ogre::Vector3> walkList;
	State gameState;
	Players currentPlayer;
	Players activePlayer;
	Cell* finalCell;
	GameHelper *helper;
	//Network *network;
	Ogre::SceneManager *sceneManager;
	Ogre::RaySceneQuery *mRaySceneQuery;// The ray scene query pointer
	Ogre::Camera *mCamera;
};

#endif