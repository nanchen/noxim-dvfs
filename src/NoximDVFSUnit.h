/*
 * File:   NoximDVFSUnit
 * Author: nan
 *
 */

#ifndef NOXIMDVFSUNIT_H
#define	NOXIMDVFSUNIT_H

#include <systemc.h>
#include "NoximMain.h"

using namespace std;

SC_MODULE(NoximDVFSUnit) {
	//	sc_in_clk clock;
	//	sc_in<bool> reset;

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
	char* toFullString() const;
	char* qTableString() const;
	//-----------------id, coord, toString, neighbor------------------

	SC_CTOR(NoximDVFSUnit) {
		// init neighbor units
		for (int i = 0; i < DIRECTIONS; i++)
			nUnit[i] = NULL;
		//		SC_METHOD( incrementDivisionCounter);
		//		sensitive << reset;
		//		sensitive << clock.neg();
	}

public:
	void initQTable();
	static void initQTablesForAll();
	static int distance(NoximDVFSUnit* dvfs1, NoximDVFSUnit* dvfs2);
private:
	float qTable[DIRECTIONS][MAX_STATIC_DIM * MAX_STATIC_DIM];
};
#endif

