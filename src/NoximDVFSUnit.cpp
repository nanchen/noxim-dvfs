/*
 * File:   NoximDVFSUnit.cpp
 * Author: nan
 *
 */
#include "NoximDVFSUnit.h"
#include <math.h>

// ====================================== Routing ==============================================
/**
 * Determinstic routing function
 */
int NoximDVFSUnit::routingQ(NoximRouteData & routeData) {
	const int dstId = routeData.dst_id;
	const int dirIn = routeData.dir_in;
	if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
		cout << toString() << ", routingQ to: " << dstId << ", dirIn = "
				<< dirIn << endl;

	// init bool values
	for (int i = 0; i < DIRECTIONS + 1; i++)
		routeData.canUpdateQTable[i] = true;

	// minQy(d)
	int dirOut = getDirWithMinQValue(dstId);

	// local
	if (dstId == getId()) {
		if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
			cout << toString() << "::routingQ dstId: " << dstId
					<< " = local ID, return DIRECTION_LOCAL" << endl;
		//		if (DIRECTION_LOCAL != dirOut)
		routeData.canUpdateQTable[dirIn] = false;
		return DIRECTION_LOCAL;
	}

	// try to route to a neighbor first
	for (int i = 0; i < DIRECTIONS; i++) {
		if (nUnit[i] == NULL)
			continue;
		if (nUnit[i]->getId() == dstId) {
			if (dirOut != i)
				routeData.canUpdateQTable[dirIn] = false;
			return i;
		}
	}

	if (dirOut == dirIn) {
		double min = Q_INFINITY;
		int ret = -1;

		// search for min value
		for (int dir = 0; dir < DIRECTIONS; dir++) {
			if (nUnit[dir] == NULL)
				continue;

			if (dir == dirIn)
				continue;

			double qValue = getQValue(dstId, dir);
			//		if (compareValues(qValue, min) < 0) {
			if (qValue < min) {
				min = qValue;
				ret = dir;
			}
		}
		routeData.canUpdateQTable[dirIn] == false;
		//		cout << toString() << ", dirIn: " << getDirStr(dirIn) << " != dirOut: "
		//				<< getDirStr(dirOut) << " reroute to " << getDirStr(ret)
		//				<< endl;
		return ret;
	}
	return dirOut;
}

vector<int> NoximDVFSUnit::nonDeterminsticRoutingQ(NoximRouteData & routeData) {
	vector<int> directions;

	const int dstId = routeData.dst_id;
	const int dirIn = routeData.dir_in;
	if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
		cout << toString() << ", nonDeterminsticRoutingQ to: " << dstId
				<< ", dirIn = " << getDirStr(dirIn) << endl;

	// init bool values
	for (int i = 0; i < DIRECTIONS + 1; i++)
		routeData.canUpdateQTable[i] = true;

	// local
	if (dstId == getId()) {
		if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
			cout << toString() << "::nonDeterminsticRoutingQ dstId: " << dstId
					<< " = local ID, return DIRECTION_LOCAL" << endl;
		routeData.canUpdateQTable[dirIn] = false;
		directions.push_back(DIRECTION_LOCAL);
		return directions;
	}

	// try to route to a neighbor first
	for (int i = 0; i < DIRECTIONS; i++) {
		if (nUnit[i] == NULL)
			continue;
		if (nUnit[i]->getId() == dstId) {
			routeData.canUpdateQTable[dirIn] = false;
			directions.push_back(i);
			return directions;
		}
	}

	// check dirIn
	vector<int> dirsWithDirIn = getDirsWithMinQValueFromDirs(dstId, -1);
	for (int i = 0; i < dirsWithDirIn.size(); i++) {
		if (dirsWithDirIn[i] == dirIn) {
			routeData.canUpdateQTable[dirIn] == false;
			break;
		}
	}

	// minQx(d)
	vector<int> dirsWithoutDirIn = getDirsWithMinQValueFromDirs(dstId, dirIn);
	return dirsWithoutDirIn;
}

/**
 * For Test
 */
vector<int> NoximDVFSUnit::scheduleRoutingPath(const int dstId) {
	NoximRouteData routeData;
	routeData.current_id = getId();
	routeData.src_id = getId();
	routeData.dst_id = dstId;
	routeData.dir_in = DIRECTION_LOCAL;

	vector<int> ret;
	int dir;
	NoximDVFSUnit* routingUnit = this;
	while (true) {
		if (routingUnit->getId() == dstId)
			break;
		dir = routingUnit->routingQ(routeData);
		ret.push_back(dir);
		routingUnit = routingUnit->nUnit[dir];
	}
	return ret;
}

