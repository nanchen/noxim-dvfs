#ifndef __NOXIM_DVFS_SETTING_H__
#define __NOXIM_DVFS_SETTING_H__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "NoximMain.h"
#include "DVFSAction.h"

using namespace std;


class NoximDVFSSetting {

  public:

	NoximDVFSSetting();

    // Load traffic table from file. Returns true if ok, false otherwise
    static bool load(const char *fname);

  private:

};

#endif
