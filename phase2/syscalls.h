#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "ker_exports.h"

void syscall_handler(state_t* caller);

void PassOrDie(state_t* caller, int exc_type);

#endif
