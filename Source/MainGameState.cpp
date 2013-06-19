#include "MainGameState.h"
#include "Network.h"

void MainGameState::Create(GameStateListener *parent, const Ogre::String& name)
{
	MainGameState *newState = new MainGameState(parent->getDeviceInfo());
	newState->parent = parent;
	parent->ManageGameState(name,newState);
}

MainGameState::MainGameState(const device_info* device) : currentUnit(0)
{
	if(device->ogre != 0)
	{
		sceneManager = device->ogre->getSceneManager("main");
		network = new Network();
		network->initialiseNetwork();
		game = new GameManager(sceneManager, &helper);
		mCamera = sceneManager->getCamera("PlayerCam");
		mCamera->setPosition(0, 100, 0);
		mCamera->lookAt(0, 0 ,0);
		mCameraMan = new OgreBites::SdkCameraMan(mCamera);		
	}
	loop = true;
	mLmouseDown = false;
	mRmouseDown = false;
	mRotateSpeed = .2;
}

MainGameState::~MainGameState()
{
	std::cout << "destructor called\n";
	//mDevice = 0;
	sceneManager = 0;
	delete network;
	network = 0;
	delete game;
	game = 0;
	delete mCameraMan;
	mCameraMan = 0;
	//mDevice = 0;
}

void MainGameState::enter(void)
{
	createScene();
}

void MainGameState::exit(void)
{
	loop = false;
	std::cout << "loop broke!\n";
	delete game;
	game = 0;
	delete network;
	network = 0;
	sceneManager->clearScene();
}

void MainGameState::createScene(void)
{
	mRaySceneQuery = sceneManager->createRayQuery(Ogre::Ray());
	mCamera->setPosition(-80, 80, 30);
	mCamera->lookAt(80, 0, 30);
	setupGUI();
	game->setupScene();
    // create your scene here :)
}

void MainGameState::setupGUI()
{
	MyGUI::Gui *gui;
	MyGUI::OgrePlatform *platform = new MyGUI::OgrePlatform();
	platform->initialise(mDevice->rwindow, sceneManager);
	gui = new MyGUI::Gui;
	gui->initialise();
	button = gui->createWidget<MyGUI::Button>("Button", 10, 10, 300, 26, MyGUI::Align::Default, "Main", "changeGameStateButton");
	button->setCaption("Change game state");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainGameState::buttonClicked);
	button = gui->createWidget<MyGUI::Button>("Button", 10, 46, 300, 26, MyGUI::Align::Default, "Main", "createUnitButton");
	button->setCaption("Create unit");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainGameState::buttonClicked);
	button = gui->createWidget<MyGUI::Button>("Button", 10, 82, 300, 26, MyGUI::Align::Default, "Main", "changePlayerButton");
	button->setCaption("Change player");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainGameState::buttonClicked);
	button = gui->createWidget<MyGUI::Button>("Button", 500, 10, 300, 26, MyGUI::Align::Default, "Main", "createServer");
	button->setCaption("Create game");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainGameState::buttonClicked);
	button = gui->createWidget<MyGUI::Button>("Button", 500, 46, 300, 26, MyGUI::Align::Default, "Main", "connectToServer");
	button->setCaption("Connect to server");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainGameState::buttonClicked);
	button = gui->createWidget<MyGUI::Button>("Button", 10, 122, 300, 26, MyGUI::Align::Default, "Main", "ejectPilot");
	button->setCaption("Eject unit");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainGameState::buttonClicked);
	MyGUI::ListBox *list = gui->createWidget<MyGUI::ListBox>("ListBox", 10, 120, 300, 100, MyGUI::Align::Default, "Main", "unitList");
	list->eventListSelectAccept += MyGUI::newDelegate(this, &MainGameState::itemAcceptedCallback);
	list->setVisible(false);
	gameConsole = gui->createWidget<MyGUI::EditBox>("EditBox",500, 780, 500, 150, MyGUI::Align::Default, "Main", "console");
	gameConsole->setEditReadOnly(true);
	gameConsole->setEditMultiLine(true);
	gameConsole->setTextAlign(MyGUI::Align::Bottom | MyGUI::Align::Left);
}

