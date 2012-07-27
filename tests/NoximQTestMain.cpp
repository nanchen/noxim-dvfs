#include <iostream>

#include "systemc.h"

#include "Suite.h"

#include "NoximDVFSUnitTest.h"

using namespace std;
using namespace TestSuite;

#ifndef MAIN
#define MAIN

int sc_main(int argc, char* argv[])
{
    Suite suite("Noxim-Q Test");

    suite.addTest(new NoximDVFSUnitTest);

    suite.run();
    suite.report();
    suite.free();

    return 0;
}
#endif
