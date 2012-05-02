/*
 * File:   NoximDVFSUnit.cpp
 * Author: nan
 *
 */
#include "NoximDVFSUnit.h"
#include <math.h>

int compareValues(double val1, double val2) {
	return (int) (val1 - val2);
}

/**
 * Determinstic routing function selecting the direction with min Q value
 */
int NoximDVFSUnit::routingQ(const int dstId) {
	if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
		cout << toString() << ", routingQ to: " << dstId << endl;

	// local
	if (dstId == getId()) {
		if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
			cout << toString() << "::routingQ dstId: " << dstId
					<< " = local ID, return DIRECTION_LOCAL" << endl;
		return DIRECTION_LOCAL;
	}

	// try to route to a neighbor
	for (int i = 0; i < DIRECTIONS; i++) {
		if (nUnit[i] == NULL)
			continue;
		//TODO should return dir if dst is a neighbor?
		if (nUnit[i]->getId() == dstId) {
			return i;
		}
	}
	int dir = getDirWithMinQValue(dstId);
	return dir;
}

vector<int> NoximDVFSUnit::scheduleRoutingPath(const int dstId) {
	vector<int> ret;
	int dir;
	NoximDVFSUnit* routingUnit = this;
	while (true) {
		if (routingUnit->getId() == dstId)
			break;
		dir = routingUnit->routingQ(dstId);
		ret.push_back(dir);
		routingUnit = routingUnit->nUnit[dir];
	}
	return ret;
}

int NoximDVFSUnit::getDirWithMinQValue(int dstId) {
	if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
		cout << toString() << "::getDirWithMinQValue dstId = " << dstId
				<< "\n\tq table = \n" << qTableString() << endl;
	double min = Q_INFINITY;
	int ret = -1;
	if (dstId == getId()) {
		if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
			cout << toString() << "::getDirWithMinQValue dstId: " << dstId
					<< " = local ID, return -1" << endl;
		return ret;
	}
	// search for min value
	for (int dir = 0; dir < DIRECTIONS; dir++) {
		if (nUnit[dir] == NULL)
			continue;

		//		//TODO should return dir if dst is a neighbor?
		//		if (nUnit[dir]->getId() == dstId) {
		//			return dir;
		//		}

		double qValue = getQValue(dstId, dir);
		//		if (compareValues(qValue, min) < 0) {
		if (qValue < min) {
			min = qValue;
			ret = dir;
		}
	}
	return ret;
}

double NoximDVFSUnit::getQValue(int dstId, int yDir) {
	if (yDir == DIRECTION_LOCAL)
		return 0.0;
	if (dstId == getId())
		return 0.0;
	return qTable[yDir][dstId];
}

void NoximDVFSUnit::setQValue(int dstId, int yDir, double qValue) {
	if (qValue > Q_INFINITY)
		qValue = Q_INFINITY;
	qTable[yDir][dstId] = qValue;
}

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
			// Queuex + d(x,y) + d(y,d)
			int distance = 1 + 1 + NoximDVFSUnit::distance(y, d);
			setQValue(i, nDir, (double) distance);
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
		for (int i = 0; i <= MAX_ID; i++) {
			//TODO should avoid the case when getId() == y.id?
			if (getId() == i)
				continue;
			setQValue(i, nDir, qValue);
		}
	}
}

