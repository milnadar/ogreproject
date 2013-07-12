#include "Game.h"
#include "UnitManager.h"
//#include "Network.h"
#include "GameHelper.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

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
	mCamera = sceneManager->getCamera("PlayerCam");
	mCamera->setPosition(0, 100, 0);
	mCamera->lookAt(0, 0 ,0);
	//if unit required to be ejected from a vihicle
	needToEject = false;
	currentPlayer = Players::player1;
	activePlayer = Players::player1;
}

GameManager::~GameManager()
{
	//
}

void GameManager::setupScene()
{
	mRaySceneQuery = sceneManager->createRayQuery(Ogre::Ray());
	mCamera->setPosition(-80, 80, 30);
	mCamera->lookAt(80, 0, 30);
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

void GameManager::selectUnit(GameUnit *unit, bool showSelection)
{
	if(unit != NULL && unit->playable() && unit->getOwner() == currentPlayer)
	{
		if(currentUnit != NULL)
			deselectCurrentUnit();
		currentUnit = unit;
		currentUnit->getNode()->showBoundingBox(true);
		//consoleOutput("Selected unit " + currentUnit->getUnitName());
		if(showSelection)
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

bool GameManager::moveCurrentUnitToCell(Cell* cell)
{
	if (currentUnit == 0)
		return false;
	std::vector<Cell*> path;
	std::vector<Cell*>::iterator pathItr;
	//when vehicle is selected, avoid clicking to cell that is occupied by this vehicle
	if(currentUnit->getType() == UnitType::VEHICLE)
	{
		if(field->areCellsNeighbours(currentUnit->getCell(), cell, 0))
			return false;
	}
	path = field->findPath(currentUnit->getCell(), cell, currentUnit->getType() - 1);
	if(path.size() > currentUnit->stepsLeftToMove() || path.empty())
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

bool GameManager::mouseMoved(const OIS::MouseEvent &arg)
{
	//std::cout << "mouse moved\n";
	bool result = true;
	//MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
	if(arg.state.buttonDown(OIS::MB_Right))
	{
		mCamera->yaw(Ogre::Degree(-arg.state.X.rel * .2));
		mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * .2));
	}
	if(getGameState() == State::EditState)
		result = mouseMovedInEditState(arg);
	else if(getGameState() == State::PlayState)
		result = mouseMovedInPlayState(arg);
	//mCameraMan->injectMouseMove(arg);
	return result;
}

bool GameManager::mouseMovedInEditState(const OIS::MouseEvent &arg)
{
	if(currentUnit != NULL)
	{
		MyGUI::IntPoint position = MyGUI::InputManager::getInstance().getMousePosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(position.left/float(arg.state.width),position.top/float(arg.state.height));
        mRaySceneQuery->setRay(mouseRay);
 
        Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator itr = result.begin();
 
		if (itr != result.end() && itr->movable)
		{
			Ogre::Vector3 position = itr->movable->getParentSceneNode()->getPosition();
			currentUnit->SetPosition(position);
		}
	}
	return true;
}

bool GameManager::mouseMovedInPlayState(const OIS::MouseEvent &arg)
{
	return true;
}

bool GameManager::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	//if(!loop)
	//	return false;
	//mCameraMan->injectMouseDown(arg, id);
	MyGUI::InputManager::getInstance().injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
	bool result = true;
	if(id == OIS::MB_Left)
	{
		//mLmouseDown = true;
	}
	else if(id == OIS::MB_Right)
	{
		//mRmouseDown = true;
		MyGUI::PointerManager::getInstance().setVisible(false);
		//deselectCurrentUnit();
	}
	if(getGameState() == State::EditState)
		result = mousePressedInEditState(arg, id);
	else if(getGameState() == State::PlayState)
		result = mousePressedInPlayState(arg, id);
	return result;
}

bool GameManager::mousePressedInEditState(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		MyGUI::IntPoint position = MyGUI::InputManager::getInstance().getMousePosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(position.left/float(arg.state.width),position.top/float(arg.state.height));
        mRaySceneQuery->setRay(mouseRay);
 
        Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator itr;
		for(itr = result.begin(); itr != result.end(); itr++)
		{
			if(currentUnit != NULL)
			{
				if (itr->movable && itr->movable->getName().find("cell") == 0)
				{
					Cell* cell = Ogre::any_cast<Cell*>(itr->movable->getUserAny());
					if(field->setUnitOnCell(cell, currentUnit))
						deselectCurrentUnit();
					break;
				}
			}
			else
			{
				if (itr->movable && itr->movable->getName().find("trooper") == 0)
				{
					selectUnit(Ogre::any_cast<Trooper*>(itr->movable->getUserAny()), false);
					break;
				}
			}
		}
	}
	else if(id == OIS::MB_Right)
	{
		//
	}
	return true;
}

