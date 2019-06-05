#ifdef __cplusplus
extern "C" {
#endif

/////////////// String functions ///////////////

#ifndef WIN32

#define FILEPATHSEPARATOR "/"

#else

#define FILEPATHSEPARATOR "\\"

#endif
char * str_upper(char * str);
char * str_lower(char * str);

/////////////// File functions ////////////////

int hxc_open (const char *filename, int flags, ...);

FILE *hxc_fopen (const char *filename, const char *mode);
int hxc_fread(void * ptr, size_t size, FILE *f);
char * hxc_fgets(char * str, int num, FILE *f);
int hxc_fclose(FILE * f);

#ifndef stat
#include <sys/stat.h>
#endif
int get_stat( const char *filename, struct stat *buf);

typedef struct filefoundinfo_
{
	int isdirectory;
	char filename[256];
	uint64_t size;
}filefoundinfo;

void * find_first_file(char *folder, char *file, filefoundinfo* fileinfo);
int find_next_file(void * handle,char *folder,char *file,filefoundinfo* fileinfo);
int find_close(void * handle);

int  hxc_mkdir(char * folder);

char * get_current_directory(char *currentdirectory,int buffersize);

char * get_file_name_base(char * fullpath,char * filenamebase);
char * get_file_name_ext(char * fullpath,char * filenameext);
int get_file_name_wext(char * fullpath,char * filenamewext);
int get_path_folder(char * fullpath,char * folder);
int check_file_ext(char * path,char *ext);
int get_file_size(char * path);

#ifdef __cplusplus
}
#endif
