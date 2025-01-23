#ifndef DBFNC_H
#define DEFNC_H
#define DB_PATH "file_data.db"
void init_db();
void save_to_db(const char *key, const char *value);
char *get_from_db(const char *key);
char *display_all_files();
#endif