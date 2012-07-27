#ifndef NOXIMDVFSUNITTEST_H
#define NOXIMDVFSUNITTEST_H

#include "Test.h"

#include "NoximDVFSUnit.h"


class NoximDVFSUnitTest : public TestSuite::Test
{
    public:
        void run();
        NoximDVFSUnitTest(){};
        ~NoximDVFSUnitTest(){};

    private:
        void testRoutingQ();
        void testDirWithMinQValue();
        void testGetQValue();
//        void testQTableChangeRegularDelivery();
        void printVector(const vector<int>& v);
        void printArray(int array[]);
        vector<int> NoximDVFSUnitTest::runRouting(const int srcId, const int dstId);
        vector<int> runAndAssertRouting(const int srcId, const int dstId, vector<int> expected);
        vector<int> runAndAssertRoutingWithArray(const int srcId, const int dstId, int expected[]);
};

#endif // NOXIMDVFSUNITTEST_H
