#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "helper.h"

static const binary_t zero  = {0, 0};
static const binary_t three = {1, 1};

binary_t data;
stat_t   stat;

int alarm_count;

int main(void) {
  struct timespec sleep_time;
  sleep_time.tv_sec = 0;
  sleep_time.tv_nsec = ALARM_DELAY * 1000; // Convert microseconds to nanoseconds
  
  // Initialize statistics
  stat.O_O = 0;
  stat.O_I = 0;
  stat.I_O = 0;
  stat.I_I = 0;
  
  alarm_count = 0;
  
  // Register exit handler
  if (atexit(atexit_handler)) {
    fputs("atexit failed", stderr);
    exit(EXIT_FAILURE);
  }
  
  while (1) {
    // Alternate data between zero and three
    data = zero;
    
    // Small delay to allow potential modification
    nanosleep(&sleep_time, NULL);
    
    // Check if modification happened and update statistics
    if (data.O == 0 && data.I == 0) ++stat.O_O;
    else if (data.O == 0 && data.I == 1) ++stat.O_I;
    else if (data.O == 1 && data.I == 0) ++stat.I_O;
    else ++stat.I_I;
    
    ++alarm_count;
    
    data = three;
    
    // If we've collected enough statistics, print and reset
    if (alarm_count >= ALARM_COUNTS_TO_PRINT) {
      printf("ppid=%d, pid=%d, stat={%zu, %zu, %zu, %zu}\n",
             getppid(), getpid(), stat.O_O, stat.O_I, stat.I_O, stat.I_I);
      
      // Reset for next cycle
      alarm_count = 0;
      stat.O_O = 0;
      stat.O_I = 0;
      stat.I_O = 0;
      stat.I_I = 0;
    }
  }
}
