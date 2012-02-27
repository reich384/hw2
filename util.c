/************************
 * util.c
 *
 * utility functions
 *
 ************************/

#include "util.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/***************
 * These functions are just some handy file functions.
 * We have not yet covered opening and reading from files in C,
 * so we're saving you the pain of dealing with it, for now.
 *******/
FILE* file_open(char* filename) {
	FILE* fp = fopen(filename, "r");
	if(fp == NULL) {
		fprintf(stderr, "Error opening file %s, abort.\n", filename);
		exit(1);
	}
	return fp;
}

/*******************
 * To use this function properly, create a char* and malloc 
 * 160 bytes for it. Then pass that char* in as the argument
 ******************/
char* file_getline(char* buffer, FILE* fp) {
	buffer = fgets(buffer, 160, fp);
	return buffer;
}

//Kludge: This gives me a "multiple definition" error if I put it in the
//.h file, so I'm putting it here.
target_t* head = NULL;

//Separate functions for target nodes because they have two extra pointers.
/* 	Add a new node to the linked list with the specified data.
	The new node is added to the end of the list.
	Returns pointer to new node.
*/
target_t* add_target(char* data){
	if (head == NULL){
		// Create the very first node
		head = malloc(sizeof(target_t));
		head->data = malloc(sizeof(char)*(strlen(data)+1));
		strncpy(head->data, data, (strlen(data)+1));
		head->dependency_head = NULL;
		head->command_head = NULL;
		head->next = NULL;
		return head;
	}
	else {
		target_t* node = head;
		//Find end of list
		while (node->next != NULL)
			node = node->next;
		//Allocate new node and move pointer to it
		node->next = malloc(sizeof(target_t));
		node = node->next;
		//Initialize new node
		node->data = malloc(sizeof(char)*(strlen(data)+1));
		strncpy(node->data, data, (strlen(data)+1));
		node->dependency_head = NULL;
		node->command_head = NULL;
		node->next = NULL;
		return node;
	}
}

target_t* search_target(char* data){
	target_t* node = head;
	while(node != NULL){
        //Search through the list and look for the node with the specified data.If found, return 0.
        if (strcmp(data, node->data) == 0)
			return node;
		node = node->next;
	}
	//printf("Did not find %s\n", data);
	return NULL;
}

/* 	Add a new node to the linked list with the specified data.
	The new node is added to the end of the list.
	Returns new list (pointer to first node).
*/
linked_list* list_add(linked_list* list, char* data){
	if (list == NULL){
		// Create the very first node
		list = malloc(sizeof(linked_list));
		list->data = malloc(sizeof(char)*(strlen(data)+1));
		strncpy(list->data, data, (strlen(data)+1));
		list->next = NULL;
	}
	else {
		linked_list* node = list;
		//Find end of list
		while (node->next != NULL)
			node = node->next;
		//Allocate new node and move pointer to it
		node->next = malloc(sizeof(linked_list));
		node = node->next;
		//Initialize new node
		node->data = malloc(sizeof(char)*(strlen(data)+1));
		strncpy(node->data, data, (strlen(data)+1));
		node->next = NULL;
	}
	return list;
}

linked_list* list_search(linked_list* list, char* data){
	linked_list* node = list;
	while(node != NULL){
        //Search through the list and look for the node with the specified data.
        if (strcmp(data, node->data) == 0)
			return node;
		node = node->next;
	}
	//printf("Did not find %s\n", data);
	return NULL;
}

// makeargv
/* Taken from Unix Systems Programming, Robbins & Robbins, p37 */

int makeargv(const char *s, const char *delimiters, char ***argvp) {
   int error;
   int i;
   int numtokens;
   const char *snew;
   char *t;

   if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
      errno = EINVAL;
      return -1;
   }
   *argvp = NULL;
   snew = s + strspn(s, delimiters);
   if ((t = malloc(strlen(snew) + 1)) == NULL)
      return -1;
   strcpy(t,snew);
   numtokens = 0;
   if (strtok(t, delimiters) != NULL)
      for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++) ;

   if ((*argvp = malloc((numtokens + 1)*sizeof(char *))) == NULL) {
      error = errno;
      free(t);
      errno = error;
      return -1;
   }

   if (numtokens == 0)
      free(t);
   else {
      strcpy(t,snew);
      **argvp = strtok(t,delimiters);
      for (i=1; i<numtokens; i++)
         *((*argvp) +i) = strtok(NULL,delimiters);
   }
   *((*argvp) + numtokens) = NULL;
   return numtokens;
}

void freemakeargv(char **argv) {
   if (argv == NULL)
      return;
   if (*argv != NULL)
      free(*argv);
   free(argv);
}



