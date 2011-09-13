extern void item_add(db_t *db, const char *item, const char *usr, const char *pwd);

extern const char *item_get_fld(db_t *db, const char *item, const char *field);
extern const char *item_get_usr(db_t *db, const char *item);
extern const char *item_get_pwd(db_t *db, const char *item);

extern void item_remove(db_t *db, const char *item);
