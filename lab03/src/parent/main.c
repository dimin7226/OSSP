#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "helper.h"

size_t  amount = 0;
child_t child_array[MAX_CHILD];

int main(void) {
  init();

  printf("[+] - create child process\n"
         "[-] - kill last child process\n"
         "[l] - list parent and child processes\n"
         "[k] - kill all child processes\n"
         "[q] - exit\n"
         ">");
  while (1) {
    switch (getchar()) {
      case '+':
        create_child();
        printf("Child with pid=%d was created\n", child_array[amount - 1].pid);
        break;

      case '-':
        if (amount > 0) {
          pid_t pid = kill_last_child();
          printf("Child with pid=%d was killed, %zu left\n", pid, amount);
        } else {
          puts("No child processes to kill");
        }
        break;

      case 'l':
        printf("Parent process pid=%d, amount of children: %zu\n", getpid(), amount);
        for (size_t i = 0; i < amount; i++) {
          printf("Child C_%zu pid=%d\n", i, child_array[i].pid);
        }
        break;

      case 'k':
        if (amount > 0) {
          kill_all();
          puts("All child processes were killed");
        } else {
          puts("No child processes to kill");
        }
        break;

      case 'q':
        if (amount > 0) {
          kill_all();
          puts("All child processes were killed, program is finished");
        }
        exit(EXIT_SUCCESS);

      case '\n':
      case ' ':
        printf(">");
        break;

      default:
        break;
    }
  }
}
