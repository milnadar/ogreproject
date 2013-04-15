#include "GameField.h"
#include "OgreMeshManager.h"

GameField::GameField(Ogre::SceneManager* mgr) : sceneMgr(mgr), lastCell(NULL)
{
	fieldWidth = 20;
	fieldHeight = 20;
	std::vector<Cell*> tmp;
	for(int i = 0; i < fieldHeight; i++)
	{
		for(int j = 0; j < fieldWidth; j++)
		{
			tmp.push_back(NULL);
		}
		field.push_back(tmp);
	}
}

GameField::~GameField()
{
	for(int i = 0; i < 10; i++)
		for(int j = 0; j < 10; j++)
		{
			//
		}
}

void GameField::printField()
{
	std::cout << "\n\n";
	for(int j = fieldHeight-1; j >=0 ; j--)
	{
		for(int i = 0; i < fieldWidth; i++)
		{
			std::cout << field[j][i]->getState();
		}
		std::cout << "   ";
		for(int i = 0; i < fieldWidth; i++)
		{
			std::cout << field[j][i]->isCheckedForRadius();
		}
		std::cout << '\n';
		if(j % 2 == 0)
			std::cout << ' ';
	}
}

void GameField::setupField()
{
	Ogre::Entity *ent;
	Ogre::SceneNode *node;
	for(int i = 0; i < fieldHeight; i++)
		for(int j = 0; j < fieldWidth; j++)
		{
			Ogre::String name = "cell" + Ogre::StringConverter::toString(i) + "_" + Ogre::StringConverter::toString(j);
			Cell *cell = new Cell(i, j, name);
			field[i][j] = cell;
			Ogre::Vector3 pos;
			if(i % 2 != 0)
				pos = Ogre::Vector3(i == 0 ? i * 3 : i * 4.4, 0, (j * 5.2) + 2.6);
			else
				pos = Ogre::Vector3(i == 0 ? i * 3 : i * 4.4, 0, j * 5.2);
			ent = sceneMgr->createEntity(name, "cell.mesh");
			if(i % 2 != 0 && j == fieldHeight - 1)
			{
				ent->setVisible(false);
				cell->setState(Cell::CellState::OTHER);
			}
			node = sceneMgr->getRootSceneNode()->createChildSceneNode(name + "node", pos);
			node->attachObject(ent);
			node->setScale(2,2,2);
			cell->setCellEntity(ent);
			ent->setUserAny(Ogre::Any(cell));
		}
		/*Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
		Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, fieldHeight * 5, fieldWidth * 6, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
		Ogre::Entity* entGround = sceneMgr->createEntity("GroundEntity", "ground");
		node = sceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(entGround);
		entGround->setMaterialName("field");
		entGround->setCastShadows(false);
		node->setPosition(fieldHeight, -1, fieldWidth);*/
}

bool GameField::setUnitOnCell(Cell *cell, GameUnit* unit)
{
	if(cell != NULL && cell->isWalkable() && unit!= NULL)
	{
		printField();
		//clear cell unit was on
		Cell *unitCell = unit->getCell();
		if(unitCell != NULL)
		{
			unitCell->removeUnitFromCell();
			if(unit->getType() == UnitType::VEHICLE)
				makeCellsInRadiusOccupied(unitCell, 1, false);
		}
		cell->setUnit(unit);
		unit->setUnitCell(cell);
		if(unit->getType() == UnitType::VEHICLE)
			makeCellsInRadiusOccupied(cell, 1, true);
		printField();
		return true;
	}
	return false;
}

bool GameField::setUnitOnCell(int indexi, int indexj, GameUnit* unit)
{
	//if indexes are valid
	if(validateIndexes(indexi, indexj))
	{
		//check if cell is clear
		Cell* fieldCell = field[indexi][indexj];
		if(fieldCell->isWalkable())
		{
			//clear cell unit was on
			Cell *unitCell = unit->getCell();
			if(unitCell != NULL)
				unitCell->removeUnitFromCell();
			fieldCell->setUnit(unit);
			fieldCell->setUnit(unit);
			return true;
		}
	}
	return false;
}

bool GameField::removeUnitFromCell(GameUnit *unit)
{
	Cell *cell = unit->getCell();
	if(cell != NULL)
	{
		cell->removeUnitFromCell();
		if(unit->getType() == UnitType::VEHICLE)
			makeCellsInRadiusOccupied(unit->getCell(), 1, false);
		unit->setUnitCell(NULL);
		return true;
	}
	return false;
}