bool GameManager::mousePressedInPlayState(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	//if(mLmouseDown)
	if(arg.state.buttonDown(OIS::MB_Left))
	{
		MyGUI::IntPoint position = MyGUI::InputManager::getInstance().getMousePosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(position.left/float(arg.state.width),position.top/float(arg.state.height));
        mRaySceneQuery->setRay(mouseRay);
		mRaySceneQuery->setSortByDistance(true);
 
        Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator itr;
		for(itr = result.begin(); itr != result.end(); itr++)
		{
			if(getCurrentPlayer() != getActivePlayer())
				break;
			if(currentUnit != NULL && currentUnit->playable())	
			//if(hasSelectedUnit())
			{
				//if unit has to be ejected from vehicle
				if(needToEject && currentUnit->getType() == UnitType::VEHICLE)
				{
					if (itr->movable && itr->movable->getName().find("cell") == 0)
					{
						//if mouse was clicked in cell which is available to eject
						Cell *cell = Ogre::any_cast<Cell*>(itr->movable->getUserAny());
						if(cell->isShowedAsAvailable())
						{
							needToEject = false;
							Vehicle *vehicle = static_cast<Vehicle*>(currentUnit);
							GameUnit *unit = vehicle->ejectPilot();
							unit->setVisible(true);
							field->showAvailableCellsToEject(currentUnit, false);
							field->setUnitOnCell(cell, unit);
							selectUnit(unit);
						}
					}
					return true;
				}
				if(itr->movable && itr->movable->getName().find("trooper") == 0)
				{
					GameUnit* pointedUnit = Ogre::any_cast<Trooper*>(itr->movable->getUserAny());
					if(pointedUnit->getOwner() != getCurrentPlayer())
					{
						if(currentUnit->canShoot())
						{
							GameUnit *targetUnit = Ogre::any_cast<Trooper*>(itr->movable->getUserAny());
							performRangeAttack(currentUnit, targetUnit);
						}
					}
					else
					{
						//deselectCurrentUnit();
						//currentUnit = pointedUnit;
						selectUnit(pointedUnit);
					}
					break;
				}
				else if(itr->movable && itr->movable->getName().find("vehicle") == 0)
				{
					Vehicle *pointedVehicle = Ogre::any_cast<Vehicle*>(itr->movable->getUserAny());
					if(pointedVehicle->getOwner() == 0)
					{
						GameUnit *unit = currentUnit;
						putUnitInVehicle(unit, pointedVehicle);
						//if(field->setUnitInVehicle(unit, pointedVehicle))
						//{
							//if driver was set in vehicle remove it's figure from field
						//	deselectCurrentUnit();
						//	field->removeUnitFromCell(unit);
						//	selectUnit(pointedVehicle);
						//}
					}
					else if(pointedVehicle->getOwner() == getCurrentPlayer())
						selectUnit(pointedVehicle);
					break;
				}
				else if (itr->movable && itr->movable->getName().find("cell") == 0)
				{
					Cell* cell = Ogre::any_cast<Cell*>(itr->movable->getUserAny());
					bool result = false;
					if(hasSelectedUnit())
						result = moveCurrentUnitToCell(cell);
					//result = game->moveUnitToCell(currentUnit, cell);
					if(result)
					{
					/*	char data[5];
						data[0] = (char)NetworkGameState::GSGameEvent;
						data[1] = (char)NetworkGameEvent::GEMoveUnit;
						data[2] = (char)currentUnit->getID();
						data[3] = (char)cell->getI();
						data[4] = (char)cell->getJ();
						std::cout << "sending packet\n" << "GSGameEvent\n" << "GEMoveUnit\n";
						std::cout << "Unit ID = " << (int)data[2] << '\n' << " to position " << (int)data[3] << ' ' << (int)data[4] << '\n';
						if(isServer)
							network->sendDataToClient(data, sizeof(data));
						else
							network->sendDataToServer(data, sizeof(data));*/
					}
					break;
				}
			}
			else
			{
				if (itr->movable && itr->movable->getName().find("trooper") == 0)
				{
					selectUnit(Ogre::any_cast<Trooper*>(itr->movable->getUserAny()));
					break;
				}
				else if(itr->movable && itr->movable->getName().find("vehicle") == 0)
				{
					selectUnit(Ogre::any_cast<Vehicle*>(itr->movable->getUserAny()));
					break;
				}
			}
		}
	}
	//else if(mRmouseDown)
	else if(arg.state.buttonDown(OIS::MB_Right))
	{
		if(0)//needToEject)
		{
			//rplace with GameManager::ShowAvailableToEject() implementation
			//needToEject = false;
			//field->showAvailableCellsToEject(currentUnit, false);
			//field->showAvailableCellsToMove(currentUnit, true);
		}
		else
		{
			deselectCurrentUnit();
		}
	}
	return true;
}

