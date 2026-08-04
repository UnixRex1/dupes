#ifndef DUPES_H
#define DUPES_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct {
    char *path;
    char *name;
    off_t size;
} FileEntry;

typedef struct {
    FileEntry *items;
    size_t count;
    size_t capacity;
} FileList;

void help(const char *program_name);

#endif
