#include "Cell.h"

Cell::Cell(int iPos, int jPos, Ogre::String idName) : ipos(iPos), jpos(jPos), idname(idName),
	f(0), g(0), h(0), state(0), parent(NULL), cellEntity(NULL), unit(NULL)
{
	//
}

bool Cell::isWalkable()
{
	if(state != 0)
		return false;
	return true;
}

void Cell::clear()
{
	f = 0;
	g = 0;
	h = 0;
	parent = NULL;
}

void Cell::setUnit(GameUnit *unit)
{
	//this->unit = unit;
	//state = 3; //tmp value
}

void Cell::removeUnitFromCell()
{
//	this->unit = NULL;
//	state = 0;
}