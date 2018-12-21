// signal.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "stdint.h"
#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int sig_atomic_t;
typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t newhandler);
int raise(int signum);
int kill(pid_t pid, int signum);
















#define SIGHUP	1	// Hangup
#define SIGINT		2	// Interrupt
#define SIGQUIT	3	// Quit
#define SIGILL		4	// Illegal instruction
#define SIGTRAP	5	// Trace/breakpoint trap
#define SIGABRT	6	// Aborted
#define SIGEMT	7	// Emulate
#define SIGFPE	8	// Floating point exception
#define SIGKILL	9	// Killed
#define SIGBUS	10	// User defined signal 1
#define SIGSEGV	11	// Segmentation fault
#define SIGSYS	12	// Bad system call
#define SIGPIPE	13	// Broken pipe
#define SIGALRM	14	// Alarm clock
#define SIGTERM	15	// Terminated
#define SIGURG	16	// Urgent I/O
#define SIGSTOP	17	// Stop (cannot cancel)
#define SIGTSTP	18	// Stop (from keyboard, can cancel)
#define SIGCONT	19	// Continue stopped proc
#define SIGCHLD	20	// Child status changed
#define SIGTTIN	21	// Stopped (tty input)
#define SIGTTOU	22	// Stopped (tty output)
#define SIGIO		23	// I/O Possible
#define SIGXCPU	24	// CPU time limit exceeded
#define SIGXFSZ	25	// File size limit exceeded
#define SIGVTALRM	26	// Virtual timer expired
#define SIGPROF	27	// Profiling timer
#define SIGWINCH	28	// Window changed
#define SIGINFO	29	// Info requested
#define SIGUSR1	30	// First user-available real-time signal.
#define SIGUSRMAX	127	// Last user-available real-time signal.



#define __SIGCOUNT	128

#define SIG_ERR ((void (*)(int)) -1)
#define SIG_DFL ((void (*)(int)) 0)
#define SIG_IGN ((void (*)(int)) 1)














#ifdef __cplusplus
}
#endif
