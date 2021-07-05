/*
*	pfind_utils.c
*	by Christopher Fuentes
*	cfuentes@g.harvard.edu
*	HUID 60857326
* 
*	This is a utility file for pfind, providing functions 
*	to handle error printing, matching expressions, and
*	parsing command line arguments. 
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include "pfind_utils.h"

#define USAGE 			"dirname [-name {string|exp}] [-type {f|d|b|c|p|l|s}]"
#define ERR_MSG_SIZE 	512
#define VALID_TYPES 	"fdbcpla\0"

void extract_flags(int ac, char **av, Args *argsp);
char validate_filetype(char *typesstring);

/*
* 	Uses fnmatch to match filename against a query string 'findme'
*/
BOOL match_exp(char *filename, char *findme) {
	if (findme != DEFAULT_ARGS.findme) /*Only match qstring if specified */
		if (fnmatch(findme, filename, 0) == FNM_NOMATCH) 
			return NO;
	return YES;
}

/* Helper function to parse out command line args and check 
	number of arguments */
void parse_args(int ac, char **av, Args *argsp) {
	/* Initialize argsp */
	argsp->dirname 	= DEFAULT_ARGS.dirname;
	argsp->findme 	= DEFAULT_ARGS.findme;
	argsp->type 	= DEFAULT_ARGS.type;

	/* Program expects program name and dirname at a minimum to start*/
	int num_args_expected = 2;

	if (ac < 2) { die(YES, "Starting directory must be specified"); }

	/* Try to get the qstring and type. */
	extract_flags(ac, av , argsp);

	/*	Now we check that the total number of arguments supplied is logical.
		We already want at least two (progname and dirname).
		For each flag pair, two more are expected. E.g., -name <name> */
	num_args_expected += (argsp->findme == NULL ? 0 : 2) + 
							(argsp->type == '\0' ? 0 : 2);

	/* getopt() already checks for missing required flag values, so we can 
	attributed wrong # of args to wrong # of dirnames specified */
	if (ac != num_args_expected) 
		die(YES, "Must specify exactly one directory"); 
}

/* Helper-helper function to extract qstring and type from flags
	and store them in an Args */
void extract_flags(int ac, char **av, Args *argsp) {
	argsp->dirname = *++av;

	char *arg;
	while ( (arg = *++av) != NULL ) {		
		if (arg[0] == '-') {
			if (strcmp("-name", arg) == 0) {
				if (argsp->findme != DEFAULT_ARGS.findme) 
					die(YES, "Syntax Err: -name can only be specified once");
				if (*(av + 1) == NULL)
					die(NO, "missing argument to `-name'");
				argsp->findme = *++av;
			} else if (strcmp("-type", arg) == 0) {
				if (argsp->type != DEFAULT_ARGS.type) 
					die(YES, "Syntax Err: -type can only be specified once");
				if (*(av + 1) == NULL) 
					die(NO, "missing argument to `-type'");
				argsp->type = validate_filetype(*++av);
			} else {
				die(NO, "unknown predicate `%s'", arg);
			}
		} else die(YES, "paths must precede expression: %s", arg);
	}
}

/* Checks -type argument (if any) and validates that it is 
	exactly 1 character fromt he supported list, VALID_TYPES */
char validate_filetype(char *typestring) {
	/*
	* 	getopt_long_only would throw an error if -type was specified
	*	but no value was given. Therefore, the string length must be
	*	at least 1. 
	*/
	char type;
	if (strlen(typestring) > 1) {  
		die(NO, "Arguments to -type should contain only one letter");
	} else { 
		type = typestring[0]; 
		int i = 0, 
			t;
		char *valid = VALID_TYPES;
		while ( (t = valid[i++] ) != '\0') {
			if (type == t) {
				return t;
			}
		}
		die(NO, "Unknown argument to -type: %c", type);
	}
	return '\0'; /*silence compiler warnings*/
}
/*
 * Gets the stat for a given path. Exit on failure. 
 */
void get_stat(char *path, Stat *stat) {
	if ( lstat(path, stat) == -1 ){
		pfind_perror("%s: `%s'", PROGNAME, path);
		exit(1);
	}
}

/* Given a mode_t from a struct stat, returns {f|d|b|c|p|l|s}. 
	Exits program on error*/
char filetype(mode_t mode) {
	if (S_ISBLK(mode)) 	return 'b';
	if (S_ISCHR(mode)) 	return 'c';
	if (S_ISFIFO(mode)) return 'p';
	if (S_ISREG(mode)) 	return 'f';
	if (S_ISLNK(mode)) 	return 'l';
	if (S_ISSOCK(mode)) return 's';
	if (S_ISDIR(mode)) 	return 'd';
	die(YES, "Unrecognized filetype");
	return '\0';
}

/*
*	Helper function to print err message msg as format string with
*	arguments (as printf). Also prints USAGE, and exits.
*
*	@param print_usage 	Prints usage string if set to YES	
* 	@param msg_fmt 		Message format string, as in printf
*	@param ... 			Args to format strign, as in printf
*	@purpose 			Formats string with args, prints msg to stderr, 
*						prints USAGE to stdout, and exits.
*/
void die(BOOL print_usage, char *msg_fmt, ...) {
	char msg[ERR_MSG_SIZE];
	va_list vlist;
	va_start(vlist,msg_fmt);
	vsprintf(msg, msg_fmt, vlist);
	va_end(vlist);
	fprintf(stderr, "%s: %s\n", PROGNAME, msg);
	if (print_usage)
		printf("Usage: %s %s\n", PROGNAME, USAGE);
	exit(1);
}

/*
* 	Accepts a formats string, args, and prints the message
*	using perror. 
*/
void pfind_perror(char *fmt_str, ...){
	char msg[ERR_MSG_SIZE];
	va_list vlist;
	va_start(vlist, fmt_str);
	vsprintf(msg, fmt_str, vlist);
	va_end(vlist);
	perror(msg);
}

/* Helper function to print a struct args to stdout */
void print_args(Args *args) {
	printf("Dirname: %s\t Qry_String: %s\t Type: %c\t\n", 
				args->dirname, args->findme, args->type);
}