void GameField::showavailableCellsToMove(GameUnit *unit, bool show)
{
	if(unit != NULL && unit->playable())
	{
		Cell *unitCell = unit->getCell();
		setAvailableCellsInRadius(unitCell, unit->stepsLeftToMove(), show);
	}
}

void GameField::setAvailableCellsInRadius(Cell *cell, int radius, bool available)
{
	//if cell is in required radius to paint
	if(--radius >= 0)
		if(cell != NULL)
		{
			Cell* neighbour = NULL;
			int shiftForXr;
			int shiftForXl;
			//get all 6 neighbours of the cell
			for(int i = 0; i < 6; i ++)
			{
				if (cell->getI() % 2 == 0)
				{
					shiftForXr = 0;
					shiftForXl = 1;
				}
				else
				{
					shiftForXr = 1;
					shiftForXl = 0;
				}
				switch(i)
				{
				case 0 : {neighbour = getCellByIndex(cell->getI() + 1, cell->getJ() - shiftForXl); break;};
				case 1 : {neighbour = getCellByIndex(cell->getI() + 1, cell->getJ() + shiftForXr); break;};
				case 2 : {neighbour = getCellByIndex(cell->getI(), cell->getJ() + 1); break;};
				case 3 : {neighbour = getCellByIndex(cell->getI() - 1, cell->getJ() + shiftForXr); break;};
				case 4 : {neighbour = getCellByIndex(cell->getI() - 1, cell->getJ() - shiftForXl); break;};
				case 5 : {neighbour = getCellByIndex(cell->getI(), cell->getJ() - 1); break;};
				}
				if(neighbour != NULL)
				{
					//if cell wasn't yet painted as walkable/!walkable, and cell cen be moved to
					if(neighbour->isShowedAsAvailable() != available && neighbour->isWalkable())
						neighbour->showCellAsAvailable(available);
					//for every neighbour of the cell, check if it is in radius, and paint if needed
					setAvailableCellsInRadius(neighbour, radius, available);
				}
			}
		}
}

bool GameField::areCellsNeighbours(const Cell* parent, const Cell* target, int radius) const
{
	Cell* neighbour = NULL;
	int shiftForXr;
	int shiftForXl;
	if (parent->getI() % 2 == 0)
	{
		shiftForXr = 0;
		shiftForXl = 1;
	}
	else
	{
		shiftForXr = 1;
		shiftForXl = 0;
	}
	//get all 6 neighbours of the cell
	for(int i = 0; i < 6; i ++)
	{
		switch(i)
		{
		case 0 : {neighbour = getCellByIndex(parent->getI() + 1, parent->getJ() - shiftForXl); break;};
		case 1 : {neighbour = getCellByIndex(parent->getI() + 1, parent->getJ() + shiftForXr); break;};
		case 2 : {neighbour = getCellByIndex(parent->getI(), parent->getJ() + 1); break;};
		case 3 : {neighbour = getCellByIndex(parent->getI() - 1, parent->getJ() + shiftForXr); break;};
		case 4 : {neighbour = getCellByIndex(parent->getI() - 1, parent->getJ() - shiftForXl); break;};
		case 5 : {neighbour = getCellByIndex(parent->getI(), parent->getJ() - 1); break;};
		}
		if(neighbour == target)
			return true;
	}
	return false;
}

bool GameField::setUnitInVehicle(GameUnit *unit, GameUnit *vehicle)
{
	if(static_cast<Vehicle*>(vehicle)->setUnitIn(unit))
	{
		unit->setVisible(false);
		return true;
	}
	return false;
}

bool GameField::makeCellsInRadiusOccupied(const Cell* parent, int radius, bool occupied)
{
	Cell* neighbour = NULL;
	int shiftForXr;
	int shiftForXl;
	if (parent->getI() % 2 == 0)
	{
		shiftForXr = 0;
		shiftForXl = 1;
	}
	else
	{
		shiftForXr = 1;
		shiftForXl = 0;
	}
	//get all 6 neighbours of the cell
	for(int i = 0; i < 6; i ++)
	{
		switch(i)
		{
		case 0 : {neighbour = getCellByIndex(parent->getI() + 1, parent->getJ() - shiftForXl); break;};
		case 1 : {neighbour = getCellByIndex(parent->getI() + 1, parent->getJ() + shiftForXr); break;};
		case 2 : {neighbour = getCellByIndex(parent->getI(), parent->getJ() + 1); break;};
		case 3 : {neighbour = getCellByIndex(parent->getI() - 1, parent->getJ() + shiftForXr); break;};
		case 4 : {neighbour = getCellByIndex(parent->getI() - 1, parent->getJ() - shiftForXl); break;};
		case 5 : {neighbour = getCellByIndex(parent->getI(), parent->getJ() - 1); break;};
		}
		if(neighbour != NULL)
			neighbour->setState(occupied ? Cell::CellState::UNIT : Cell::CellState::EMPTY);
	}
	return true;
}


