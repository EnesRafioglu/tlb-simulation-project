#include "main.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "arguments.h"
#include "csv.h"
#include "simulation/simulation.hpp"

int main(int argc, char* argv[]) {
  struct Request requests[] = {
    {11111111, 1, 1},
    {11111111, 2, 0},
    {11111111, 3, 1},
    {11111111, 4, 0},
    {11111111, 5, 1},
    {11111111, 6, 1},
    {11111111, 7, 0},
    {11111111, 8, 0},
    {11111111, 9, 0},
    {11111111, 10, 1},
    {11111111, 1, 0},
    {11111111, 0, 1},
    {11111111, 0, 1},
    {22222222, 0, 0},
    {33333333, 0, 0},
    {44444444, 0, 0},
    {55555555, 0, 1},
    {11111111, 0, 0},
    {66666666, 0, 0},
    {11111111, 0, 0},
    {77777777, 0, 0}
  };

  int maxCycles = 1000000;

  struct Result result = run_simulation(maxCycles,16,2,4096,12,100,sizeof(requests) / sizeof(requests[0]) ,requests,0);
  printf("%d\n%d\n%d\n%d\n", result.cycles, result.hits, result.misses, result.primitiveGateCount);



  return 0;
}

int check_architecture() {
  // Check if the size of a byte is 8 bits
  if (CHAR_BIT != 8) {
    fprintf(stderr,
            "Error: Architecture is not supported, the size of one byte is not "
            "8 bits\n");
    return 1;
  }

  // Check if the size of an int is 32 bits
  if (sizeof(int) != 4) {
    fprintf(stderr,
            "Error: Architecture is not supported, the size of an integer is "
            "not 32 bits\n");
    return 1;
  }

  return 0;
}
