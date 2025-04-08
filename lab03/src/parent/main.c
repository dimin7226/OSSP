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
         "[s] <num> - stop statistic\n"
         "[g] <num> - resume statistic\n"
         "[p] num - print C_{num} statistic and freeze other childs\n"
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

      case 's':
        call_corresponding_proc(stop_stat, stop_all_stat);
        break;

      case 'g':
        call_corresponding_proc(resume_stat, resume_all_stat);
        break;

      case 'p':
        if (!is_number_in_stdin()) {
          fputs("Invalid command! Correct form is p<num>", stderr);
          exit(EXIT_FAILURE);
        }
        size_t num = read_size_t();
        
        if (!validate_num(num)) {
          break;
        }

        stop_all_stat();
        print_stat(child_array[num]);
        alarm(P_NUM_SEC_TIMER);
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