void GameField::ignoreCellsInRadius(Cell* parent, int radius, bool ignore)
{
	Cell* neighbour = NULL;
	int shiftForXr;
	int shiftForXl;
	if (parent->getI() % 2 == 0)
	{
		shiftForXr = 0;
		shiftForXl = 1;
	}
	else
	{
		shiftForXr = 1;
		shiftForXl = 0;
	}
	//get all 6 neighbours of the cell
	for(int i = 0; i < 6; i ++)
	{
		switch(i)
		{
		case 0 : {neighbour = getCellByIndex(parent->getI() + 1, parent->getJ() - shiftForXl); break;};
		case 1 : {neighbour = getCellByIndex(parent->getI() + 1, parent->getJ() + shiftForXr); break;};
		case 2 : {neighbour = getCellByIndex(parent->getI(), parent->getJ() + 1); break;};
		case 3 : {neighbour = getCellByIndex(parent->getI() - 1, parent->getJ() + shiftForXr); break;};
		case 4 : {neighbour = getCellByIndex(parent->getI() - 1, parent->getJ() - shiftForXl); break;};
		case 5 : {neighbour = getCellByIndex(parent->getI(), parent->getJ() - 1); break;};
		}
		if(neighbour != NULL)
			neighbour->setCheckedForRadius(ignore);
	}
}

std::vector<Cell*> GameField::getCellsInRadius(const Cell* parent, int radius, bool includeInner)
{
	return foundedCells;
	//
}

std::vector<Cell*> GameField::findPath(const Cell* _start, const Cell* _finish, int unitSize = 0)
{
	bool ignoreLast = false;
	bool fin = false;
	std::list<Cell*> opened;
	std::list<Cell*> closed;
	Cell *current = NULL;
	Cell *child = NULL;
	Cell *start = NULL;
	Cell *finish = NULL;
	int tmpG;
	int tmpH;
	int shiftForXr;
	int shiftForXl;
	std::vector<Cell*> tmpPath;
	start = field[_start->getI()][_start->getJ()];
	finish = field[_finish->getI()][_finish->getJ()];
	if (start == NULL || finish == NULL)
	{
		std::cout << "One of points is out of range\n";
		return tmpPath;
	}
	if(unitSize > 0)
		ignoreCellsInRadius(start, 0, true);
	opened.push_back(start);
	//while ((!listContains (closed, finish)) && (opened.size() != 0))
	while ((!fin) && (opened.size() != 0))
	{
		current = getLowestF(opened);
		if (current == NULL)
		{
			std::cout << "current == NULL, perhaps becaurse of opened list is emty\n";
			break;
		}//if
		opened.remove(current);
		if(current == finish)
		{
			fin = true;
			if(unitSize > 0)
			{
				finish = current;
				continue;
			}
		}
		closed.push_back(current);
		current->setClosed(true);
		for (int index = 0; index < 6; index++)
		{
			if (current->getI() % 2 == 0)
			{
				shiftForXr = 0;
				shiftForXl = 1;
			}
			else
			{
				shiftForXr = 1;
				shiftForXl = 0;
			}
			switch(index)
			{
			case 0 : 
				{child = getCellByIndex(current->getI() + 1, current->getJ() - shiftForXl); tmpG = 10; break;}
			case 1 : 
				{child = getCellByIndex(current->getI() + 1, current->getJ() + shiftForXr); tmpG = 15; break;}
			case 2 : 
				{child = getCellByIndex(current->getI(), current->getJ() + 1); tmpG = 10; break;}
			case 3 : 
				{child = getCellByIndex(current->getI() - 1, current->getJ() + shiftForXr); tmpG = 10; break;}
			case 4 : 
				{child = getCellByIndex(current->getI() - 1, current->getJ() - shiftForXl); tmpG = 15; break;}
			case 5 : 
				{child = getCellByIndex(current->getI(), current->getJ() - 1); tmpG = 10; break;}
			}//switch
			if (child == NULL)
				continue;
			if(child == finish && unitSize > 0)
			{
				ignoreLast = true;
				fin = true;
				finish = child;
			}
			current->setCheckedForRadius(true);
			//check if cell is walkable and all cells around it are also walkable so vehicle could fit
			if (child->isWalkable() && !child->isClosed() && (cellsInRadiusAreWalkable(child, unitSize) || ignoreLast))
			{
				current->setCheckedForRadius(false);
				if (!listContains(opened, child))
				{
					opened.push_back(child);
					tmpH = heuristic(child, finish);
					child->setH (tmpH);
					child->setG (current->getG() + tmpG);
					child->setF (child->getG() + child->getH());
					child->setParent(current);
				}
				else
				{
					if (child->getG() > (current->getG() + tmpG))
					{
						child->setParent(current);
						child->setG(current->getG() + tmpG);
					}
				}
			}//if			
		}//for
	}//while	
	if (finish->getParent() == NULL)
		std::cout << "Path was not found\n";
	else
	{
		if(ignoreLast)
			retrievePath(finish->getParent());
		else
			retrievePath(finish);
		tmpPath = retrievedPath;
		tmpPath.pop_back();
	}
	clearMap();
	return tmpPath;
}

