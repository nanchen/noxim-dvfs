#include "DVFSAction.h"

char* DVFSAction::toString() const {
    char* ret = (char*) malloc(150 * sizeof (char));
    sprintf(
            ret,
            "DVFSAction[%3d] at %s, timeStamp: %d, action: %s, (division = %d)",
            id, coord.toString(), timeStamp, action.c_str(), division);
    return ret;
}
