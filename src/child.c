#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>

int main(int argc, char* argv[], char* envp[]) {
  if (argc != 2) {
    fprintf(stderr, "%d is invalid arguments amount, must be 2\n", argc);
    exit(EXIT_FAILURE);
  }

  puts("Child process data:");
  printf("Name: %s\n", argv[0]);
  printf("Pid: %d\n", getpid());
  printf("Ppid: %d\n", getppid());

  const int MAX_SIZE = 256;
  char      buffer[MAX_SIZE];
  FILE* fenvp = fopen(argv[1], "r");
  if (!fenvp) {
    perror("fenvp");
    exit(errno);
  }
  while (fgets(buffer, MAX_SIZE, fenvp) != NULL) {
    buffer[strcspn(buffer, "\n")] = '\0';
    printf("%s=%s\n", buffer, getenv(buffer));
  }

  fclose(fenvp);
  exit(EXIT_SUCCESS);
}