void MainGameState::updateUnitListForCurrentPlayer()
{
	/*std::vector<Ogre::String> names = UnitManager::getSingletonPtr()->getPlayerUnits(currentPlayer);
	MyGUI::ListBox *listBox = MyGUI::Gui::getInstance().findWidget<MyGUI::ListBox>("unitList");
	if(listBox)
	{
		listBox->deleteAllItems();
		std::vector<Ogre::String>::iterator itr;
		for(itr = names.begin(); itr != names.end(); itr++)
			listBox->addItem(*itr);
	}*/
}

bool MainGameState::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	std::cout << "game loop\n";
	mCameraMan->frameRenderingQueued(evt);
	if(mDevice)
	{
		mDevice->manager->capture();
		//mDevice->keyboard->capture();
		//mDevice->mouse->capture();
	}
	if(game != 0)
		game->frameRenderingQueued(evt);
	return loop;
}

bool MainGameState::frameStarted(const Ogre::FrameEvent &evt)
{
	std::cout<< "\nFrame Started\n";
	return true;
}

bool MainGameState::frameEnded(const Ogre::FrameEvent &evt)
{
	std::cout<< "\nFrame Ended\n";
	return true;
}

bool MainGameState::keyPressed(const OIS::KeyEvent &arg )
{
	if (arg.key == OIS::KC_ESCAPE)
    {
		parent->popGameState();
    }
	return true;
}

bool MainGameState::mouseMoved(const OIS::MouseEvent &arg)
{
	std::cout << "mouse moved\n";
	bool result = true;
	MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
	if(mRmouseDown)
	{
		mCamera->yaw(Ogre::Degree(-arg.state.X.rel * mRotateSpeed));
		mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * mRotateSpeed));
	}
	if(game->getGameState() == State::EditState)
		result = mouseMovedInEditState(arg);
	else if(game->getGameState() == State::PlayState)
		result = mouseMovedInPlayState(arg);
	//mCameraMan->injectMouseMove(arg);
	return result;
}

bool MainGameState::mouseMovedInEditState(const OIS::MouseEvent &arg)
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

bool MainGameState::mouseMovedInPlayState(const OIS::MouseEvent &arg)
{
	return true;
}

bool MainGameState::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(!loop)
		return false;
	mCameraMan->injectMouseDown(arg, id);
	MyGUI::InputManager::getInstance().injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
	bool result = true;
	if(id == OIS::MB_Left)
	{
		mLmouseDown = true;
	}
	else if(id == OIS::MB_Right)
	{
		mRmouseDown = true;
		MyGUI::PointerManager::getInstance().setVisible(false);
		//deselectCurrentUnit();
	}
	if(game->getGameState() == State::EditState)
		result = mousePressedInEditState(arg, id);
	else if(game->getGameState() == State::PlayState)
		result = mousePressedInPlayState(arg, id);
	return result;
}

bool MainGameState::mousePressedInEditState(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	/*if(mLmouseDown)
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
						currentUnit = NULL;
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
			}
		}
	}
	else if(mRmouseDown)
	{
		//
	}*/
	return true;
}

