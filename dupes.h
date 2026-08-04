#ifndef DUPES_H_   
#define DUPES_H_

// Define Maximum Number of Files.  Currently 5 Billion
#define maximum_number_of_files 5000000000

char *get_current_directory (void);
int get_file (const char *base_name, const struct stat *info, const int typeflag,
	      struct FTW *pathinfo);
int get_file_list (char *pwd);
int check_existing (char *dirc, char *base_name, uintptr_t tfilesize);
int add_file_to_memory (char *dirc, char *base_name, uintptr_t tfilesize);

char *directory_name[maximum_number_of_files];
char *file_name[maximum_number_of_files];
long file_size[maximum_number_of_files];
int delete_files(char *full_name);
void help(void);

#endif // DUPES_H_
