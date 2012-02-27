/**************************
 * dime.h -- the header file for dime.c, a 
 * distributed make
 *
 *
 *
 *************************/

#ifndef _DIME_H_
#define _DIME_H_

#define true 1
#define false 0
typedef int bool;

void dime_usage(char*);
void parse_file(char*);

extern target_t* head;

#endif
