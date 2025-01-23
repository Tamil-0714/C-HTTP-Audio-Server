#include "stdfnc.h"
#include "clrfnc.h"
#include "dbfnc.h"
#include "fileio.h"
#include "globals.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

unsigned char *read_html(const char *file_name, size_t *size) {
  FILE *file = fopen(file_name, "r");
  if (file == NULL) {
    perror("Error opening file");
    return NULL;
  }

  // Get file size
  fseek(file, 0, SEEK_END);
  *size = ftell(file);
  rewind(file);

  // Allocate memory (+1 for null terminator if needed)
  unsigned char *HTML_data = malloc(*size + 1);
  if (HTML_data == NULL) {
    perror("Memory allocation failed");
    fclose(file);
    return NULL;
  }

  // Read file into buffer
  size_t bytes_read = fread(HTML_data, 1, *size, file);
  if (bytes_read != *size) {
    perror("Error reading file");
    free(HTML_data);
    fclose(file);
    return NULL;
  }

  // Optional: Null-terminate the buffer
  HTML_data[*size] = '\0';

  fclose(file);
  return HTML_data;
}

unsigned char *read_audio(const char *file_name, size_t *size) {
  char *audio_path = "sounds/";
  int len = (strlen(file_name) + strlen(audio_path)) + 1;
  char internal_file_name_pointer[len];
  snprintf(internal_file_name_pointer, len, "%s%s", audio_path, file_name);
  yellow();
  printf("actual file name : %s\n", internal_file_name_pointer);
  reset();
  FILE *file = fopen(internal_file_name_pointer, "rb");
  if (NULL == file) {
    perror("Error opening FIle");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  *size = ftell(file);
  rewind(file);

  unsigned char *audio_buffer = malloc(*size);

  if (NULL == audio_buffer) {
    perror("Memory alloccation failed");
    fclose(file);
    return NULL;
  }

  fread(audio_buffer, 1, *size, file);
  fclose(file);
  return audio_buffer;
}

// Base64 encoding table
static const char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Function to perform Base64 encoding
char *base64_encode(const unsigned char *data, size_t input_length,
                    size_t *output_length) {
  size_t encoded_size = 4 * ((input_length + 2) / 3);
  char *encoded_data = malloc(encoded_size + 1); // +1 for null terminator
  if (encoded_data == NULL)
    return NULL;

  for (size_t i = 0, j = 0; i < input_length;) {
    uint32_t octet_a = i < input_length ? data[i++] : 0;
    uint32_t octet_b = i < input_length ? data[i++] : 0;
    uint32_t octet_c = i < input_length ? data[i++] : 0;

    uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

    encoded_data[j++] = base64_table[(triple >> 18) & 0x3F];
    encoded_data[j++] = base64_table[(triple >> 12) & 0x3F];
    encoded_data[j++] = base64_table[(triple >> 6) & 0x3F];
    encoded_data[j++] = base64_table[triple & 0x3F];
  }

  for (size_t i = 0; i < (encoded_size % 4); ++i) {
    encoded_data[encoded_size - 1 - i] = '=';
  }
  encoded_data[encoded_size] = '\0'; // Null terminator

  if (output_length)
    *output_length = encoded_size;
  return encoded_data;
}

char *extract_file_id(char *str) {

  if (strlen(str) < 7) {
    // printf("execlued\n");
    return NULL;
  }
  int offset = 7;
  int len = strlen(str) - offset;
  char *res = malloc(len + 1);
  if (NULL == res) {
    perror("Memory allocation failed ");
    return NULL;
  }
  for (int i = 0; i < len; i++) {
    res[i] = str[i + offset];
  }
  res[len] = '\0';
  return res;
}

int verify_audio_route(char *str) {
  char *toCompare = "/audio/";
  if (strlen(str) == 6) {
    if (strcmp(str, "/audio") == 0)
      return 1;
    else
      return 0;
  }
  for (int i = 0; i < strlen(toCompare); i++) {
    if (str[i] == '\0' || str[i] != toCompare[i]) {
      // printf("this is %c and this is %c\n", str[i], toCompare[i]);
      return 0;
    }
  }
  return 1;
}

// Function to add a file name to the array (with dynamic resizing).
char **add_file_to_array(char **files, int *count, int *capacity,
                         const char *file_name) {
  // Resize the array if needed.
  if (*count >= *capacity) {
    *capacity *= 2;
    files = realloc(files, (*capacity) * sizeof(char *));
    if (files == NULL) {
      perror("Memory reallocation failed");
      exit(EXIT_FAILURE); // Exit if realloc fails.
    }
  }

  // Allocate space for the file name and copy it into the array.
  files[*count] = malloc(strlen(file_name) + 1); // +1 for '\0'
  if (files[*count] == NULL) {
    perror("Memory allocation for file name failed");
    exit(EXIT_FAILURE); // Exit if malloc fails.
  }
  strcpy(files[*count], file_name); // Copy the file name.
  (*count)++;                       // Increment file count.

  return files; // Return the updated array.
}

/*
{
  "KeyID"  : "Song name can separate with space",
  "KeyID2" : "Song name can separate with space",
  "KeyID3" : "Song name can separate with space",
  "KeyID4" : "Song name can separate with space",
  "KeyID5" : "Song name can separate with space",
}
*/

// Function to list files in a directory and store them in an array.
char **list_files(const char *path, int *file_count) {
  struct dirent *entry;
  DIR *dir = opendir(path); // Open the directory

  if (dir == NULL) {
    perror("Unable to open directory");
    *file_count = 0;
    return NULL;
  }

  int capacity = 10; // Initial capacity for the array.
  char **files = malloc(capacity * sizeof(char *));
  if (files == NULL) {
    perror("Memory allocation failed");
    closedir(dir);
    *file_count = 0;
    return NULL;
  }

  *file_count = 0; // Initialize the file count.

  // Read directory entries.
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) { // Only regular files.
      // printf("Adding file: %s\n", entry->d_name);  // Print each file name.
      files = add_file_to_array(files, file_count, &capacity, entry->d_name);
    }
  }

  closedir(dir); // Close the directory.
  files = realloc(files, (*file_count + 1) * sizeof(char *));

  if (files == NULL) {
    perror("reallocation failed null char");
  }

  files[*file_count] = NULL;
  return files; // Return the array of file names.
}

// Function to free the memory allocated for the array.
void free_file_array(char **files, int count) {
  for (int i = 0; i < count; i++) {
    free(files[i]); // Free each string.
  }
  free(files); // Free the array of pointers.
}

char *extract_first_line(char *buffer, int line_length) {
  char *result_str = malloc(line_length);

  strncpy(result_str, buffer, line_length);
  result_str[line_length] = '\0';

  return result_str;
}