void GameField::clearMap()
{
	retrievedPath.clear();
	for(int i = 0; i < fieldHeight; i++)
		for(int j = 0; j < fieldWidth; j++)
			field[i][j]->clear();
}

Cell* GameField::getCellByIndex(int index1, int index2) const
{
	Cell *cell = NULL;
	if(index1 < fieldHeight && index2 < fieldWidth && index1 >= 0 && index2 >= 0)
		cell = field[index1][index2];
	return cell;
}

int GameField::heuristic(Cell* start, Cell* finish)
{
	return (abs(finish->getI() - start->getI()) + abs(finish->getJ() - start->getJ())) * 10;
}

bool GameField::cellsInRadiusAreWalkable(const Cell* parent, int radius)
{
	if(radius == 0)
		return true;
	Cell* child = NULL;
	int shiftForXr;
	int shiftForXl;
 	for (int index = 0; index < 6; index++)
	{
		if (parent->getI() % 2 == 0)
		{
			shiftForXr = 0;
			shiftForXl = 1;
		}
		else
		{
			shiftForXr = 1;
			shiftForXl = 0;
		}
		switch(index)
		{
		case 0 : 
			{child = getCellByIndex(parent->getI() + 1, parent->getJ() - shiftForXl); break;}
		case 1 : 
			{child = getCellByIndex(parent->getI() + 1, parent->getJ() + shiftForXr); break;}
		case 2 : 
			{child = getCellByIndex(parent->getI(), parent->getJ() + 1); break;}
		case 3 : 
			{child = getCellByIndex(parent->getI() - 1, parent->getJ() + shiftForXr); break;}
		case 4 : 
			{child = getCellByIndex(parent->getI() - 1, parent->getJ() - shiftForXl); break;}
		case 5 : 
			{child = getCellByIndex(parent->getI(), parent->getJ() - 1); break;}
		}//switch
		if(child != NULL && child != parent)
		{
			if(!child->isWalkable())
				return false;
		}
	}
	return true;
}

bool GameField::validateIndexes(int indexi, int indexj)
{
	if(indexi < 10 && indexi >= 0 && indexj < 10 && indexj >=0)
		return true;
	return false;
}

void GameField::retrievePath(Cell* target)
{
	retrievedPath.push_back(target);
	if (target->getParent() != NULL)
		retrievePath(target->getParent());
}

bool GameField::listContains(std::list<Cell*>& list, Cell* cell)
{
	for (listIterator = list.begin(); listIterator != list.end(); listIterator++)
	{
		if ((cell->getI() == (*listIterator)->getI()) && (cell->getJ() == (*listIterator)->getJ()))
		{
			return true;
		}
	}
	return false;
}

Cell* GameField::getLowestF(std::list<Cell*>& list)
{
	Cell *cell = NULL;
	if (!list.empty())
	{
		cell = list.front();
		for (listIterator = list.begin(); listIterator != list.end(); listIterator++)
		{
			if ((*listIterator)->getF() < cell->getF())
				cell = (*listIterator);
		}
	}
	return cell;
}