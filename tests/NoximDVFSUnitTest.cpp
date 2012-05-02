#include "NoximDVFSUnitTest.h"
#include "NoximMain.h"
#include "NoximNoC.h"
#include <systemc.h>
#include <vector>

void NoximDVFSUnitTest::run() {
	testRoutingQ();
	testDirWithMinQValue();
	testGetQValue();
	testQTableChangeRegularDelivery();
}

void NoximDVFSUnitTest::printVector(const vector<int>& v) {
	cout << "vector: ";
	for (int i = 0; i < v.size(); i++)
		cout << getDirStr(v[i]) << ",";
	cout << endl;
}

void NoximDVFSUnitTest::printArray(int array[]){
    int i = 0;
    cout << "array: ";
    while(array[i]){
        cout << getDirStr(array[i]) << ", ";
        i++;
        array++;
    }
    cout << endl;
}

vector<int> NoximDVFSUnitTest::runRouting(const int srcId, const int dstId){
    NoximDVFSUnit* srcUnit = NoximDVFSUnit::getDVFS(srcId);
    vector<int> ret = srcUnit->scheduleRoutingPath(dstId);
    cout << "Routing result: ";
    printVector(ret);
    return ret;
}

vector<int> NoximDVFSUnitTest::runAndAssertRouting(const int srcId, const int dstId, vector<int> expected){
	vector<int> ret = runRouting(srcId, dstId);
	printVector(expected);
	test_(expected.size() == ret.size());
    for (int i = 0; i < ret.size(); i++){
        if(expected[i]!=ret[i]){
            cout << "expected: " << expected[i] << ", but actual: " << ret[i] << endl;
            test_(expected[i]==ret[i]);
        }
    }
    return ret;
}

vector<int> NoximDVFSUnitTest::runAndAssertRoutingWithArray(const int srcId, const int dstId, int expected[]){
    cout << "run with array" << endl;
    vector<int> exp (expected, expected + sizeof(expected) / sizeof(int) );
    return runAndAssertRouting(srcId, dstId, exp);
}


void NoximDVFSUnitTest::testRoutingQ(){
    cout << "--------------------Testing NoximDVFSUnit-----------------------" << endl;
    NoximGlobalParams::mesh_dim_x = 2;
    NoximGlobalParams::mesh_dim_y = 2;
    NoximGlobalParams::verbose_mode = VERBOSE_HIGH;
    NoximNoC* noc = new NoximNoC("NoximNoC");

    // 0 -> ?
    vector<int> ret = runRouting(0,1);
    test_(ret.size()==1);
    test_(DIRECTION_EAST == ret[0]);

    ret = runRouting(0,2);
    test_(ret.size()==1);
    test_(DIRECTION_SOUTH == ret[0]);

    ret = runRouting(0,3);
    test_(ret.size()==2);
    test_(DIRECTION_EAST==ret[0]);
    test_(DIRECTION_SOUTH==ret[1]);

    // 1 -> ?
    ret = runRouting(1, 0);
    test_(ret.size()==1);
    test_(DIRECTION_WEST == ret[0]);

    ret = runRouting(1, 2);
    test_(ret.size()==2);
    test_(DIRECTION_SOUTH == ret[0]);
    test_(DIRECTION_WEST == ret[1]);

    ret = runRouting(1, 3);
    test_(ret.size()==1);
    test_(DIRECTION_SOUTH == ret[0]);

    // 2 -> ?
    ret = runRouting(2, 0);
    test_(ret.size() == 1);
    test_(DIRECTION_NORTH == ret[0]);

    ret = runRouting(2, 1);
    test_(ret.size() == 2);
    test_(DIRECTION_NORTH == ret[0]);
    test_(DIRECTION_EAST == ret[1]);

    ret = runRouting(2, 3);
    test_(ret.size() == 1);
    test_(DIRECTION_EAST == ret[0]);

    // 3 -> ?
    ret = runRouting(3,0);
    test_(ret.size()==2);
    test_(DIRECTION_NORTH == ret[0]);
    test_(DIRECTION_WEST == ret[1]);

    ret = runRouting(3, 1);
    test_(ret.size() == 1);
    test_(ret[0] == DIRECTION_NORTH);

    ret = runRouting(3, 2);
    test_(ret.size() == 1);
    test_(ret[0] == DIRECTION_WEST);
}

void NoximDVFSUnitTest::testDirWithMinQValue(){
	NoximDVFSUnit* dvfs0 = NoximDVFSUnit::getDVFS(0);
	// dst id = 0 ==> local
	int dir = dvfs0->getDirWithMinQValue(0);
	test_(DIRECTION_LOCAL == dir);

	dir = dvfs0->getDirWithMinQValue(3);
	test_(DIRECTION_EAST == dir);

	dir = dvfs0->getDirWithMinQValue(8);
	test_(-1 == dir);
}

void NoximDVFSUnitTest::testGetQValue(){
    NoximDVFSUnit* dvfs3 = NoximDVFSUnit::getDVFS(3);
    test_(0.0 == dvfs3->getQValue(3,DIRECTION_EAST));
    test_(3.0 == dvfs3->getQValue(1,DIRECTION_WEST));
}

void NoximDVFSUnitTest::testQTableChangeRegularDelivery(){
    NoximDVFSUnit* dvfs2 = NoximDVFSUnit::getDVFS(2);
    NoximDVFSUnit* dvfs3 = NoximDVFSUnit::getDVFS(3);
    dvfs3->setQueueTime(100.0);
    dvfs2->notifyNeighborWithRegularFlitDelivery(DIRECTION_EAST, 0);
    test_(52.0 == dvfs3->getQValue(0, DIRECTION_WEST));
    dvfs3->setQueueTime(1.0);
    dvfs2->notifyNeighborWithRegularFlitDelivery(DIRECTION_EAST, 0);
    test_(27.5 == dvfs3->getQValue(0, DIRECTION_WEST));
}



