#include "Cell.h"

Cell::Cell(int iPos, int jPos, Ogre::String idName) : ipos(iPos), jpos(jPos), idname(idName),
	f(0), g(0), h(0), state(0), parent(NULL), cellEntity(NULL), unit(NULL), closed(false), showedAsAvailable(false),
	checkedForRadius(false)
{

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
	closed = false;
	checkedForRadius = false;
}

void Cell::setUnit(GameUnit *unit)
{
	//add check if there are enemy unit near this cell
	//this->unit = unit;
	Ogre::Vector3 pos = cellEntity->getParentSceneNode()->getPosition();
	unit->SetPosition(pos);
	state = 3; //tmp value
}

void Cell::removeUnitFromCell()
{
	//this->unit = NULL;
	state = 0;
}

void Cell::showCellAsAvailable(bool available)
{
	if(available)
	{
		cellEntity->setMaterialName("selected_cell");
		showedAsAvailable = true;
	}
	else
	{
		cellEntity->setMaterialName("02-Default");
		showedAsAvailable = false;
	}
}