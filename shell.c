#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "mymemory.h"

int main () {
	printf("\nShell> Start\n");

	printf("\n------ INITIALIZATION -------\n");
	initialize();

	printf("\n------ SEGMENT TABLE BEFORE ASSIGNMENT -------\n");
	printsegmenttable();

	printf("\n------ MEMORY BEFORE ASSIGNMENT -------");
	printmemory();
	
	char *ptrlist[100];

	ptrlist[0] = (char *) mymalloc ( 10 ) ;
  strcpy (ptrlist[0], "This test1");
  printf( "shell> content of allocated memory: %s\n", ptrlist[0] ) ;
  
  ptrlist[1] = (char *) mymalloc ( 10 ) ;
  strcpy (ptrlist[1], "This test2");
  printf( "shell> content of allocated memory: %s\n", ptrlist[1] ) ;
  
  ptrlist[2] = (char *) mymalloc ( 10 ) ;
  strcpy (ptrlist[2], "This test3");
  printf( "shell> content of allocated memory: %s\n", ptrlist[2] ) ;
  
  ptrlist[3] = (char *) mymalloc ( 10 ) ;
  strcpy (ptrlist[3], "This test4");
  printf( "shell> content of allocated memory: %s\n", ptrlist[3] ) ;
  
  ptrlist[4] = (char *) mymalloc ( 10 ) ;
  strcpy (ptrlist[4], "This test5");
  printf( "shell> content of allocated memory: %s\n", ptrlist[4] ) ;
  
  ptrlist[5] = (char *) mymalloc ( 10 ) ;
  strcpy (ptrlist[5], "This test6");
  printf( "shell> content of allocated memory: %s\n", ptrlist[5] ) ;

	printf("\n------ SEGMENT TABLE AFTER ASSIGNMENT -------\n");
	printsegmenttable();

	printf("\n------ MEMORY AFTER ASSIGNMENT -------");
	printmemory();
	
	myfree(ptrlist[0]);
	myfree(ptrlist[2]);
	myfree(ptrlist[3]);

	printf("\n------ SEGMENT TABLE AFTER FREEING -------\n");
	printsegmenttable();

	printf("\n------ MEMORY AFTER FREEING -------");
	printmemory();

	mydefrag(ptrlist);

	printf("\n------ SEGMENT TABLE AFTER DEFRAGMENTATION -------\n");
	printsegmenttable();

	printf("\n------ MEMORY AFTER DEFRAGMENTATION -------");
	printmemory();

	//Change one of the strings to check pointers point to correct thing
  strcpy (ptrlist[4], "This 5");
  
	// Add more to memory to see if the linked list works
  ptrlist[6] = (char *) mymalloc ( 10 ) ;
  strcpy (ptrlist[6], "This test7");
  printf( "shell> content of allocated memory: %s\n", ptrlist[6] ) ;
  
	// Reassigning data to a defrag-ed pointer
  ptrlist[2] = (char *) mymalloc ( 9 ) ;
  strcpy (ptrlist[2], "rewrite2");
  printf( "shell> content of allocated memory: %s\n", ptrlist[2] ) ;

  // Test Printing
	printf("\n------ SEGMENT TABLE AFTER FINAL TESTING -------\n");
	printsegmenttable();
  printf("\n------ MEMORY AFTER FINAL TESTING -------");
  printmemory();

	printf("\nShell> End\n");

	return 0;
}