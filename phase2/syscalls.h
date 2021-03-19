#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "ker_exports.h"

void PassOrDie(state_t* caller, int exc_type);

#endif
