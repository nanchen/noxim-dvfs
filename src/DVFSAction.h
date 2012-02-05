#ifndef __DVFS_ACTION_H__
#define __DVFS_ACTION_H__

#include <stdlib.h>
#include <string>

#include "NoximMain.h"

using namespace std;

// Structure used to store information into the table
class DVFSAction {
public:
	NoximCoord coord;
    int id;
    int timeStamp;
    string action;
    unsigned int division;
	char* toString() const;
};
#endif
