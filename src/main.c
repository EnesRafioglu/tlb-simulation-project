#include <stdio.h>
#include <stdlib.h>
#include "arguments.h"

#include "csv.h"

extern int test_function();


int main(int argc, char *argv[]) {
  struct Configuration cnf;
  parse_arguments(argc, argv, &cnf);

  int line_count;
  struct Request* requests;
  requests = parse_csv(cnf.inputFile, &line_count);

  printf("%u\n",line_count );
  if(requests == NULL) {
    printf("Error: Couldn't parse input file %s\n", cnf.inputFile);
    return 1;
  }
  
  for (int i = 0; i < line_count; i++) {
    struct Request req;
    req = requests[i];

    printf("Request %d: addr=%u, data=%u, we=%d\n", i, req.addr, req.data, req.we);
    
  }
  
  printf("The linked C++ test_function returned: %d\n", test_function());

  free(requests);

  return 0;
}
