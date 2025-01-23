
#define ID_LENGTH 16
#define MAX_FILES 1000

#ifndef FILEIO_H
#define FILEIO_H

void generate_random_id(char *id);
int is_unique_id(const char *id);
// void assign_unique_id_to_files(char file_names[][256], int file_count);
#endif
