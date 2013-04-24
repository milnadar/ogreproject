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
	void printField();
	void setupField();
	void setUnitOnCell();
	std::vector<Cell*> findPath(const Cell*, const Cell*, int unitSize);
	void clearMap();
	Cell* getCellByIndex(int , int) const;
	bool setUnitOnCell(int, int, GameUnit*);
	bool setUnitOnCell(Cell*, GameUnit*);
	bool removeUnitFromCell(GameUnit *unit);
	//bool setUnitOnCell(const Cell*, GameUnit*); ---?
	void showAvailableCellsToMove(const GameUnit *unit, bool show);
	void showAvailableCellsToEject(const GameUnit *unit, bool show);
	bool areCellsNeighbours(const Cell* parent, const Cell* target, int radius) const;
	bool setUnitInVehicle(GameUnit *unit, GameUnit *vehicle);
private:
	void setAvailableCellsInRadius(Cell *cell, int radius, bool available);
	bool cellsInRadiusAreWalkable(const Cell* parent, int radius);
	bool makeCellsInRadiusOccupied(const Cell* parent, int radius, bool occupied);
	void ignoreCellsInRadius(Cell* parent, int radius, bool ignore);
	bool validateIndexes(int, int);
	std::vector<Cell*> getCellsInRadius(const Cell* parent, int radius, bool includeInner = true);
	Cell *lastCell;
	int lastRadius;
	void retrievePath(Cell*);
	bool listContains(std::list<Cell*>&, Cell*);
	int heuristic (Cell*, Cell*);
	Cell* getLowestF(std::list<Cell*>&);
	std::vector<std::vector<Cell*> >field;
	std::list<Cell*>::iterator listIterator;
	std::vector<Cell*> retrievedPath;
	std::vector<Cell*> foundedCells;
	Ogre::SceneManager *sceneMgr;
	int fieldWidth;
	int fieldHeight;
};

#endif