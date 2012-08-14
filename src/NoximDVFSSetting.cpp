#include "NoximDVFSSetting.h"
#include "NoximDVFSUnit.h"

NoximDVFSSetting::NoximDVFSSetting() {
}
static vector<DVFSAction> actions;

bool NoximDVFSSetting::load(const char *fname) {
	// Open file
	ifstream fin(fname, ios::in);
	if (!fin)
		return false;

	// Initialize variables
	actions.clear();

	cout << "DVFSSetting::load() DVFS settings from " << fname << endl;
	// Cycle reading file
	while (!fin.eof()) {
		char line[512];
		fin.getline(line, sizeof(line) - 1);

		if (line[0] != '\0') {
			if (line[0] != '%' && line[0] != '#') {

				int id, timeStamp;
				char action[10];
				unsigned int division = 0;

				int params = sscanf(line, "%d %d %s %d", &id, &timeStamp,
						&action, &division);
//				cout << "params = " << id << " @ " << timeStamp
//						<< ": " << action << " division = " << division << endl;
				if (params >= 3) {
					DVFSAction dvfsAction = DVFSAction();

					dvfsAction.coord = id2Coord(id);
					dvfsAction.id = id;

					dvfsAction.timeStamp = timeStamp;
					dvfsAction.action = action;

					if (params == 4 && division > 0)
						dvfsAction.division = division;

					actions.push_back(dvfsAction);
					NoximDVFSUnit* unit = NoximDVFSUnit::getDVFS(dvfsAction.id);
					unit->actions.push_back(dvfsAction);

//					cout << "Parsed dvfs action: " << dvfsAction.toString()
//							<< endl;
					DVFSAction dvfsInVecotr = unit->actions.back();
					cout << "Parsed dvfs action: " << dvfsInVecotr.toString()
							<< endl;
				}
			}
		}
	}
	return true;
}
