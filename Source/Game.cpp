#include "Game.h"
#include "UnitManager.h"
//#include "Network.h"
#include "GameHelper.h"

//bool isServer = false;

GameManager::GameManager(Ogre::SceneManager *manager, GameHelper *helper) : currentUnit(0)
{
	this->sceneManager = manager;
	//this->network = network;
	this->helper = helper;
	attacker = NULL;
	target = NULL;
	walkSpeed = 10.0f;
	direction = Ogre::Vector3::ZERO;
	gameState = State::PlayState;
}

GameManager::~GameManager()
{
	//
}

void GameManager::setupScene()
{
	UnitManager *unitManager = new UnitManager(sceneManager);
	field = new GameField(sceneManager);
	field->setupField();
	GameUnit *unit = NULL;
	for(int i = 0; i < 5; i ++)
	{
		unit = UnitManager::getSingletonPtr()->createUnit(currentPlayer, 1, helper->getID());
		field->setUnitOnCell(field->getCellByIndex(i, i + 2), unit);
	}
	unit = UnitManager::getSingletonPtr()->createUnit(currentPlayer, 2, helper->getID());
	field->setUnitOnCell(field->getCellByIndex(10, 5), unit);
	//updateUnitListForCurrentPlayer();
	//network.initialiseNetwork();
}

void GameManager::setUnits(std::vector<int> ids)
{
	if(ids.size() < 10)
		return;
	for(int i = 0; i < 5; i++)
	{
		GameUnit *unit = UnitManager::getSingletonPtr()->createUnit(Players::player1, 1, ids[i]);
		field->setUnitOnCell(field->getCellByIndex(0, i), unit);
	}
	for(int i = 0; i < 5; i++)
	{
		GameUnit *unit = UnitManager::getSingletonPtr()->createUnit(Players::player2, 1, ids[i+5]);
		field->setUnitOnCell(field->getCellByIndex(9, i), unit);
	}
}

void GameManager::changeGameState()
{
	//MyGUI::ListBox *listBox = MyGUI::Gui::getInstance().findWidget<MyGUI::ListBox>("unitList");
	if(gameState == State::PlayState)
	{
		gameState = State::EditState;
		//listBox->setVisible(true);
	}
	else
	{
		gameState = State::PlayState;
		//listBox->setVisible(false);
	}
	currentUnit = NULL;
}

bool GameManager::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	/*char buffer[256];
	int result = 0;
	if(isServer)
		result = network->getDataFromClient(buffer, sizeof(buffer));
	else
		result = network->getDataFromServer(buffer, sizeof(buffer));
	if(result > 0)
		parseData(buffer, result);*/
	UnitManager::getSingletonPtr()->addTime(evt.timeSinceLastFrame);
	attackScenario();
	if(direction == Ogre::Vector3::ZERO)
	{
		if(nextLocation())
		{
			Ogre::Vector3 src = currentUnit->getNode()->getOrientation() * Ogre::Vector3::UNIT_X;
			Ogre::Quaternion quat = src.getRotationTo(direction);
			currentUnit->getNode()->rotate(quat);
			currentUnit->startAnimation(GameUnit::AnimationList::WALK_ANIMATION, true);
		}
	}
	else
	{
		Ogre::Real move = walkSpeed * evt.timeSinceLastFrame;
		distance -= move;
		if(distance <= 0.00f)
		{
			currentUnit->SetPosition(destination);
			currentUnit->moveOneStep();
			direction = Ogre::Vector3::ZERO;
			if(!nextLocation())
			{
				currentUnit->stopAnimation();
				field->setUnitOnCell(finalCell, currentUnit);
				field->showAvailableCellsToMove(currentUnit, true);
				finalCell = NULL;
			}
			else
			{
				Ogre::Vector3 src = currentUnit->getNode()->getOrientation() * Ogre::Vector3::UNIT_X;
				if ((1.0f + src.dotProduct(direction)) < 0.0001f) 
				{
					currentUnit->getNode()->yaw(Ogre::Degree(180));
				}
				else
				{
					Ogre::Quaternion quat = src.getRotationTo(direction);
					currentUnit->getNode()->rotate(quat);
				} // else
			}
		}
		else
		{
			currentUnit->TranslateUnit(direction * move);
		}
	}
	return true;
}

