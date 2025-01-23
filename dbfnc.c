#include "dbfnc.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to initialize the database and table
void init_db() {
  sqlite3 *db;
  char *err_msg = 0;
  int rc = sqlite3_open(DB_PATH, &db);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }

  const char *sql =
      "CREATE TABLE IF NOT EXISTS files(key TEXT PRIMARY KEY, value TEXT);";
  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
  }

  sqlite3_close(db);
}

// Function to save a key-value pair to the database
void save_to_db(const char *key, const char *value) {
  sqlite3 *db;
  sqlite3_open(DB_PATH, &db);

  const char *sql = "INSERT OR REPLACE INTO files(key, value) VALUES (?, ?);";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

  sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, value, -1, SQLITE_STATIC);

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

// Function to retrieve a value by key from the database
char *get_from_db(const char *key) {
  sqlite3 *db;
  sqlite3_open(DB_PATH, &db);

  const char *sql = "SELECT value FROM files WHERE key = ?;";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

  sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
  char *value = NULL;

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char *db_value = sqlite3_column_text(stmt, 0);
    value = strdup((const char *)db_value); // Copy the result
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return value; // Caller is responsible for freeing the returned value
}

char *display_all_files() {
  sqlite3 *db;
  int rc = sqlite3_open(DB_PATH, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    return NULL;
  }
  const char *sql = "SELECT key, value FROM files;";
  sqlite3_stmt *stmt;

  // Prepare the SQL statement
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return NULL;
  }

  // printf("All key-value pairs in the database:\n");

  char *json = malloc(2);
  if (json == NULL) {
    perror("Initial allocation for json failed");
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return NULL;
  }
  strcpy(json, "{");
  // Execute the statement and iterate over each row
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char *key = sqlite3_column_text(stmt, 0);
    const unsigned char *value = sqlite3_column_text(stmt, 1);
    int len = strlen((const char *)key) + strlen((const char *)value) + 9;
    char *OBJ_element = malloc(len);
    if (OBJ_element == NULL) {
      perror("Allocation for OBJ_element failed");
      free(json);
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      return NULL;
    }
    snprintf(OBJ_element, len, "\"%s\" : \"%s\",", key, value);

    char *temp = realloc(json, strlen(json) + len + 1);
    if (NULL == temp) {
      perror("Reallocation failed");
      free(json);
      free(OBJ_element);
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      return NULL;
    }
    json = temp;
    strcat(json, OBJ_element);
    free(OBJ_element);

    // printf("Key: %s, Value: %s\n", key, value);
  }
  size_t json_len = strlen(json);
  if (json_len > 1 && json[json_len - 1] == ',') {
    json[json_len - 1] = '}';
  } else {
    strcat(json, "}");
  }

  // Finalize the statement to avoid memory leaks
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return json;
}