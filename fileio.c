#include "fileio.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


char unique_ids[MAX_FILES][ID_LENGTH + 1];  // Definition of unique_ids
int unique_id_count = 0;
void generate_random_id(char *id) {
  const char charset[] =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  for (int i = 0; i < ID_LENGTH; i++) {
    int key = rand() % (int)(sizeof(charset) - 1);
    id[i] = charset[key];
  }
  id[ID_LENGTH] = '\0';
}

// Check if an ID is unique (not already in unique_ids)
int is_unique_id(const char *id) {
  for (int i = 0; i < unique_id_count; i++) {
    if (strcmp(unique_ids[i], id) == 0) {
      return 0; // ID is not unique
    }
  }
  return 1; // ID is unique
}