bool MainGameState::mousePressedInPlayState(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	if(mLmouseDown)
	{
		MyGUI::IntPoint position = MyGUI::InputManager::getInstance().getMousePosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(position.left/float(arg.state.width),position.top/float(arg.state.height));
        mRaySceneQuery->setRay(mouseRay);
		mRaySceneQuery->setSortByDistance(true);
 
        Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator itr;
		for(itr = result.begin(); itr != result.end(); itr++)
		{
			if(game->getCurrentPlayer() != game->getActivePlayer())
				break;
			if(currentUnit != NULL && currentUnit->playable())	
			{
				//if unit has to be ejected from vehicle
				/*if(needToEject && currentUnit->getType() == UnitType::VEHICLE)
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
				}*/
				if(itr->movable && itr->movable->getName().find("trooper") == 0)
				{
					GameUnit* pointedUnit = Ogre::any_cast<Trooper*>(itr->movable->getUserAny());
					if(pointedUnit->getOwner() != game->getCurrentPlayer())
					{
						if(currentUnit->canShoot())
						{
							GameUnit *targetUnit = Ogre::any_cast<Trooper*>(itr->movable->getUserAny());
							game->performRangeAttack(currentUnit, targetUnit);
						}
					}
					else
					{
						//deselectCurrentUnit();
						//currentUnit = pointedUnit;
						game->selectUnit(pointedUnit);
					}
					break;
				}
				else if(itr->movable && itr->movable->getName().find("vehicle") == 0)
				{
					Vehicle *pointedVehicle = Ogre::any_cast<Vehicle*>(itr->movable->getUserAny());
					if(pointedVehicle->getOwner() == 0)
					{
						GameUnit *unit = currentUnit;
						//if(field->setUnitInVehicle(unit, pointedVehicle))
						if(game->setUnitInVehicle(unit, pointedVehicle))
						{
							//if driver was set in vehicle remove it's figure from field
							//deselectCurrentUnit();
							//field->removeUnitFromCell(unit);
							game->selectUnit(pointedVehicle);
							//network
						}
					}
					else if(pointedVehicle->getOwner() == game->getCurrentPlayer())
						game->selectUnit(pointedVehicle);
					break;
				}
				else if (itr->movable && itr->movable->getName().find("cell") == 0)
				{
					Cell* cell = Ogre::any_cast<Cell*>(itr->movable->getUserAny());
					bool result = false;
					result = game->moveUnitToCell(currentUnit, cell);
					if(result)
					{
						char data[5];
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
							network->sendDataToServer(data, sizeof(data));
					}
					break;
				}
			}
			else
			{
				if (itr->movable && itr->movable->getName().find("trooper") == 0)
				{
					game->selectUnit(Ogre::any_cast<Trooper*>(itr->movable->getUserAny()));
					break;
				}
				else if(itr->movable && itr->movable->getName().find("vehicle") == 0)
				{
					game->selectUnit(Ogre::any_cast<Vehicle*>(itr->movable->getUserAny()));
					break;
				}
			}
		}
	}
	else if(mRmouseDown)
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
			game->deselectCurrentUnit();
		}
	}
	return true;
}

bool MainGameState::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	mCameraMan->injectMouseUp(arg, id);
	MyGUI::InputManager::getInstance().injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
		if(id == OIS::MB_Left)
		mLmouseDown = false;
	else if(id == OIS::MB_Right)
	{
		mRmouseDown = false;
		MyGUI::PointerManager::getInstance().setVisible(true);
	}
	return true;
}

void MainGameState::buttonClicked(MyGUI::Widget* _widget)
{
	if(_widget != NULL)
	{
		if(_widget->getName() == "createUnitButton")
		{
			//if(game->getGameState() == State::EditState)
			//	currentUnit = UnitManager::getSingletonPtr()->createUnit(currentPlayer, 1, helper.getID());
		}
		else if(_widget->getName() == "changeGameStateButton")
		{
			//changeGameState();
		}
		else if(_widget->getName() == "changePlayerButton")
		{
			game->endTurn();
			char data[2];
			data[0] = (char)NetworkGameState::GSGameEvent;
			data[1] = (char)NetworkGameEvent::GEEndTurn;
			if(isServer)
				network->sendDataToClient(data, sizeof(data));
			else
				network->sendDataToServer(data, sizeof(data));
		}
		else if(_widget->getName() == "createServer")
		{
			isServer = true;
			network->createServer();
		}
		else if(_widget->getName() == "connectToServer")
		{
			isServer = false;
			bool result = network->connectToServer("25.175.166.86");
			if(result)
			{
				char data[3];
				data[0] = (char)NetworkGameState::GSSystemEvent;
				data[1] = (char)NetworkSystemEvent::SEInitialise;
				data[2] = (char)0;
				std::cout << "sending initialise " << (int)data[0] << ' ' << (int)data[1] << ' ' << (int)data[2] << '\n';
				network->sendDataToServer(data, sizeof(data));
				//currentPlayer = GameManager::Players::player2;
				game->endTurn();
			}
		else if(_widget->getName() == "ejectPilot")
		{
			if(currentUnit != NULL && currentUnit->getType() == UnitType::VEHICLE)
			{
				Vehicle *vehicle = static_cast<Vehicle*>(currentUnit);
				if(vehicle->canEject())
				{
					//needToEject = true;
					//field->showAvailableCellsToMove(currentUnit, false);
					//field->showAvailableCellsToEject(currentUnit, true);
				}
			}
		}
		}
	}
}

void MainGameState::itemAcceptedCallback(MyGUI::ListBox* _sender, size_t _index)
{
	//Ogre::String str = _sender->getItem(_index);
	//currentUnit = UnitManager::getSingletonPtr()->getUnitByName(str);
}

void MainGameState::consoleOutput(Ogre::String string)
{
	if(gameConsole)
	{
		gameConsole->addText(string + '\n');
	}
}