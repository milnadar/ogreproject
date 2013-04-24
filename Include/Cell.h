#ifndef _Cell_
#define _Cell_

#include "OgreEntity.h"
#include "OgreString.h"
#include "GameUnit.h"

class Cell
{
public:
	enum CellState {EMPTY = 0, UNIT, REMOVABLE, OTHER};
	Cell();
	Cell(int iPos, int jPos, Ogre::String idName);
	//bool operator= (const Cell*);
	void setF(int value) {f = value;};
	void setG(int value) {g = value;};
	void setH(int value) {h = value;};
	void setState(CellState value) {state = value;};
	CellState getState() const {return state;};
	void setParent(Cell *parent) {this->parent = parent;};
	Cell* getParent() const {return parent;};
	void setCellEntity(Ogre::Entity *entity) {cellEntity = entity;};
	bool isWalkable();
	Ogre::Entity* getEntity() const {return cellEntity;};
	const Ogre::String& getName() const {return idname;};
	int getF() const {return f;};
	int getG() const {return g;};
	int getH() const {return h;};
	int getI() const {return ipos;};
	int getJ() const {return jpos;};
	void setClosed(bool value) {closed = value;};
	bool isClosed() const {return closed;};
	bool isCheckedForRadius() const {return checkedForRadius;};
	void setCheckedForRadius(bool checked) {checkedForRadius = checked;};
	void setUnit(GameUnit*);
	void removeUnitFromCell();
	void clear();
	void showCellAsAvailable(bool available);
	bool isShowedAsAvailable() {return showedAsAvailable;};
	~Cell();
private:
	Ogre::Entity *cellEntity;
	GameUnit *unit;
	int ipos;
	int jpos;
	int f;
	int g;
	int h;
	CellState state;
	bool closed;
	bool checkedForRadius;
	bool showedAsAvailable;
	Ogre::String idname;
	Cell* parent;
	//Unit *unit;
};

#endif