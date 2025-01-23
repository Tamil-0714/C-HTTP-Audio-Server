#include "dbfnc.h"
#include "fileio.h"
#include "globals.h"
#include "stdfnc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void update_DB_records() {
  const char *path = "sounds";
  int file_count = 0;

  char **names = list_files(path, &file_count);
  if (NULL == names) {
    perror("No files found");
    return;
  }
  for (int i = 0; names[i] != NULL; i++) {
    char id[ID_LENGTH + 1];
    do {
      generate_random_id(id);
    } while (!is_unique_id(id));
    char *id_copy = strdup(id);
    if (unique_id_count >= MAX_FILES) {
      unique_id_count = 0;
    }
    strcpy(unique_ids[unique_id_count++], id_copy);
    char *name = strdup(names[i]);
    save_to_db(id_copy, name);
    free(id_copy);
    free(name);
  }
  free_file_array(names, file_count);
}

int main(int argc, char **argv) {
    init_db();
  update_DB_records();
  printf("files json if key : %s\n",display_all_files());
  return 0;
}