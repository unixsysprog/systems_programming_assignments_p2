#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include "pfind_utils.h"

void searchdir(Args *args);
void searchfile(char *filename, Args old_args);
void enumerate_dir_files(Args *args);

int main(int ac, char **av){
	Args args = DEFAULT_ARGS;
	parse_args(ac, av, &args);
	searchdir(&args);
	return 0;
}

void searchdir(Args *args) {
	Stat 	stat;
	char 	ftype, 	
			*base_name = basename(args->dirname);
	BOOL 	expmatch, typematch;

	get_stat(args->dirname, &stat);

	ftype 		= filetype(stat.st_mode);
	expmatch 	= match_exp(base_name, args->findme);
	typematch 	= (args->type == '\0' || args->type == ftype);

	if (expmatch && typematch)
		printf("%s\n", args->dirname);

	if (ftype == 'd') { /*it's a directory - recurse*/
		enumerate_dir_files(args);
	}
}


void enumerate_dir_files(Args *args) {
	DIR		*dir;		/* the directory */
	Dirent	*dirent;	/* each entry	 */

	if ( (dir = opendir(args->dirname) ) == NULL) {
		pfind_perror("%s: `%s'", PROGNAME, args->dirname);
		return;
	}

	while ( ( dirent = readdir( dir ) ) != NULL )
		searchfile(dirent->d_name, *args);

	if (dirent != NULL) { closedir( dir ); }
}

void searchfile(char *filename, Args old_args) {
	Args 	new_args;
	char 	*newpathname;
	int 	fn_len;

	if ( strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)  
		return;

	/*  fn_len = len(filename) + len(dirname) + 1 for '/' + 1 for '\0' */
	fn_len = strlen(filename) + strlen(old_args.dirname) + 2; 
	if ( (newpathname = malloc(sizeof(char) * fn_len)) == NULL )
		die(NO, "Unable to allocate %d bytes to hold filename", fn_len);

	sprintf(newpathname, "%s/%s", old_args.dirname, filename);

	/* Create new args struct with same findme and type but new dirname */
	new_args 			= old_args;
	new_args.dirname 	= newpathname;

	/* Recurse with new arguments */
	searchdir(&new_args);

	/* Avoid mem leak */
	free(newpathname);
}
