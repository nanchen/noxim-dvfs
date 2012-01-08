/*
 * NoximTile.cpp
 *
 *  Created on: Jan 2, 2012
 *      Author: nan
 */
#include "NoximTile.h"

//----------------Frequency division & power gating------------------------
void NoximTile::setDivision(unsigned int division) {
	divider->division = division;
}

void NoximTile::setOff(bool off) {
	divider->off = off;
}
//-----END--------Frequency division & power gating------------------------


//----------------ID, coord and toString------------------------------------
void NoximTile::setNTile(int dir, NoximTile* neighbor){
	nTile[dir] = neighbor;
	r->dvfs->setNUnit(dir,neighbor->r->dvfs);
}

void NoximTile::setCoord(int x, int y) {
	coord.x = x;
	coord.y = y;
	if (r)
		r->setCoord(x,y);
}

void NoximTile::setId(int aId){
	assert(aId>=0);
	id = aId;
	if(r){
		r->local_id = getId();
		r->setId(aId);
	}
}

int NoximTile::getId() const {
	assert(id>=0);
	return id;
}

char* NoximTile::toString() const {
	char* ret = (char*) malloc(50 * sizeof(char));
	sprintf(ret, "  Tile  [%3d] at %s", id, coord.toString());
	return ret;
}
//----------------ID, coord and toString------END---------------------------