vector<int> NoximDVFSUnit::getDirsWithMinQValueFromDirs(int dstId,
		int excludedDir) {
	vector<int> directions;

	double min = Q_INFINITY;

	// search for min value
	for (int dir = 0; dir < DIRECTIONS; dir++) {
		if (dir == excludedDir)
			continue;

		if (nUnit[dir] == NULL)
			continue;

		double qValue = getQValue(dstId, dir);
		if (qValue < min) {
			min = qValue;
			directions.clear();
			directions.push_back(dir);
		} else if (qValue == min)
			directions.push_back(dir);
	}
	return directions;
}

int NoximDVFSUnit::getDirWithMinQValue(int dstId) {
	if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
		cout << toString() << "::getDirWithMinQValue dstId = " << dstId
				<< "\n\tq table = \n" << qTableString() << endl;

	if (dstId == getId()) {
		if (NoximGlobalParams::verbose_mode > VERBOSE_MEDIUM)
			cout << toString() << "::getDirWithMinQValue dstId: " << dstId
					<< " = local ID, return LOCAL" << endl;
		return DIRECTION_LOCAL;
	}

	double min = Q_INFINITY;
	int ret = -1;

	// search for min value
	for (int dir = 0; dir < DIRECTIONS; dir++) {
		if (nUnit[dir] == NULL)
			continue;

		double qValue = getQValue(dstId, dir);
		if (qValue < min) {
			min = qValue;
			ret = dir;
		}
	}
	return ret;
}

// ============================== Common ==================================================
double NoximDVFSUnit::getQValue(int dstId, int yDir) {
	assert(dstId != getId());
	assert(yDir != DIRECTION_LOCAL);
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

// ======================================= Q value manipulation ========================================
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

// --------------------------------- init----------------------------
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
	//	cout << "init q table for " << toFullString() << endl;
	for (int dir = 0; dir < DIRECTIONS; dir++) {
		initQTableForANeighbor(dir);
	}
	//	cout << qTableString() << endl;
}

void NoximDVFSUnit::initQTablesForAll() {
	const int MAX_ID = getMaxId();
	for (int i = 0; i <= MAX_ID; i++) {
		NoximDVFSUnit* dvfs = NoximDVFSUnit::getDVFS(i);
		dvfs->initQTable();
	}
}

void NoximDVFSUnit::printAllQTables() {
	const int MAX_ID = getMaxId();
	for (int i = 0; i <= MAX_ID; i++) {
		NoximDVFSUnit* dvfs = NoximDVFSUnit::getDVFS(i);
		cout << dvfs->qTableString() << endl;
	}
}

void NoximDVFSUnit::setQTableForANeighborOnFreqScaling(int nDir,
		double queueTimeY, unsigned int newDivision,
		unsigned int prevDivision) {
	const int MAX_ID = getMaxId();
	// neighbor exists
	if (nUnit[nDir]) {
		for (int i = 0; i <= MAX_ID; i++) {
			//TODO should avoid the case when getId() == y.id?
			if (getId() == i)
				continue;
			double preValue = getQValue(i, nDir);
			double qValue = preValue
					+ queueTimeY * (newDivision - prevDivision);
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
				n->setQTableForANeighbor(getOppositDir(dir), Q_INFINITY);
				break;
			case Q_NOTIFY_INIT:
				n->initQTableForANeighbor(getOppositDir(dir));
				break;
			case Q_NOTIFY_FREQ_SCALING:
				n->setQTableForANeighborOnFreqScaling(getOppositDir(dir),
						queueTime, division, preDivision);
				break;
			default:
				assert(false);
			}
			cout << n->qTableString() << endl;
		}
	}
}

