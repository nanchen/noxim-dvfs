/*
 * File:   NoximDivider.cpp
 * Author: nan
 *
 */
#include "NoximDivider.h"

bool NoximDivider::isDutyCycle() {
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

void NoximDivider::incrementDivisionCounter() {
	//	cout << "NoximDivider.increment division counter, divisionCount = " << divisionCount << endl;
	if (reset.read())
		divisionCount = 0;
	else
		divisionCount++;
}

