#include "GameField.h"

GameField::GameField(Ogre::SceneManager* mgr) : sceneMgr(mgr)
{
	std::vector<Cell*> tmp;
	for(int i = 0; i < 10; i++)
	{
		for(int j = 0; j < 10; j++)
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

void GameField::setupField()
{
	Ogre::Entity *ent;
	Ogre::SceneNode *node;
	for(int i = 0; i < 10; i++)
		for(int j = 0; j < 10; j++)
		{
			Ogre::String name = "cell" + Ogre::StringConverter::toString(i) + Ogre::StringConverter::toString(j);
			Cell *cell = new Cell(i, j, name);
			field[i][j] = cell;
			Ogre::Vector3 pos;
			if(i % 2 != 0)
				pos = Ogre::Vector3(i * 2.5, 0, (j * 3) + 1.5);
			else
				pos = Ogre::Vector3(i * 2.5, 0, j * 3);
			ent = sceneMgr->createEntity(name, "cell.mesh");
			if(i % 2 != 0 && j == 9)
			{
				ent->setVisible(false);
				cell->setState(1);
			}
			node = sceneMgr->getRootSceneNode()->createChildSceneNode(name + "node", pos);
			node->attachObject(ent);
			cell->setCellEntity(ent);
			ent->setUserAny(Ogre::Any(cell));
		}
}

bool GameField::setUnitOnCell(Cell *cell, GameUnit* unit)
{
	if(cell->isWalkable())
	{
		//clear cell unit was on
		Cell *unitCell = unit->getCell();
		if(unitCell != NULL)
			unitCell->removeUnitFromCell();
		cell->setUnit(unit);
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
			return true;
		}
	}
	return false;
}

std::vector<Cell*> GameField::findPath(const Cell* _start, const Cell* _finish)
{
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
	opened.push_back(start);
	while ((!listContains (closed, finish)) && (opened.size() != 0))
	{
		current = getLowestF(opened);
		if (current == NULL)
		{
			std::cout << "current == NULL, perhaps becaurse of opened list is emty\n";
			break;
		}//if
		opened.remove(current);
		closed.push_back(current);
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
				{child = getCellByIndex(current->getI() + shiftForXr, current->getJ() - 1); tmpG = 10; break;}
			case 1 : 
				{child = getCellByIndex(current->getI() + 1, current->getJ()); tmpG = 15; break;}
			case 2 : 
				{child = getCellByIndex(current->getI() + shiftForXr, current->getJ() + 1); tmpG = 10; break;}
			case 3 : 
				{child = getCellByIndex(current->getI() - shiftForXl,current->getJ() + 1); tmpG = 10; break;}
			case 4 : 
				{child = getCellByIndex(current->getI() - 1,current->getJ()); tmpG = 15; break;}
			case 5 : 
				{child = getCellByIndex(current->getI() - shiftForXl,current->getJ() - 1); tmpG = 10; break;}
			}//switch
			if (child == NULL)
				continue;
			if (child->isWalkable() && !listContains (closed, child))
			{
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
}

Cell* GameField::getCellByIndex(int index1, int index2)
{
	Cell *cell = NULL;
	if(index1 < 10 && index2 < 10 && index1 >= 0 && index2 >= 0)
		cell = field[index1][index2];
	return cell;
}

int GameField::heuristic(Cell* start, Cell* finish)
{
	return (abs(finish->getI() - start->getI()) + abs(finish->getJ() - start->getJ())) * 10;
}

bool GameField::validateIndexes(int indexi, int indexj)
{
	if(indexi < 10 && indexi >= 0 && indexj < 10 && indexj >=0)
		return true;
	return false;
}

void GameField::retrievePath(Cell* target)
{
	target->getEntity()->getParentSceneNode()->showBoundingBox(true);
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