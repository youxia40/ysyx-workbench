#ifndef _db_h
#define _db_h

#define DB_FILE "~/ysyx-workbench/LCTHW/devpkg/db"
#define DB_DIR "~/ysyx-workbench/LCTHW/devpkg"


int DB_init();
int DB_list();
int DB_update(const char *url);
int DB_find(const char *url);

#endif
