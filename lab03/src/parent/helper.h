#ifndef PARENT__HELPER_H_
#define PARENT__HELPER_H_

#include <stdbool.h>
#include <signal.h>

#include <sys/types.h>

#include "../protocol.h"

#define MAX_CHILD 1024

typedef struct {
  pid_t pid;
  bool  print_allowed;
} child_t;

// Single child actions

void create_child(void);
pid_t kill_last_child(void);

// All child actions

void kill_all(void);

// Utility

void init(void);

void atexit_handler(void);
void child_handler(int sig, siginfo_t* info, void* context);

void send_signal(child_t child, enum parent signal);

size_t get_index_by_pid(pid_t pid);

#endif //PARENT__HELPER_H_
