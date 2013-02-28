#ifndef _Cell_
#define _Cell_

#include "OgreEntity.h"
#include "OgreString.h"
#include "GameUnit.h"

class Cell
{
public:
	Cell();
	Cell(int iPos, int jPos, Ogre::String idName);
	//bool operator= (const Cell*);
	void setF(int value) {f = value;};
	void setG(int value) {g = value;};
	void setH(int value) {h = value;};
	void setState(int value) {state = value;};
	void setParent(Cell *parent) {this->parent = parent;};
	Cell* getParent() const {return parent;};
	void setCellEntity(Ogre::Entity *entity) {cellEntity = entity;};
	bool isWalkable();
	const Ogre::Entity* getEntity() const {return cellEntity;};
	int getF() const {return f;};
	int getG() const {return g;};
	int getH() const {return h;};
	int getI() const {return ipos;};
	int getJ() const {return jpos;};
	void setUnit(GameUnit*) {};
	void removeUnitFromCell() {};
	~Cell();
private:
	Ogre::Entity *cellEntity;
	GameUnit *unit;
	int ipos;
	int jpos;
	int f;
	int g;
	int h;
	int state;
	Ogre::String idname;
	Cell* parent;
	//Unit *unit;
};

#endif