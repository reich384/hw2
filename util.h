/********************
 * util.h
 *
 * You may put your utility function definitions here
 * also your structs, if you create any
 *********************/

#include <stdio.h>
#include <string.h>

// the following ifdef/def pair prevents us from having problems if 
// we've included util.h in multiple places... it's a handy trick
#ifndef _UTIL_H_
#define _UTIL_H_

// This stuff is for easy file reading
FILE* file_open(char*);
char* file_getline(char*, FILE*);

typedef struct node_sub{
	char* data;	
	struct node_sub* next;
}linked_list;

typedef struct node_target{
	char* data;	
	struct node_target* next;
	struct node_sub* dependency_head;
	struct node_sub* command_head;
}target_t;

target_t* add_target(char* data);
target_t* search_target(char* data);
target_t* first_target();

linked_list* list_add(linked_list* list, char* data);
linked_list* list_search(linked_list* list, char* data);

int makeargv(const char *s, const char *delimiters, char ***argvp);
void freemakeargv(char **argv);

#endif
