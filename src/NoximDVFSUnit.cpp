/*
 * File:   NoximDVFSUnit.cpp
 * Author: nan
 *
 */
#include "NoximDVFSUnit.h"

int NoximDVFSUnit::distance(NoximDVFSUnit* dvfs1, NoximDVFSUnit* dvfs2) {
	return hammingDistance(dvfs1->getCoord(), dvfs2->getCoord());
}

void NoximDVFSUnit::initQTable() {
	cout << "init q table for " << toFullString() << endl;
	const int MAX_ID = getMaxId();

	for (int dir = 0; dir < DIRECTIONS; dir++) {
		// neighbor exists
		if (nUnit[dir]) {
			NoximDVFSUnit* y = nUnit[dir];

			for (int i = 0; i <= MAX_ID; i++) {
				if (getId() == i)
					continue;
				NoximDVFSUnit* d = NoximDVFSUnit::getDVFS(i);
				int distance = 1 + NoximDVFSUnit::distance(y, d);
				qTable[dir][i] = (float) distance;
			}
		}
	}
	cout << qTableString() << endl;
}

void NoximDVFSUnit::initQTablesForAll() {
	const int MAX_ID = getMaxId();
	for (int i = 0; i <= MAX_ID; i++) {
		NoximDVFSUnit* dvfs = NoximDVFSUnit::getDVFS(i);
		dvfs->initQTable();
	}
}

// ---------------------DVFS unit array--------------------------------
static NoximDVFSUnit* a[MAX_STATIC_DIM * MAX_STATIC_DIM];

NoximDVFSUnit* NoximDVFSUnit::getDVFS(int x, int y) {
	return getDVFS(xy2Id(x, y));
}

NoximDVFSUnit* NoximDVFSUnit::getDVFS(const NoximCoord& c) {
	return getDVFS(coord2Id(c));
}

NoximDVFSUnit* NoximDVFSUnit::getDVFS(int id) {
	return a[id];
}

void NoximDVFSUnit::setDVFS(int id, NoximDVFSUnit* dvfs) {
	//	cout << dvfs->toString() << " set at position: " << id << endl;
	a[id] = dvfs;
}
// -------END-------------DVFS unit array--------------------------------


//----------------ID, coord and toString---------------------------------
void NoximDVFSUnit::setNUnit(int dir, NoximDVFSUnit* neighbor) {
	nUnit[dir] = neighbor;
}
void NoximDVFSUnit::setId(int aId) {
	assert(aId >= 0);
	id = aId;
}

int NoximDVFSUnit::getId() const {
	assert(id >= 0);
	return id;
}

void NoximDVFSUnit::setCoord(int x, int y) {
	//	cout << "DVFS.setCoord x " << x << ", y " << y << endl;
	coord.x = x;
	coord.y = y;
}

char* NoximDVFSUnit::toString() const {
	char* ret = (char*) malloc(250 * sizeof(char));
	sprintf(ret, "DVFSUnit[%3d] at %s", id, coord.toString());
	return ret;
}

char* NoximDVFSUnit::toFullString() const {
	char* ret = (char*) malloc(250 * sizeof(char));
	sprintf(ret, "DVFSUnit[%3d] at %s", id, coord.toString());
	sprintf(ret, "%s  neighbors: \n{\n", ret);
	for (int i = 0; i < DIRECTIONS; i++) {
		if (nUnit[i])
			sprintf(ret, "%s\t%5s ==> %s\n", ret, getDirStr(i),
					nUnit[i]->toString());
	}
	sprintf(ret, "%s}", ret);
	return ret;
}

char* NoximDVFSUnit::qTableString() const {
	char* ret = (char*) malloc(100000 * sizeof(char));
	sprintf(
			ret,
			"--------------------------------------------------q table--------------------------------------------------\n");
	for (int dir = 0; dir < DIRECTIONS; dir++) {
		if (nUnit[dir] == NULL)
			continue;
		sprintf(ret, "%s%24s\t", ret, getDirStr(dir));
	}
	sprintf(ret, "%s\n", ret);
	const int MAX_ID = getMaxId();
	for (int i = 0; i <= MAX_ID; i++) {
		if (getId() == i)
			continue;
		for (int dir = 0; dir < DIRECTIONS; dir++) {
			if (nUnit[dir] == NULL)
				continue;
			NoximDVFSUnit* d = NoximDVFSUnit::getDVFS(i);
			sprintf(ret, "%s %s = %2.1f \t", ret, d->toString(), qTable[dir][i]);
		}
		sprintf(ret, "%s\n", ret);
	}
	return ret;
}
//----------------ID, coord and toString------END------------------------
