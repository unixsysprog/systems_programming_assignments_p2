/*
* 	pfind_utils.h
*	by Christopher Fuentes
*	cfuentes@g.harvard.edu
*	HUID 60857326
*
*	This file contains some common definitions/utilities for 
* 	pfind, including argument parsing, error message printing, 
*	and matching files against command line conditions. 
*/

#include <sys/stat.h>
#include <dirent.h>

/* Typedefs for convenience */
typedef struct stat 	Stat;
typedef struct dirent 	Dirent;
typedef struct args { char *dirname; char *findme; char type; } Args;
typedef enum { NO = 0, YES } BOOL; /*obj-c style BOOL*/

#define DEFAULT_ARGS 	(Args){ NULL, NULL, '\0'}
#define PROGNAME 		"pfind"

/* Parses command line arguments and places them in the argsp */
void parse_args(int ac, char **av, Args *argsp);

/* Given a mode_t, returns the filetype from {f|d|b|c|p|l|s}  */
char filetype(mode_t mode);

/* Gets the stat info for a given path */
void get_stat(char *path, Stat *stat);

/* Debugging function for printing args to stdout */
void print_args(Args *args);

/* Matches a dirname against a pattern */
BOOL match_exp(char *dirname, char *findme);

/* Prints a message and exits. Optionally prints USAGE string */
void die(BOOL print_usage, char *msg_fmt, ...);

/* Prints the formatted string using perror */
void pfind_perror(char *fmt_str, ...);
