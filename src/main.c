#include <stdio.h>

extern int test_function();

int main(int argc, char *argv[]) {
  printf("The linked C++ test_function returned: %d\n", test_function());

  return 0;
}