void NoximDVFSUnit::setQTableForANeighborOnFreqScaling(int nDir,
		double queueTimeY, unsigned int newDivision, unsigned int prevDivision) {
	const int MAX_ID = getMaxId();
	// neighbor exists
	if (nUnit[nDir]) {
		for (int i = 0; i <= MAX_ID; i++) {
			//TODO should avoid the case when getId() == y.id?
			if (getId() == i)
				continue;
			double preValue = getQValue(i, nDir);
			double qValue = preValue + queueTimeY
					* (newDivision - prevDivision);
			setQValue(i, nDir, qValue);
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
				n-> setQTableForANeighbor(getOppositDir(dir), Q_INFINITY);
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

void NoximDVFSUnit::notifyNeighborWithRegularFlitDelivery(int neighborDir,
		int dstId) {
	//Q value through neighbor with minimal q value: ty = Qy(d, minQy(d))

	//	//TODO this logic should be in sendeing router
	//	const int routedOutputPort = routingQ(dstId);
	//	int dirOfMinQy = getDirWithMinQValue(dstId);
	//	if (dirOfMinQy != routedOutputPort) {
	//		if (NoximGlobalParams::verbose_mode > VERBOSE_OFF)
	//			cout << toString()
	//					<< ": notify neighbor: dirOfMinQy != outputPort -> there was a special case in routing, return! "
	//					<< endl;
	//		return;
	//	}

	int dirOfMinQy = getDirWithMinQValue(dstId);
	double ty = getQValue(dstId, dirOfMinQy);

	if (ty == Q_INFINITY) {
		if (NoximGlobalParams::verbose_mode > VERBOSE_OFF)
			cout << toString() << ": notify neighbor: ty is infinity return! "
					<< endl;
		return;
	}
	NoximDVFSUnit* n = nUnit[neighborDir];
	if (n) {
		if (NoximGlobalParams::verbose_mode > VERBOSE_OFF)
			cout << toString() << ": notify neighbor: " << getDirStr(
					neighborDir) << ", for regular delivery"
					<< ", dirOfMinQy = " << getDirStr(dirOfMinQy) << ", ty = "
					<< ty << endl;
		n->setQTableForRegularFlitDelivery(getOppositDir(neighborDir), dstId,
				ty);
	}
}

void NoximDVFSUnit::setQTableForRegularFlitDelivery(int nDir, int dstId,
		double ty) {

	// check special case
	const int routedOutputPort = routingQ(dstId);
	int dirOfMinQy = getDirWithMinQValue(dstId);
	if (dirOfMinQy != routedOutputPort) {
		if (NoximGlobalParams::verbose_mode > VERBOSE_OFF)
			cout << toString()
					<< ": set q table for regular delivery: dirOfMinQy != outputPort -> there was a special case in routing, return! "
					<< endl;
		return;
	}

	// neighbor exists
	if (nUnit[nDir]) {

		// Q'x(d,y) = Qx(d,y) + deltaQx(d,y)
		// deltaQx(d,y) = eta * (qx + 1 + ty - Qx(d,y))
		const double currentQValue = getQValue(dstId, nDir);
		// not update infinity value
		if (currentQValue >= Q_INFINITY) {
			if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
				cout << toString()
						<< ": setQTableForRegularFlitDelivery current q vlaue > infinity, return!"
						<< endl;
			}
			return;
		}
		const double ETA = 0.5;
		const double delta = ETA * (queueTime + 1 + ty - currentQValue);
		const double newQValue = currentQValue + delta;

		// set q table
		if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
			cout << qTableString()
					<< "\n \t\t\t\t\t|\n\t\t\t\t\t|\n\t\t\t\t\tV\n" << endl;
		setQValue(dstId, nDir, newQValue);
		if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
			cout << qTableString() << endl;

		// print calculation
		if (NoximGlobalParams::verbose_mode > VERBOSE_LOW)
			cout << toString()
					<< "\n\tQ'x(d,y) = Qx(d,y) + DeltaQx(d,y) where Qx(d,y) = Qx("
					<< dstId << ", " << getDirStr(nDir) << ") = "
					<< currentQValue
					<< "\n\t  Delta = ETA * (qx + 1 + ty - Qx(d,y)\n\t\t= "
					<< ETA << " * (" << queueTime << " + 1 + " << ty << " - "
					<< currentQValue << ")\n\t\t= " << delta
					<< "\n\t  Q'x(d,y) = " << newQValue << endl;
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
	char* ret = (char*) malloc(100 * sizeof(char));
	sprintf(ret, "%s DVFSUnit[%3d] at %s", currentTimeStr(), id,
			coord.toString());
	return ret;
}

char* NoximDVFSUnit::toFullString() const {
	char* ret = (char*) malloc(250 * sizeof(char));
	sprintf(ret, "%s  neighbors: \n{\n", toString());
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
			double qValue = getQValue(i, dir);
			if (qValue == Q_INFINITY)
				sprintf(ret, "%s %s = %s \t", ret, d->toString(), "INF");
			else
				sprintf(ret, "%s %s = %3.3f \t", ret, d->toString(),
						getQValue(i, dir));
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
			if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
				cout << "@" << sc_time_stamp()
						<< " :: duty cycle! divisionCount = " << divisionCount
						<< ", division = " << division << endl;
		return true;
	}
	return false;
}

void NoximDVFSUnit::incrementDivisionCounter() {
	if (divisionCount != 0 && divisionCount % 1000 == 0)
		cout << "NoximDVFSUnit.increment division counter, divisionCount = "
				<< divisionCount << endl;
	if (reset.read())
		divisionCount = 0;
	else
		divisionCount++;
}

void NoximDVFSUnit::checkDVFSActions() {
	const int relativeTime = floor(currentTime())
			- NoximGlobalParams::stats_warm_up_time;
	//	cout << "NoximDVFSUnit.checkDVFSActions division counter, currentTime() = " << floor(currentTime()) << endl;
	if (!reset.read()) {
		for (unsigned int i = 0; i < actions.size(); i++) {
			DVFSAction action = actions.at(i);
			if (action.timeStamp == relativeTime) {
				//				 cout << "actions.size() before executing: " << actions.size() << endl;
				executeAction(action);
				actions.erase(actions.begin() + i);
				//				 cout << "actions.size() after executing: " << actions.size() << endl;
			}
		}
	}
}

void NoximDVFSUnit::executeAction(DVFSAction action) {
	cout << currentTimeStr() << ", Executing action: " << action.toString()
			<< endl;
	string actionStr = action.action;
	// skip execution when off except the action is "on"
	if (off && actionStr.compare("on") != 0)
		return;

	if (actionStr.compare("off") == 0)
		setOff(true);
	else if (actionStr.compare("on") == 0)
		setOff(false);
	else if (actionStr.compare("div") == 0)
		setDivision(action.division);
	else
		assert(false);
}

void NoximDVFSUnit::setDivision(unsigned int division) {
	this->preDivision = this->division;
	this->division = division;
	this->notifyAllNeighbors(Q_NOTIFY_FREQ_SCALING);
}

void NoximDVFSUnit::setOff(bool off) {
	//TODO should power resuming reset division to "1" ?
	// turn off
	if (this->off == false && off == true) {
		this->off = off;
		this->notifyAllNeighbors(Q_NOTIFY_INFINITY);
		offSignal.write(1);
		// turn on
	} else if (this->off == true && off == false) {
		this->off = off;
		this->notifyAllNeighbors(Q_NOTIFY_INIT);
		this->division = 1; // reset division to initial value
		offSignal.write(0);
	}
}

bool NoximDVFSUnit::isNeighborOff(int dir) {
	if (DIRECTION_LOCAL == dir)
		return off;
	NoximDVFSUnit* neighbor = nUnit[dir];
	if (neighbor == NULL)
		return true;
	else
		return neighbor->off;
}
// ---------END--------- divider----------------------------------------