void GameManager::selectUnit(GameUnit *unit)
{
	if(unit != NULL && unit->playable() && unit->getOwner() == currentPlayer)
	{
		if(currentUnit != NULL)
			deselectCurrentUnit();
		currentUnit = unit;
		currentUnit->getNode()->showBoundingBox(true);
		//consoleOutput("Selected unit " + currentUnit->getUnitName());
		field->showAvailableCellsToMove(currentUnit, true);
	}
}

void GameManager::deselectCurrentUnit()
{
	if(currentUnit != NULL && currentUnit->playable())
	{
		currentUnit->getNode()->showBoundingBox(false);
		field->showAvailableCellsToMove(currentUnit, false);
		currentUnit = NULL;
	}
}

void GameManager::endTurn()
{
	if(activePlayer == Players::player1)
		activePlayer = Players::player2;
	else
		activePlayer = Players::player1;
	//updateUnitListForCurrentPlayer();
	UnitManager::getSingletonPtr()->resetUnitsStats();
	deselectCurrentUnit();
	attacker = NULL;
	target = NULL;
	//MyGUI::Button *button = MyGUI::Gui::getInstancePtr()->findWidget<MyGUI::Button>("changePlayerButton");
	//button->setEnabled(activePlayer == currentPlayer);
}

void GameManager::parseData(char *data, int size)
{
	/*std::cout << "data received. Num bytes = " << size << '\n';
	switch(data[0]){
	case NetworkGameState::GSEcho :
		break;
	case NetworkGameState::GSSystemEvent :
		{
			//receive 10 ids
			if(data[1] == NetworkSystemEvent::SEInitialise)
			{
				if(isServer)
				{
					if(data[2] != 0)
					{
						std::cout << "Wrong\n " << (int)data[0] << ' ' << (int)data[1] << ' ' << (int)data[2] << ' ' << size << '\n';
						break;
					}
					std::cout << "Client asked for ids\n";
					//int ids[10];
					std::vector<int>ids(10);
					char dataToSend[13];
					dataToSend[0] = (char)NetworkGameState::GSSystemEvent;
					dataToSend[1] = (char)NetworkSystemEvent::SEInitialise;
					dataToSend[2] = (char)0;
					for(int i = 0; i < 10; i++)
					{
						ids[i] = helper->getID();
						std::cout << ids[i] << ' ';
						dataToSend[i+3] = (char)ids[i];
					}
					network->sendDataToClient(dataToSend, sizeof(dataToSend));
					//create units on field
					setUnits(ids);
				}
				else
				{
					if(data[2] != 0)
					{
						std::cout << "Wrong\n";
						break;
					}
					std::vector<int>ids(10);
					std::cout << "Server sent ids\n";
					for(int i = 0; i < 10; i++)
					{
						std::cout << (int)data[i+3] << ' ';
						ids[i] = (int)data[i+3];
					}
					setUnits(ids);
					//create units on field
				}
			}
			break;
		}
	case NetworkGameState::GSGameEvent :
		{
			std::cout << "GSGameEvent\n";
			if(data[1] == NetworkGameEvent::GEMoveUnit)
			{
				std::cout << "GEMoveUnit\n";
				std::cout << "Unit ID = " << (int)data[2] << '\n' << " to position " << (int)data[3] << ' ' << (int)data[4] << '\n';
				int unitID = data[2];
				Ogre::String name = "unit" + Ogre::StringConverter::toString(unitID);
				GameUnit *unit = UnitManager::getSingletonPtr()->getUnitByName(name);
				int i = data[3];
				int j = data[4];
				Cell *cell = field->getCellByIndex(i, j);
				if(cell != NULL && unit != NULL)
				{
					currentUnit = unit;
					moveUnitToCell(unit, cell);
				}
			}
			else if(data[1] == NetworkGameEvent::GEEndTurn)
			{
				endTurn();
			}
			break;
		}
	}*/
}

