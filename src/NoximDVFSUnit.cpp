/*
 * File:   NoximDVFSUnit.cpp
 * Author: nan
 *
 */
#include "NoximDVFSUnit.h"

int NoximDVFSUnit::distance(NoximDVFSUnit* dvfs1, NoximDVFSUnit* dvfs2) {
	return hammingDistance(dvfs1->getCoord(), dvfs2->getCoord());
}

void NoximDVFSUnit::initQTableForANeighbor(int nDir) {
	// max id
	const int MAX_ID = getMaxId();
	//	cout << "MAX_ID = " << MAX_ID << endl;
	// neighbor exists
	if (nUnit[nDir]) {
		NoximDVFSUnit* y = nUnit[nDir];

		for (int i = 0; i <= MAX_ID; i++) {
			//TODO should avoid the case when getId() == y.id?
			if (getId() == i)
				continue;
			NoximDVFSUnit* d = NoximDVFSUnit::getDVFS(i);
			int distance = 1 + NoximDVFSUnit::distance(y, d);
			qTable[nDir][i] = (double) distance;

		}
	}
}

void NoximDVFSUnit::initQTable() {
	cout << "init q table for " << toFullString() << endl;

	for (int dir = 0; dir < DIRECTIONS; dir++) {
		initQTableForANeighbor(dir);
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

void NoximDVFSUnit::setQTableForANeighbor(int nDir, double qValue) {
	const int MAX_ID = getMaxId();
	// neighbor exists
	if (nUnit[nDir]) {
		NoximDVFSUnit* y = nUnit[nDir];

		for (int i = 0; i <= MAX_ID; i++) {
			//TODO should avoid the case when getId() == y.id?
			if (getId() == i)
				continue;
			qTable[nDir][i] = qValue;

		}
	}
}

void NoximDVFSUnit::setQTableForANeighborOnFreqScaling(int nDir,
		double queueTimeY, unsigned int newDivision, unsigned int prevDivision) {
	const int MAX_ID = getMaxId();
	// neighbor exists
	if (nUnit[nDir]) {
		NoximDVFSUnit* y = nUnit[nDir];
		for (int i = 0; i <= MAX_ID; i++) {
			//TODO should avoid the case when getId() == y.id?
			if (getId() == i)
				continue;
			double preValue = qTable[nDir][i];
			double qValue = preValue + queueTimeY
					* (newDivision - prevDivision);
			qTable[nDir][i] = qValue;
		}
	}
}

void NoximDVFSUnit::notifyAllNeighbors(int event) {
	cout << toString() << ": notify all neighbors, event = " << event << endl;
	for (int dir; dir < DIRECTIONS; dir++) {
		NoximDVFSUnit* n = nUnit[dir];
		if (n) {
			cout << n->qTableString()
					<< "\n \t\t\t\t\t|\n\t\t\t\t\t|\n\t\t\t\t\tV\n" << endl;
			switch (event) {
			case Q_NOTIFY_INFINITY:
				n-> setQTableForANeighbor(getOppositDir(dir), -1.0);
				break;
			case Q_NOTIFY_INIT:
				n -> initQTableForANeighbor(getOppositDir(dir));
				break;
			case Q_NOTIFY_FREQ_SCALING:
				n-> setQTableForANeighborOnFreqScaling(getOppositDir(dir),
						queueTime, division, preDivision);
				break;
			default:
				assert(false);
			}
			cout << n->qTableString() << endl;
		}
	}
}

void NoximDVFSUnit::setQueueTime(double qTime) {
	this->queueTime = qTime;
	//	cout << "queueTime = " << this->queueTime << endl;
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
	sprintf(ret, "-----------------%s", toString());
	sprintf(ret, "%s%s", ret,
			"------------------q table--------------------------------------------------\n");
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

// --------------------- divider----------------------------------------
bool NoximDVFSUnit::isDutyCycle() {
	if (off)
		return false;
	if (division == 1)
		return true;
	if (divisionCount % division == 0) {
		if (divisionCount != 0)
			cout << "@" << sc_time_stamp()
					<< " :: duty cycle! divisionCount = " << divisionCount
					<< ", division = " << division << endl;
		return true;
	}
	return false;
}

void NoximDVFSUnit::incrementDivisionCounter() {
	//	cout << "NoximDVFSUnit.increment division counter, divisionCount = " << divisionCount << endl;
	if (reset.read())
		divisionCount = 0;
	else
		divisionCount++;
}

void NoximDVFSUnit::setDivision(unsigned int division) {
	this->preDivision = this->division;
	this->division = division;
	this->notifyAllNeighbors(Q_NOTIFY_FREQ_SCALING);
}

void NoximDVFSUnit::setOff(bool off) {
	//TODO should power resuming reset division to "1" ?
	// take effect only when switching
	if (this->off == false && off == true) {
		this->off = off;
		this->notifyAllNeighbors(Q_NOTIFY_INFINITY);
	} else if (this->off == true && off == false) {
		this->off = off;
		this->notifyAllNeighbors(Q_NOTIFY_INIT);
	}

}
// ---------END--------- divider----------------------------------------
