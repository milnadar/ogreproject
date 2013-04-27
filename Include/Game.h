#ifndef _Game_H_
#define _Game_H_

#include "GameField.h"

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
	void selectUnit(GameUnit *unit);
	void deselectCurrentUnit();
	void endTurn();
	void attackScenario();
	void performRangeAttack(GameUnit* attacker, GameUnit* target);
	bool moveUnitToCell(GameUnit*, Cell*);
	bool setUnitInVehicle(GameUnit *unit, GameUnit *vehicle){return false;};
	bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	const State& getGameState() const {return gameState;}
	const Players& getCurrentPlayer() const {return currentPlayer;}
	const Players& getActivePlayer() const {return activePlayer;}
private:
	void setUnits(std::vector<int> ids);
	void parseData(char* data, int size);
	bool nextLocation();
	//return whether unit hit the target
	bool calculateRangeAttack(const UnitStats *attacker, const UnitStats *target);
	//returns units distance check result
	int calculateDistance(int distance, int modifier);
	//returns distance between two units
	int getDistance(const GameUnit* attacker, const GameUnit* target);
	//returns distance between two points
	int getDistance(const Ogre::Vector3 &position1, const Ogre::Vector3 &position2);
	GameField *field;
	GameUnit *currentUnit;
	//
	GameUnit *attacker;
	GameUnit *target;
	//
	GameUnit *ejectedUnit;
	//set to true only when unit need to be ejected. 
	bool needToEject;
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
};

#endif