bool GameManager::nextLocation()
{
	if(walkList.empty())
		return false;
	destination = walkList.front();
	walkList.pop_front();
	direction = destination - currentUnit->getPosition();
	distance = direction.normalise();
	return true;
}

bool GameManager::moveUnitToCell(GameUnit *unit, Cell* cell)
{
	std::vector<Cell*> path;
	std::vector<Cell*>::iterator pathItr;
	//when vehicle is selected, avoid clicking to cell that is occupied by this vehicle
	if(unit->getType() == UnitType::VEHICLE)
	{
		if(field->areCellsNeighbours(unit->getCell(), cell, 0))
			return false;
	}
	path = field->findPath(unit->getCell(), cell, unit->getType() - 1);
	if(path.size() > unit->stepsLeftToMove() || path.empty())
		return false;
	finalCell = path.front();
	field->showAvailableCellsToMove(currentUnit, false);
	field->removeUnitFromCell(currentUnit);
	for(pathItr = path.begin(); pathItr != path.end(); pathItr++)
		{walkList.push_front((*pathItr)->getEntity()->getParentSceneNode()->getPosition());
	}
	//consoleOutput("Unit " + unit->getUnitName() + " moved to cell " + cell->getName());
	return true;
}

void GameManager::performRangeAttack(GameUnit* attacker, GameUnit *target)
{
	if(attacker != target)
	{
		//calculate distance between two units
		int distance = getDistance(attacker->getPosition(), target->getPosition());
		//consoleOutput("distance = " + Ogre::StringConverter::toString(distance));
		//calculate distance check for attacker unit
		int distanceResult = calculateDistance(attacker->getUnitStats()->attackDistance, attacker->getUnitStats()->distanceModifier);
		if(distanceResult < distance)
			return;
		this->attacker = attacker;
		this->target = target;
	}
}

bool GameManager::calculateRangeAttack(const UnitStats *attacker, const UnitStats *target)
{
	int shot = rand() % attacker->attackPower + 1;
	if(shot > target->armor)
	{
		Ogre::String log("Hit (" + Ogre::StringConverter::toString(shot) + " > " + Ogre::StringConverter::toString(target->armor) + ')');
		//consoleOutput(log);
		return true;
	}
	else
	{
		Ogre::String log("Miss (" + Ogre::StringConverter::toString(shot) + " < " + Ogre::StringConverter::toString(target->armor)+ ')');
		//consoleOutput(log);
	}
	return false;
}

int GameManager::calculateDistance(int distance, int modifier)
{
	int value = rand() % distance;
	value += modifier;
	//consoleOutput("Distance check result: " + Ogre::StringConverter::toString(value) + " + " + Ogre::StringConverter::toString(modifier) +
	//	+ " = " + Ogre::StringConverter::toString(value + modifier));
	return value;
}

int GameManager::getDistance(const GameUnit *attacker, const GameUnit *target)
{
	//Ogre::Vector3 position1 = _attacker->getPosition();
	//Ogre::Vector3 position2 = _target->getPosition();
	return 0;
}

int GameManager::getDistance(const Ogre::Vector3 &position1, const Ogre::Vector3 &position2)
{
	Ogre::Vector3 vector = position2 - position1;
	int distance = std::floor((vector.normalise() / 5) + 0.5);
	return distance;
}

void GameManager::attackScenario()
{
	if(attacker != NULL && target != NULL)
	{
		if(attacker->hasMoreShots())
		{
			if(!attacker->isBlocked())
			{
				attacker->setBlocked(true);
				attacker->makeOneShot();
				attacker->startAnimation(GameUnit::AnimationList::SHOOT_ANIMATION, false);
				if(bool result = calculateRangeAttack(attacker->getUnitStats(), target->getUnitStats()))
					target->kill();
			}
			if(attacker->getAnimationState()->hasEnded())
			{
				attacker->resetAnimation();
				attacker->setBlocked(false);
				if(!target->isAlive())
					attacker = NULL;
			}
		}
	}
	if(target != NULL && !target->isAlive())
	{
		target->startAnimation(GameUnit::AnimationList::DEATH_ANIMATION, false);
		target = NULL;
	}
}