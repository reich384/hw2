/*******************************
 * maum.c
 *
 * Source code for MAUM
 *
 ******************************/

#include "util.h"
#include "maum.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

/*********
 * Simple usage instructions
 *********/
void maum_usage(char* progname) {
	fprintf(stderr, "Usage: %s [options] [target]\n", progname);
	fprintf(stderr, "-f FILE\t\tRead FILE as a maumfile.\n");
	fprintf(stderr, "-a Always execute all targets\n");
	fprintf(stderr, "-l FILE\t\tOutput results to specified log file, maum.log otherwise");
	fprintf(stderr, "-h\t\tPrint this message and exit.\n");
	fprintf(stderr, "-n\t\tDon't actually execute commands, just print them.\n");
	exit(0);
}

/****************************** 
 * this is the function that, when given a proper filename, will
 * parse the maumfile and read in the rules
 ***************/
void parse_file(char* filename) {
	char* line = malloc(160*sizeof(char));
	FILE* fp = file_open(filename);
	char* target_name = NULL;
	target_t* target = NULL;
	char* dependency_name = NULL;
	char* line_temp;
	while((line = file_getline(line, fp)) != NULL) {
		// this loop will go through the given file, one line at a time
		// this is where you need to do the work of interpreting
		// each line of the file to be able to deal with it later
		line_temp = strtok(line, "\n"); //Remove newline character at end if there is one
		if (line_temp == NULL) //skip if blank
			continue;
		if (*line_temp == '#') //skip if comment
			continue;
		//Remove leading whitespace
		while (*line_temp == ' ' || *line_temp == '\t')
			line_temp++;
		if (strlen(line_temp) <= 0) //skip if whitespace-only
			continue;
		//printf("Processing line: %s\n", line_temp);
		if (target == NULL)
		{
			//Not currently inside a target, look for a new one
			target_name = strtok(line_temp, ":");
			if (target_name != NULL && strlen(target_name) > 0)
			{
				//printf("New target: %s\n", target_name);
				target = add_target(target_name);
				dependency_name = strtok(NULL, " ");
				while (dependency_name != NULL)
				{
					target->dependency_head = list_add(target->dependency_head, dependency_name);
					//printf("New dependency: %s\n", dependency_name);
					dependency_name = strtok(NULL, " ");
				}
			}
		}
		else if (strncmp(line_temp, "end", 3) == 0)
		{
			//End of target's command list
			target = NULL;
			target_name = NULL;
		}
		else
		{
			//Add command to list
			target->command_head = list_add(target->command_head, line_temp);
			//printf("New command: %s\n", line_temp);
		}
	}
	fclose(fp);
	free(line);
}

void run(target_t* target,bool execute, char* logfile)
{
	//Check for dependencies and run them first if any are found.
	linked_list* dependency = target->dependency_head;
	target_t* target_sub;
	while (dependency != NULL)
	{
		target_sub = search_target(dependency->data);
		if (target_sub != NULL)
			run(target_sub, execute, logfile);
		else
			printf("Dependency missed or external dependency recognized: %s\n",dependency->data);
		dependency = dependency->next;
	}
	//Execute commands
	linked_list* command = target->command_head;
	int child_count;
	char** argv_child;
	char str[200];

	FILE *fp;



	while (command != NULL)
	{
		
		if (execute)
		{

			//Count number of children to fork and create list of
			//commands to execute in parallel
			child_count = 0;
			char* temp = strtok(command->data, ",");
			linked_list* parallel_cmds = NULL;
			while (temp != NULL)
			{
				parallel_cmds = list_add(parallel_cmds, temp);
				child_count++;
				temp = strtok(NULL, ",");
			}
	
			//Execute commands
			while (parallel_cmds != NULL)
			{
				if (fork() == 0)
				{

					//printf("Execute sub-command: %s\n", parallel_cmds->data);
					if (makeargv(parallel_cmds->data, " ", &argv_child) == -1) {
						perror("Error parsing command");
						exit(1);
					}

					//open log file to be written to
					if ((fp = fopen(logfile, "a+")) == NULL)
						perror("Failed to open log file");
					printf("%s\n",command->data);
					//str = "this is my string";
					strcpy(str,"\n>>Starting target: ");
					strcat(str,target->data);
					strcat(str,"\n\n");
					strcat(str,">>>>Running command: ");
					strcat(str,command->data);
					strcat(str,"\n\n\0");
					
					fputs ( str, fp);
					dup2(fileno(fp), 1);
					dup2(fileno(fp), 2);
					
					fclose(fp);				

					execvp(*argv_child, argv_child);
				}
			
				parallel_cmds = parallel_cmds->next;
			}

			//Wait for children
			while (child_count > 0)
			{
				int status;
				child_count--;
				wait(&status);
				//printf("wait() finished. child_count = %d\n", child_count);
			}
		}
		else
		
			printf("%s\n", command->data);
		command = command->next;
	}
}

int main(int argc, char* argv[]) {
	// Declarations for getopt
	extern int optind;
	extern char* optarg;
	int ch;
	char* format = "f:hnl:";
	
	// Variables you'll want to use
	char* filename = "Maumfile";
	char* logfile = "maum.log";
	bool execute = true;

	// Part 2.2.1: Use getopt code to take input appropriately.
	while((ch = getopt(argc, argv, format)) != -1) {
		switch(ch) {
			case 'f':
				filename = strdup(optarg);
				break;
			case 'n':
				execute = false;
				break;
			case 'h':
				maum_usage(argv[0]);
				break;
			case 'l':
				logfile = strdup(optarg);
				printf("Logging tripped: %s \n", logfile);
				break;
		}
	}
	argc -= optind;
	argv += optind;

	/* at this point, what is left in argv is the targets that were 
		specified on the command line. argc has the number of them.
		If getopt is still really confusing,
		try printing out what's in argv right here, then just running 
		maum with various command-line arguments. */

	parse_file(filename);

	/* after parsing the file, you'll want to execute all of the targets
		that were specified on the command line, along with their dependencies, etc. */
	
	if (argc == 0)
	{
		//No arguments, use first listed target
		if (head == NULL)
		{
			printf("Error: No targets in Maumfile.\n");
			exit(1);
		}
		else
			run(head, execute, logfile);
	}
	else
	{
		int i;
		target_t* target;
		for (i = 0; i < argc; i++)
		{
			target = search_target(argv[i]);
			if (target != NULL)
				run(target, execute, logfile);
			else
				printf("Error: No such target \"%s\"\n", argv[i]);
		}
	}

	return 0;
}
