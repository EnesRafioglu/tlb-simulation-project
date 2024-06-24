#include <stdio.h>
#include <stdlib.h>
#include "arguments.h"

#ifdef __cplusplus
extern "C" {
#endif

  #include "csv.hpp"

#ifdef __cplusplus
}
#endif


extern int test_function();


int main(int argc, char *argv[]) {
  struct Configuration cnf;
  parse_arguments(argc, argv, &cnf);

  int line_count;
  struct Request* requests;
  requests = parse_csv(cnf.inputFile, &line_count);

  printf("%u\n",line_count );
  for (int i = 0; i < line_count; i++) {
    struct Request req;
    req = requests[i];

    printf("Request %d: addr=%d, data=%d, we=%d\n", i, req.addr, req.data, req.we);
    
  }

  printf("The linked C++ test_function returned: %d\n", test_function());

  free(requests);
  return 0;
}
