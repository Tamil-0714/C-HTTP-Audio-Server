#include "dbfnc.h"
#include <stdio.h>
int main() {
  //   init_db();
  //   save_to_db("key1", "value1");
  //   save_to_db("nice", "ok");
  //   printf("Test : %s\n", get_from_db("nice"));
  init_db();
  save_to_db("key1", "value1");
  save_to_db("nice", "ok");
  printf("Test : %s\n", get_from_db("nice"));
  return 0;
}