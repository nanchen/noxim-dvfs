/*
 * File:   NoximDivider
 * Author: nan
 *
 */

#ifndef NOXIMDIVIDER_H
#define	NOXIMDIVIDER_H

#include <systemc.h>

using namespace std;

SC_MODULE(NoximDivider) {
	sc_in_clk clock;
	sc_in<bool> reset;

	bool off;
	unsigned int division;

	void incrementDivisionCounter();

	SC_CTOR(NoximDivider) {
		off = false;
		division = 1;

		SC_METHOD( incrementDivisionCounter);
		sensitive << reset;
		sensitive << clock.neg();
	}

public:
	bool isDutyCycle();

private:
	unsigned int divisionCount;
};

#endif

