/*
 * File:   NoximDVFSUnit
 * Author: nan
 *
 */

#ifndef NOXIMDVFSUNIT_H
#define	NOXIMDVFSUNIT_H

#include <systemc.h>
#include <string>
#include "NoximMain.h"
#include "NoximDVFSSetting.h"
#include "DVFSAction.h"

using namespace std;

SC_MODULE(NoximDVFSUnit) {
	const static int Q_NOTIFY_INIT = 0;
	const static int Q_NOTIFY_INFINITY = 1;
	const static int Q_NOTIFY_FREQ_SCALING = 2;

	const static double Q_INFINITY = 1000000000.0; // 1 billon

	// ---------divider-----------------
	sc_in_clk clock;
	sc_in<bool> reset;
	sc_out<bool> offSignal;
	bool off;
	unsigned int division;
	unsigned int preDivision;
	void incrementDivisionCounter();
	void checkDVFSActions();
	void executeAction(DVFSAction action);

	//-----------------id, coord, toString, neighbor------------------
	// static array
	static NoximDVFSUnit* getDVFS(int x, int y);
	static NoximDVFSUnit* getDVFS(const NoximCoord& c);
	static NoximDVFSUnit* getDVFS(int id);
	static void setDVFS(int id, NoximDVFSUnit* dvfs);

	// neighbor unit
	NoximDVFSUnit* nUnit[DIRECTIONS];
	void setNUnit(int dir, NoximDVFSUnit* neighbor);

	// coord
	NoximCoord coord;
	NoximCoord getCoord() const {
		return coord;
	}
	void setCoord(int x, int y);

	// id
	int id;
	void setId(int aId);
	int getId() const;

	char* toString() const;
	char* toString(bool time) const;

	char* toFullString() const;
	char* qTableString() const;
	//-----------------id, coord, toString, neighbor----END-----------

	SC_CTOR(NoximDVFSUnit) {

		// divider functions
		off = false;
		division = 1;
		preDivision = 1;
		SC_METHOD( incrementDivisionCounter);
		sensitive << reset;
		sensitive << clock.neg();

		SC_METHOD( checkDVFSActions);
		sensitive << reset;
		sensitive << clock.pos();

		queueTime = 1.0;
		prevQueueTime = 1.0;

		// init neighbor units
		for (int i = 0; i < DIRECTIONS; i++)
		nUnit[i] = NULL;

		// init q table
		for(int dir=0;dir <DIRECTIONS;dir++)
		for(int destination =0; destination < MAX_STATIC_DIM * MAX_STATIC_DIM; destination ++)
		setQValue(destination, dir, Q_INFINITY);
	}

public:
	int getDirWithMinQValue(int dstId);
	int getDirWithMinQValueFromDirs(int dstId, int excludedDir);

	vector<int> getDirsWithMinQValueFromDirs(int dstId, int excludedDir);

	double getQValue(int dstId, int yDir);
	void setQValue(int dstId, int yDir, double qValue);
	void notifyAllNeighbors(int event);
	void setQTableForANeighbor(int nDir, double qValue);
	void setQTableForANeighborOnFreqScaling(int nDir, double queueTimeY,
			unsigned int newDivision, unsigned int prevDivision);
	void updateQTable(int dirIn, NoximRouteData& routeData);
	void initQTableForANeighbor(int nDir);
	void initQTable();

	static void initQTablesForAll();
	static void printAllQTables();
	static int distance(NoximDVFSUnit* dvfs1, NoximDVFSUnit* dvfs2);

	//routing
	int routingQ(NoximRouteData & routeData);
	vector<int> nonDeterminsticRoutingQ(NoximRouteData & routeData);
	vector<int> scheduleRoutingPath(const int dstId);

	// divider
	void setDivision(unsigned int division);
	void setOff(bool off);
	bool isNeighborOff(int dir);
	bool isDutyCycle();
	void setQueueTime(double qTime);
	double getQueueTime();
//	void updateQTableWithQueueTimeChange(int sendToDstId, int sendToDir);

	// dvfs-setting
	vector<DVFSAction> actions;

private:
	double qTable[DIRECTIONS][MAX_STATIC_DIM * MAX_STATIC_DIM];
	unsigned int divisionCount;
	double queueTime;
	double prevQueueTime;
};
#endif

