#ifndef STDFNC_H
#define STDFNC_H
#include <stddef.h> // For size_t
unsigned char *read_html(const char *file_name, size_t *size);
unsigned char *read_audio(const char *file_name, size_t *size);
char *base64_encode(const unsigned char *data, size_t input_length,
                    size_t *output_size);
char *extract_file_id   (char *str);
int verify_audio_route(char *str);
char *extract_first_line(char *buffer, int line_len);
// char *convert_to_json(char **names);
char **list_files(const char *path, int *file_count);
// char *generate_file_id_json();
void free_file_array(char **files, int count);
#endif


/*

char *generate_file_id_json(char **names) {

  char *json = malloc(2);
  strcpy(json, "{");
  for (int i = 0; names[i] != NULL; i++) {
    char id[ID_LENGTH + 1];
    do {
      generate_random_id(id);
    } while (!is_unique_id(id));
    char *id_copy = strdup(id);
    if (unique_id_count >= MAX_FILES) {
      // Reset the array by setting the count back to zero
      unique_id_count = 0;
    }

    strcpy(unique_ids[unique_id_count++], id_copy);
    char *name = strdup(names[i]);
    int len = strlen(id_copy) + strlen(name) + 9;
    char *OBJ_element = malloc(len);
    snprintf(OBJ_element, len, "\"%s\" : \"%s\",", id_copy, name);
    save_to_db(id_copy, name);
    free(id_copy);
    free(name);

    json = realloc(json, strlen(json) + len + 1);
    if (NULL == json) {
      perror("reallocatioon failed");
      exit(EXIT_FAILURE);
    }
    strcat(json, OBJ_element);
    free(OBJ_element);
  }
  json[strlen(json) - 1] = '}';
  return json;
  // green();
  // printf("json : %s\n", json);
  // reset();
}

char *convert_to_json(char **names) {
  // Start with the opening bracket for the JSON array
  char *json = malloc(2); // To store the opening '[' and '\0'
  strcpy(json, "[");

  // Iterate through the names array
  for (int i = 0; names[i] != NULL; i++) {
    // Allocate space for the string element (with quotes and comma)
    char *escaped_name =
        names[i]; // Assuming names are safe (no escaping required)
    size_t len = strlen(escaped_name) + 4; // Quotes + comma + \0
    char *entry = malloc(len);

    // Format: "name",
    snprintf(entry, len, "\"%s\",", escaped_name);

    // Reallocate the JSON string to append the new entry
    json = realloc(json, strlen(json) + len + 1);
    if (NULL == json) {
      perror("json memory allocation failed");
      exit(EXIT_FAILURE);
    }
    strcat(json, entry); // Append the entry to the JSON string

    free(entry); // Free the temporary entry string
  }

  // Replace the last comma with the closing bracket ']'
  json[strlen(json) - 1] = ']';

  return json;
}

*/