typedef void db_t;

extern db_t *db_load(const char *path);

extern const char *db_dump(db_t *db);

extern void db_sync(db_t *db, const char *path);
extern void db_unload(db_t *db);
