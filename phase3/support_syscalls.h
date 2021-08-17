#ifndef _SUPPORT_SYSCALLS_H_
#define _SUPPORT_SYSCALLS_H_

#define CREATEPROCESS 1
#define TERMINATEPROCESS 2
#define PASSEREN 3
#define VERHOGEN 4
#define WAITIO 5
#define GETCPUTIME 6
#define WAITCLOCK 7
#define GETSUPPORTPTR 8

#define PRINTCHR 2
#define BYTELEN 8

#include "proj_lib.h"
#include "sup_exports.h"

static void support_syscall_handler(state_t* caller);

#endif
