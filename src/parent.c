#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <wait.h>
#include <unistd.h>

const char child_path[] = "CHILD_PATH";

extern char** environ;
extern const char child_path[];

void print_envp(char* envp[]);
char** create_child_env(char* fenvp);

char* search_child_path(char** str_arr);

int main(int argc, char* argv[], char* envp[]) {
  if (argc != 2) {
    fprintf(stderr, "%d is invalid amount of arguments, must be 2\n", argc);
    exit(EXIT_FAILURE);
  }

  print_envp(envp);
  char* const* env = create_child_env(argv[1]);

  size_t child_count = 0;
  int    opt;
  while (1) {
    printf("[+] - child process with getenv()\n"
           "[*] - child process with envp[]\n"
           "[&] - child process with environ\n"
           "[q] - exit\n"
           ">");
    opt = getchar();
    getchar();

    if (opt == 'q') {
      exit(EXIT_SUCCESS);
    }
    if (opt != '+' && opt != '*' && opt != '&') {
      continue;
    }

    char* child_process = NULL;
    switch ((char) opt) {
      case '+':
        child_process = getenv(child_path);
        break;

      case '*':
        child_process = search_child_path(envp);
        break;

      case '&':
        child_process = search_child_path(environ);
        break;
    }

    char child_name[10];
    sprintf(child_name, "child_%zu", child_count++);

    char* const args[] = {child_name, argv[1], NULL};

    pid_t pid = fork();
    if (pid > 0) {
      int status;
      wait(&status);
    } else if (pid == 0) {
      if (execve(child_process, args, env) == -1) {
        perror("execve");
        exit(errno);
      }
    } else {
      perror("fork");
      exit(errno);
    }
  }
}

static int qsort_cmp(const void* str1, const void* str2) {
  return strcmp(*(const char**) str1, *(const char**) str2);
}

void print_envp(char* envp[]) {
  size_t envpc = 0;
  while (envp[envpc]) {
    ++envpc;
  }

  qsort(envp, envpc, sizeof(char*), qsort_cmp);

  puts("Parent environment variables:");
  for (size_t i = 0; i < envpc; ++i) {
    puts(envp[i]);
  }
}

char** create_child_env(char* fenvp) {
  FILE* stream = fopen(fenvp, "r");
  if (!stream) {
    perror("fopen");
    exit(errno);
  }

  char** env = malloc(sizeof(char*));
  size_t i = 0;

  const int MAX_SIZE = 256;
  char      buffer[MAX_SIZE];
  while (fgets(buffer, MAX_SIZE, stream) != NULL) {
    buffer[strcspn(buffer, "\n")] = '\0';

    char* env_val = getenv(buffer);
    if (env_val) {
      env[i] = malloc((strlen(buffer) + strlen(env_val) + 2) * sizeof(char));
      strcat(strcat(strcpy(env[i], buffer), "="), env_val);
      env = realloc(env, (++i + 1) * sizeof(char*));
    }
  }
  env[i] = NULL;

  return env;
}

char* search_child_path(char** str_arr) {
  while (*str_arr) {
    if (!strncmp(*str_arr,  "CHILD_PATH", strlen( "CHILD_PATH"))) {
      return *str_arr + strlen("CHILD_PATH") + 1;
    }
    ++str_arr;
  }
  return NULL;
}
