#include <stdio.h>

#include "arguments.h"

extern int test_function();

int main(int argc, char *argv[]) {
  struct Configuration cnf;
  parse_arguments(argc, argv, &cnf);

  printf("The linked C++ test_function returned: %d\n", test_function());

  return 0;
}