// ----------------------------- Regular delivery ------------------------------------
void NoximDVFSUnit::updateQTable(int dirIn, NoximRouteData & routeData) {
	if (NoximGlobalParams::dvfs_regular_update == false) {
		//		cout << "NoximGlobalParams::dvfs_regular_update == false" << endl;
		return;
	}

	if (routeData.canUpdateQTable[dirIn] == false) {
		if (NoximGlobalParams::verbose_mode > VERBOSE_OFF)
			cout << toString() << ": updateQTable: canUpdateQTable[dirIn] == "
					<< routeData.canUpdateQTable[dirIn]
					<< " -> there was a special case in routing, return! "
					<< endl;
		return;
	}
	const int MAX_ID = getMaxId();
	for (int i = 0; i < DIRECTIONS; i++) {
		NoximDVFSUnit* neighbor = nUnit[i];
		// neighbor exists
		if (neighbor) {
			for (int dstId = 0; dstId <= MAX_ID; dstId++) {
				if (getId() == dstId)
					continue;

				// Q'x(d,y) = Qx(d,y) + deltaQx(d,y)
				// deltaQx(d,y) = eta * (qx + 1 + ty - Qx(d,y))
				const double currentQValue = getQValue(dstId, i);
				// not update infinity value
				if (currentQValue >= Q_INFINITY) {
					if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
						cout << toString()
								<< ": setQTableForRegularFlitDelivery current q vlaue >= infinity, return!"
								<< endl;
					}
					continue;
				}

				int neighborDirOfMinQy = neighbor->getDirWithMinQValue(dstId);
				double ty;
				if (neighborDirOfMinQy == DIRECTION_LOCAL)
					ty = neighbor->getQueueTime();
				else
					ty = neighbor->getQValue(dstId, neighborDirOfMinQy);

				if (ty >= Q_INFINITY) {
					if (NoximGlobalParams::verbose_mode > VERBOSE_OFF)
						cout << toString()
								<< " ty is infinity, dont't update this q value, continue "
								<< endl;
					continue;
				}

				const double delta = NoximGlobalParams::ETA
						* (getQueueTime() + ty - currentQValue);
				if (delta != 0.0)
					if (NoximGlobalParams::verbose_mode > VERBOSE_OFF)
						cout << toString()
								<< ": setQTableForRegularFlitDelivery: delta = "
								<< delta << " > 0: " << "qx = " << queueTime
								<< ", ty = " << ty << ", currentQValue = "
								<< currentQValue << endl;

				const double newQValue = currentQValue + delta;

				// print calculation
				if (NoximGlobalParams::verbose_mode > VERBOSE_LOW)
					cout << toString()
							<< ": setQTableForRegularFlitDelivery:\n\tQ'x(d,y) = Qx(d,y) + DeltaQx(d,y) where Qx(d,y) = Qx("
							<< dstId << ", " << getDirStr(i) << ") = "
							<< currentQValue
							<< "\n\t  Delta = ETA * (qx + ty - Qx(d,y)\n\t\t= "
							<< NoximGlobalParams::ETA << " * (" << queueTime
							<< " + " << ty << " - " << currentQValue
							<< ")\n\t\t= " << delta << "\n\t  Q'x(d,y) = "
							<< newQValue << endl;
				setQValue(dstId, i, newQValue);
			}
		}
	}
}

void NoximDVFSUnit::setQueueTime(double qTime) {
	this->prevQueueTime = this->queueTime;
	this->queueTime = qTime;
	if (NoximGlobalParams::verbose_mode > VERBOSE_OFF)
		cout << toString() << ": setQueueTime: " << prevQueueTime << " -> "
				<< this->queueTime << endl;
	//	updateQTableWithQueueTimeChange();
}

double NoximDVFSUnit::getQueueTime() {
	return this->queueTime;
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
	return toString(true);
}

char* NoximDVFSUnit::toString(bool time) const {
	char* ret = (char*) malloc(100 * sizeof(char));
	sprintf(ret, "%sDVFS[%3d]%s", time ? currentTimeStr() : "", id,
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
		sprintf(ret, "%s%18s\t", ret, getDirStr(dir));
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
				sprintf(ret, "%s %s=%s\t", ret, d->toString(false), "INF");
			else
				sprintf(ret, "%s %s=%3.3f\t", ret, d->toString(false),
						getQValue(i, dir));
		}
		sprintf(ret, "%s\n", ret);
	}
	return ret;
}
//----------------ID, coord and toString------END------------------------

// --------------------- divider----------------------------------------
bool NoximDVFSUnit::isDutyCycle() {
//	if (off)
//		return false;
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
	//	if (divisionCount != 0 && divisionCount % 1000 == 0)
	//		cout << "NoximDVFSUnit.increment division counter, divisionCount = "
	//				<< divisionCount << endl;
	if (reset.read())
		divisionCount = 0;
	else
		divisionCount++;
}

void NoximDVFSUnit::setDivision(unsigned int division) {
	this->preDivision = this->division;
	this->division = division;
	if (this->preDivision != this->division)
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
	assert(neighbor != NULL);
	return neighbor->off;
}
// ---------END--------- divider----------------------------------------

//============================= dvfs setting ======================================
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