bool GameManager::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	//mCameraMan->injectMouseUp(arg, id);
	//MyGUI::InputManager::getInstance().injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
	//	if(id == OIS::MB_Left)
	//	mLmouseDown = false;
	//else if(id == OIS::MB_Right)
	//{
	//	mRmouseDown = false;
	//	MyGUI::PointerManager::getInstance().setVisible(true);
	//}
	return true;
}

bool GameManager::prepareUnitToBeEjected()
{
	if(currentUnit != NULL && currentUnit->getType() == UnitType::VEHICLE)
	{
		Vehicle *vehicle = static_cast<Vehicle*>(currentUnit);
		if(vehicle->canEject())
		{
			needToEject = true;
			field->showAvailableCellsToMove(currentUnit, false);
			field->showAvailableCellsToEject(currentUnit, true);
		}
	}
	return true;
}

bool GameManager::putUnitInVehicle(GameUnit *unit, Vehicle *vehicle)
{
	if(!vehicle->occupied())
	{
		//since vehicle occupies 7 cells, we have to ignore all of them to find path to vehicle
		//after findPath method is called, all cells are restored to theirs previous states, so 
		//those cells will be occupied again
		field->ignoreOccupiedInRadius(vehicle->getCell(), 1, true, true);
		std::vector<Cell*> path = field->findPath(unit->getCell(), vehicle->getCell(), 0);
		//std::vector<Cell*> path = field->findPath(vehicle->getCell(), unit->getCell(), 1);
		if(path.size() == 0)
		{
			unit->setVisible(false);
			vehicle->setVisible(false);
			return false;
		}
		else
		{
			if(path.size() - 2 > unit->stepsLeftToMove() || path.empty())
				return false;
			finalCell = path[2];
			field->showAvailableCellsToMove(unit, false);
			field->removeUnitFromCell(unit);
			for(int i = 2; i < path.size(); i++)
				{walkList.push_front(path[i]->getEntity()->getParentSceneNode()->getPosition());
			}
		}
	}
	else
		return false;
}