#include "helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/wait.h>
#include <unistd.h>

extern size_t  amount;
extern child_t child_array[MAX_CHILD];

// Single child actions

void create_child(void) {
  if (amount == MAX_CHILD - 1) {
    fputs("Child amount overflow", stderr);
    exit(EXIT_FAILURE);
  }

  pid_t ret = fork();
  switch (ret) {
    case 0: {
      // Child process
      char child_name[8];
      sprintf(child_name, "C_%zu", amount);
      if (execlp("./child", child_name, NULL) == -1) {
        perror("execlp");
        exit(errno);
      }
    }
      break;

    case -1:
      perror("fork");
      exit(errno);

    default:
      // Parent process
      child_array[amount].pid           = ret;
      child_array[amount].print_allowed = true;
      ++amount;
      break;
  }
}

pid_t kill_last_child(void) {
  if (amount == 0) {
    fputs("child_array is empty", stderr);
    exit(EXIT_FAILURE);
  }
  pid_t pid = child_array[--amount].pid;
  send_signal(child_array[amount], PARENT_KILL);
  waitpid(pid, NULL, 0);
  return pid;
}

// All child actions

void kill_all(void) {
  while (amount > 0) {
    kill_last_child();
  }
}

// Utility

void init(void) {
  // Init atexit
  if (atexit(atexit_handler)) {
    fputs("atexit failed", stderr);
    exit(EXIT_FAILURE);
  }

  // Init child signals handler
  struct sigaction child_signals;
  child_signals.sa_flags     = SA_SIGINFO;
  child_signals.sa_sigaction = child_handler;

  if (sigaction(SIGUSR2, &child_signals, NULL)) {
    perror("sigaction");
    exit(errno);
  }
}

void atexit_handler(void) {
  kill_all();
  puts("All child processes are killed, program is finished");
}

void child_handler(int sig, siginfo_t* info, void* context) {
  size_t i = get_index_by_pid(info->si_pid);

  switch (info->si_int) {
    case CHILD_ASK:
      if (child_array[i].print_allowed) {
        send_signal(child_array[i], PARENT_RESPONSE);
      }
      break;

    case CHILD_INFORM:
      printf("Child with ppid=%d, pid=%d informed parent\n", getpid(), child_array[i].pid);
      break;

    default:
      fprintf(stderr,
              "Unknown signal occurred in child_handler: %d\n",
              info->si_int);
      exit(EXIT_FAILURE);
  }
}

void send_signal(child_t child, enum parent signal) {
  union sigval value = {signal};
  if (sigqueue(child.pid, SIGUSR1, value)) {
    perror("sigqueue");
    exit(errno);
  }
}

size_t get_index_by_pid(pid_t pid) {
  for (size_t i = 0; i < amount; ++i) {
    if (child_array[i].pid == pid) {
      return i;
    }
  }
  fprintf(stderr, "pid=%d is not child of the process\n", pid);
  exit(EXIT_FAILURE);
}
```# CODE

The next step in our plan is to update the `helper.h` file in the parent directory to remove declarations of unused functions that were previously removed from `helper.c`. This modification ensures that we maintain consistency between the header file and its implementation, preventing potential issues with undeclared or unused functions. Specifically, we need to remove declarations related to statistics control functions (`stop_stat`, `resume_stat`, etc.) and their associated utilities (`call_corresponding_proc`, `validate_num`, etc.), while preserving essential process management functions like `create_child` and `kill_all`.

<mermaid>
graph TD
    Start[Start: Update helper.h] --> A{Identify unused function declarations}
    A -- Yes --> B[Remove declarations for stop_stat, resume_stat, print_stat]
    B --> C[Remove declarations for stop_all_stat, resume_all_stat]
    C --> D[Remove declarations for utility functions like call_corresponding_proc]
    D --> E[Remove alarm_handler declaration]
    E --> F[Ensure consistency with helper.c implementation]
    F --> G[Validate header file integrity]
    G --> End[Header file updated successfully]
</mermaid>

(modified lab03/src/parent/helper.h)