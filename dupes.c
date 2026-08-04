// Version 1
// Added the range and zero-search options

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ftw.h>
#include <unistd.h>
#include <libgen.h>

// Define Maximum Number of Files.  Currently 5 Billion
#define maximum_number_of_files 5000000

char *directory_name[maximum_number_of_files];
char *file_name[maximum_number_of_files];
long file_size[maximum_number_of_files];
int filenum = 0;
char *file_name[];
int hit = 0;
int delete = 0;
int compnum = 0;
int tolerance = 0;
unsigned long int pass = 0;

void help(void);
char *get_current_directory(void);

int get_file(const char *base_name, const struct stat *info, const int typeflag,
             struct FTW *pathinfo);

int get_file_list(char *pwd);

int check_existing(char *dirc, char *base_name, uintptr_t tfilesize);

int add_file_to_memory(char *dirc, char *base_name, uintptr_t tfilesize);

int main(int argc, char *argv[]) {

    printf("\n");
    compnum = 20;

    int compare = compnum;

    if (argv[1]) {

        if (strstr(argv[1], "-h")) {
            help();
        } else if (strstr(argv[1], "-d")) {
            delete = 1;
        } else {
            compare = atoi(argv[1]);
            if (compare < 1) {
                compare = 0;
            }
            compnum = compare;
        }

    }
    if (argv[2]) {

        if (strstr(argv[2], "-d")) {
            delete = 1;
        } else {
            tolerance = atoi(argv[2]);
        }
    }

    if (argv[3] && strstr(argv[3], "-d")) {

        delete = 1;

    }

    char *pwd = get_current_directory();
    get_file_list(pwd);
}

int get_file_list(char *pwd) {
    DIR *d;
    struct dirent *dir;

    d = opendir(pwd);
    if (d) {

        nftw(pwd, get_file, FTW_D, FTW_PHYS);
    }
    closedir(d);
    return (0);
}

char *get_current_directory() {
    char *pwd = getenv("PWD");

    return pwd;
}

int get_file(const char *base_name, const struct stat *info, const int typeflag,
             struct FTW *pathinfo) {
    const char *path = base_name;
    char *basec, *current_filename;

    basec = strdup(path);
    current_filename = basename(basec);
    uintptr_t tfilesize = info->st_size;

    if (typeflag == FTW_F && current_filename[0] != '.') {

        add_file_to_memory(basec, current_filename, tfilesize);
    }
    return 0;
}

int add_file_to_memory(char *dirc, char *base_name, uintptr_t tfilesize) {
    int exists = 0;

    pass++;
    if (pass > 1) {
        exists = check_existing(dirc, base_name, tfilesize);
    }
    if (exists == 0) {
        filenum++;
        directory_name[filenum] = malloc(255 * sizeof(char));
        file_name[filenum] = malloc(255 * sizeof(char));
        file_size[filenum] = tfilesize;
        directory_name[filenum] = dirc;
        strcpy(file_name[filenum], base_name);
    }
    return 0;
}

int check_existing(char *dirc, char *base_name, uintptr_t tfilesize) {
    int i = 0;
    int existing = 0;
    int low = 0;
    int high = 0;

    for (i = 1; i < filenum + 1; i++) {
        if (strncmp(base_name, file_name[i], compnum) == 0) {
            int comparison = file_size[i];

            if (tolerance < 1) { tolerance = 1; }
            low = comparison - tolerance;
            high = comparison + tolerance;


            if (tfilesize > low && tfilesize < high) {
                hit++;
                int myfilesize = tfilesize;

                existing++;

                printf("%da) %s %ld\n", hit, directory_name[i], file_size[i]);

                if (delete == 1) {
                    printf("***** DELETED *****> %db) %s %d <***** DELETED *****", hit, dirc, myfilesize);
                    unlink(dirc);
                } else {

                    printf("%db) %s %d ", hit, dirc, myfilesize);
                }
                printf("\n\n");
            }
        }
    }
    return existing;
}


void help(void) {

    printf("\nDupes: This program finds duplicate files in a directory tree.  It matches both filenames and file sizes when determining duplicates\n\nMatching these values verses generating hashes makes the program extremely fast. It can scan tens of thousands of files in seconds.\n\n");
    printf("\nUsage:  dupes [number of characters to match from beginning] [number of bytes of leeway] [--delete]\n\n");
    printf("Example:  dupes 10 50 --delete\n\n");
    printf("The above command will delete all repetitive files that match the first 10 characters of each other, and have a file size within 50 bytes of each other\n");
    printf("\nNote: The default values are 20 and 0, meaning the first 20 characters of a filename must match, and the files must have the exact same size.  Filenames less than 20 characters long must match their entire filenames\n\n");

    exit(0);


}
