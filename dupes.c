#define _XOPEN_SOURCE 700

#include "dupes.h"

#include <errno.h>
#include <ftw.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static FileList files = {0};
static size_t compare_chars = 20;
static off_t tolerance = 0;
static bool delete_duplicates = false;
static size_t duplicate_count = 0;
static size_t scanned_count = 0;

static void free_file_list(void);
static int visit_file(const char *path, const struct stat *info, int typeflag,
                      struct FTW *pathinfo);
static int process_file(const char *path, const char *name, off_t size);
static int append_file(const char *path, const char *name, off_t size);
static bool names_match(const char *left, const char *right);
static bool sizes_match(off_t left, off_t right);
static int parse_nonnegative(const char *text, uintmax_t maximum,
                             uintmax_t *value);

int main(int argc, char *argv[]) {
    const char *root = ".";
    int positional = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            help(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--delete") == 0) {
            delete_duplicates = true;
            continue;
        }

        uintmax_t value = 0;
        switch (positional) {
            case 0:
                if (parse_nonnegative(argv[i], SIZE_MAX, &value) != 0) {
                    fprintf(stderr, "Invalid character count: %s\n", argv[i]);
                    return EXIT_FAILURE;
                }
                compare_chars = (size_t)value;
                break;
            case 1:
                if (parse_nonnegative(argv[i], (uintmax_t)LLONG_MAX, &value) != 0) {
                    fprintf(stderr, "Invalid byte tolerance: %s\n", argv[i]);
                    return EXIT_FAILURE;
                }
                tolerance = (off_t)value;
                break;
            case 2:
                root = argv[i];
                break;
            default:
                fprintf(stderr, "Too many arguments.\n\n");
                help(argv[0]);
                return EXIT_FAILURE;
        }
        ++positional;
    }

    int walk_result = nftw(root, visit_file, 32, FTW_PHYS);
    if (walk_result != 0) {
        if (walk_result == -1) {
            fprintf(stderr, "Unable to scan '%s': %s\n", root, strerror(errno));
        } else {
            fprintf(stderr, "Scan stopped before completion.\n");
        }
        free_file_list();
        return EXIT_FAILURE;
    }

    printf("Scanned %zu files; found %zu duplicate%s.\n",
           scanned_count, duplicate_count, duplicate_count == 1 ? "" : "s");

    free_file_list();
    return EXIT_SUCCESS;
}

static int visit_file(const char *path, const struct stat *info, int typeflag,
                      struct FTW *pathinfo) {
    if (typeflag != FTW_F) {
        return 0;
    }

    ++scanned_count;
    const char *name = path + pathinfo->base;
    if (name[0] == '.') {
        return 0;
    }

    return process_file(path, name, info->st_size);
}

static int process_file(const char *path, const char *name, off_t size) {
    bool duplicate = false;

    for (size_t i = 0; i < files.count; ++i) {
        const FileEntry *existing = &files.items[i];
        if (!names_match(name, existing->name) || !sizes_match(size, existing->size)) {
            continue;
        }

        duplicate = true;
        ++duplicate_count;
        printf("%zua) %s (%jd bytes)\n", duplicate_count, existing->path,
               (intmax_t)existing->size);
        printf("%zub) %s (%jd bytes)%s\n\n", duplicate_count, path,
               (intmax_t)size, delete_duplicates ? " [deleting]" : "");

        if (delete_duplicates && unlink(path) != 0) {
            fprintf(stderr, "Could not delete '%s': %s\n", path, strerror(errno));
            return 0;
        }

        /* One file is reported once, against its earliest matching file. */
        break;
    }

    if (!duplicate) {
        return append_file(path, name, size);
    }
    return 0;
}

static int append_file(const char *path, const char *name, off_t size) {
    if (files.count == files.capacity) {
        size_t new_capacity = files.capacity == 0 ? 1024 : files.capacity * 2;
        if (new_capacity < files.capacity ||
            new_capacity > SIZE_MAX / sizeof(*files.items)) {
            fprintf(stderr, "Too many files to store in memory.\n");
            return 1;
        }

        FileEntry *new_items = realloc(files.items,
                                       new_capacity * sizeof(*files.items));
        if (new_items == NULL) {
            fprintf(stderr, "Out of memory while scanning files.\n");
            return 1;
        }
        files.items = new_items;
        files.capacity = new_capacity;
    }

    char *saved_path = strdup(path);
    char *saved_name = strdup(name);
    if (saved_path == NULL || saved_name == NULL) {
        free(saved_path);
        free(saved_name);
        fprintf(stderr, "Out of memory while saving '%s'.\n", path);
        return 1;
    }

    files.items[files.count++] = (FileEntry){
        .path = saved_path,
        .name = saved_name,
        .size = size,
    };
    return 0;
}

static bool names_match(const char *left, const char *right) {
    if (compare_chars == 0) {
        return strcmp(left, right) == 0;
    }
    return strncmp(left, right, compare_chars) == 0;
}

static bool sizes_match(off_t left, off_t right) {
    if (left >= right) {
        return (uintmax_t)(left - right) <= (uintmax_t)tolerance;
    }
    return (uintmax_t)(right - left) <= (uintmax_t)tolerance;
}

static int parse_nonnegative(const char *text, uintmax_t maximum,
                             uintmax_t *value) {
    if (text == NULL || *text == '\0' || *text == '-') {
        return -1;
    }

    errno = 0;
    char *end = NULL;
    uintmax_t parsed = strtoumax(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' || parsed > maximum) {
        return -1;
    }

    *value = parsed;
    return 0;
}

static void free_file_list(void) {
    for (size_t i = 0; i < files.count; ++i) {
        free(files.items[i].path);
        free(files.items[i].name);
    }
    free(files.items);
}

void help(const char *program_name) {
    printf("Dupes finds likely duplicate files in a directory tree by comparing "
           "filename prefixes and file sizes.\n\n");
    printf("Usage: %s [characters] [byte-tolerance] [directory] [--delete]\n\n",
           program_name);
    printf("Defaults:\n"
           "  characters      20\n"
           "  byte-tolerance  0 (exact size)\n"
           "  directory       current directory\n\n");
    printf("Examples:\n"
           "  %s\n"
           "  %s 10 50 /path/to/files\n"
           "  %s 10 50 /path/to/files --delete\n\n",
           program_name, program_name, program_name);
    printf("Warning: --delete permanently removes each later matching file.\n");
}
