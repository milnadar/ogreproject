#ifndef _Game_Field_
#define _Game_Field_

#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include "Cell.h"
#include <vector>
#include <list>

class GameField
{
public:
	GameField(Ogre::SceneManager *mgr);
	~GameField();
	void setupField();
	void setUnitOnCell();
	void removeUnitFromCell();
	std::vector<Cell*> findPath(const Cell*, const Cell*);
	void clearMap();
	Cell* getCellByIndex(int , int);
	bool setUnitOnCell(int, int, GameUnit*);
	bool setUnitOnCell(Cell*, GameUnit*);
	//bool setUnitOnCell(const Cell*, GameUnit*); ---?
	void showavailableCellsToMove(GameUnit *unit, bool show);
	void setAvailableCellsInRadius(Cell *cell, int radius, bool available);
private:
	bool cellsInRadiusAreWalkable(const Cell* parent, int radius);
	bool validateIndexes(int, int);
	Cell *lastCell;
	int lastRadius;
	void retrievePath(Cell*);
	bool listContains(std::list<Cell*>&, Cell*);
	int heuristic (Cell*, Cell*);
	Cell* getLowestF(std::list<Cell*>&);
	std::vector<std::vector<Cell*> >field;
	std::list<Cell*>::iterator listIterator;
	std::vector<Cell*> retrievedPath;
	Ogre::SceneManager *sceneMgr;
	int fieldWidth;
	int fieldHeight;
};

#endif