#ifndef _Gameunit_
#define _GameUnit_

class Cell;

class GameUnit
{
public:
	GameUnit() {};
	~GameUnit() {};
	Cell* getCell() {return unitCell;};
private:
	Cell* unitCell;
};

#endif