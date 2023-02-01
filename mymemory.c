/* mymemory.c
 *
 * Provides interface to memory management
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "mymemory.h"

// Our memory
Byte mymemory[MAXMEM];
Segment_t *segmenttable = NULL;

void initialize()
{
	printf("Initialize> Start\n");

	// Set memory to 0
	memset(mymemory, '\0', MAXMEM);

	// Create segment table that contains one segment description that declares the whole memory as one free segment
	// Create a single segment descriptor
	segmenttable = (Segment_t *)malloc(sizeof(Segment_t));

	// Initialise the segment
	segmenttable->allocated = 0;
	segmenttable->start = mymemory;
	segmenttable->size = MAXMEM;
	segmenttable->next = NULL;

	// Print the initial segment discriptor 
	printsegmentdescriptor(segmenttable);

	printf("Initialize> End\n");
}

void *mymalloc(size_t size)
{
	printf("\nMyMalloc> Start\n");

	// Find a free segment in the segment table
	Segment_t *ptrOldSeg = findFree(segmenttable, size);

	// If there was no free segment, then return 0
	if (ptrOldSeg == NULL)
	{
		printf("No free memory.");
		return 0;
	}
	// If a free segment was found, then allocate the memory request to it
	else
	{
		// Create a new segment descriptor and allocate memory to it
		Segment_t *ptrNewSeg = (Segment_t *)malloc(sizeof(Segment_t));

		// Link the new and old segment together
		insertAfter(ptrOldSeg, ptrNewSeg);

		// Initialise the new segment
		ptrNewSeg->start = ptrOldSeg->start + size;
		ptrNewSeg->size = ptrOldSeg->size - size;
		ptrOldSeg->size = size;

		printf("MyMalloc> End\n");

		return ptrOldSeg->start;
	}
}

// Helper functions for management segmentation table
Segment_t *findFree(Segment_t *list, size_t size)
{
	printf("FindFree> Start\n");

	// Create a pointer to the current segment and set it to the first element in the segment table
	Segment_t *ptrCurrSeg = list;

	// Loop through the list of segment discriptors and find a non-allocated segment
	while (ptrCurrSeg != NULL)
	{
		// If free segment, then see if segment is large enough.
		if (ptrCurrSeg->allocated == 0)
		{
			// If segment is large enough, return that segment.
			if (ptrCurrSeg->size >= size)
			{
				printf("FindFree> End\n");
				return ptrCurrSeg;
			}
		}
		// Otherwise, keep looking for a segment
		ptrCurrSeg = ptrCurrSeg->next;
	}
	printf("FindFree> Failed\n");
	return NULL;
}

void insertAfter(Segment_t *oldSegment, Segment_t *newSegment)
{
	// Update the new free segment (new segment) 
	newSegment->allocated = 0;
	newSegment->next = NULL;
	
	// Update the original segment (old segment) with requested memory
	oldSegment->allocated = 1;
	oldSegment->next = newSegment;
}

Segment_t * findSegment ( Segment_t * list, void * ptr )
{
	printf("FindSegment> Start\n");
	
	// Create a pointer to the current segment and set it to the first element in the segment table
	Segment_t *ptrCurrSeg = list;
	
	// Loop through the list of segment discriptors and find the desired segment
	while (ptrCurrSeg != NULL)
	{
		// Find the desired segment and return it
		if (ptrCurrSeg->start == ptr) 
		{
			printf("FindSegment> End\n");
			return ptrCurrSeg;	
		}
		// Otherwise, keep looking for the segment
		ptrCurrSeg = ptrCurrSeg->next;
	}
	printf("FindSegment> Failed\n");
	return NULL;
}

void myfree ( void * ptr )
{
  printf ( "\nMyFree> Start\n");
	
	// Find the segment to be freed
	Segment_t *ptrFreeSeg = findSegment(segmenttable, ptr);
	
	// If the segment doesn't exist, return NULL
	if (ptrFreeSeg == NULL) 
	{
		printf ( "MyFree> Failed\n");
	}

	// Deallocate the segment so that future data can be written to it
	ptrFreeSeg->allocated = 0;
	
	printf ( "MyFree> End\n");
}

void mydefrag ( void ** ptrlist)
{
  printf ( "\nMyDefrag> Start\n");
	
	// Run through array and swop allocated segment with unallocated segment
	Segment_t *ptrCurrSeg = segmenttable;
	
	while (ptrCurrSeg != NULL)
	{
		// If the current segment is unallocated and the next segment is allocated, swop them
		if ((ptrCurrSeg->allocated == 0) && (ptrCurrSeg->next->allocated == 1))
		{
			// Store the old start value of the allocated data segment
			void *oldAlloStart = ptrCurrSeg->next->start;

			// Move the pointers from the allocated segment into the unallocated segment
			void *ptrSeg = moveSegment(segmenttable, ptrCurrSeg);
			// OR moveSegment(segmenttable, ptrCurrSeg->start)

			// Run through the ptrlist - a list of allocated segments
			for (int i = 0; i < sizeof(void *); i++)
			{
				// Update the old mymemory pointer to the new start of the allocated data
				if (ptrlist[i] == oldAlloStart)
				{
					ptrlist[i] = ptrSeg;
					// Redirect allocated data segment to unallocated data segment
					memcpy(ptrSeg, oldAlloStart, ptrCurrSeg->size);
					// Redirect unallocated data segment to allocated data segment
					memset(ptrCurrSeg->next->start, '\0', ptrCurrSeg->next->size);
				}
			}
		}
		// If consecutive segments are unallocated, then combine them
		else if ((ptrCurrSeg->allocated == 0) && (ptrCurrSeg->next->allocated == 0)) 
		{
			// Update the start and size of the next unallocated segment, to be that of the current segment
			ptrCurrSeg->next->start = ptrCurrSeg->start;
			ptrCurrSeg->next->size = ptrCurrSeg->size + ptrCurrSeg->next->size;
			
			// Find the previous segment and set its next to the combined segment
			Segment_t *ptrPrevSeg = segmenttable;
			while (ptrPrevSeg != NULL)
			{
				if (ptrPrevSeg->next == ptrCurrSeg) {
					ptrPrevSeg->next = ptrCurrSeg->next;
				}
				ptrPrevSeg = ptrPrevSeg->next;
			}

			// Delete the old unallocated segment
			delSegment(segmenttable, ptrCurrSeg);		
			// OR delSegment(segmenttable, ptrCurrSeg->start)
		}

		// If the end of the table is next, then unallocated segments are combined
		if (ptrCurrSeg->next->next == NULL) 
		{
			break;
		}
		// Otherwise keep combining
		else {
			ptrCurrSeg = ptrCurrSeg->next;
		}
	}
	printf ( "MyDefrag> End\n");
}

// Deletes a segment given by a pointer to a segment descriptor
int delSegment ( Segment_t * list, Segment_t * segment )
{
	printf ( "\nDelSegment> Start\n");
	// If the segment doesn't exist, return NULL
	if (segment == NULL) 
	{
		printf ( "DelSegment> Failed\n");
		return 0;
	}

	// Clear memory so that data is no longer stored
	memset(segment->start, '\0', segment->size);
	
	// Deallocate that segment
	myfree(segment->start);
	
	printf ( "DelSegment> End\n");
	return 1;
}

// Moves a segment given by a pointer to a segment descriptor
void * moveSegment ( Segment_t * list, Segment_t * segment ) 
{
	printf ( "MoveSegment> Start\n");
	// If the segment doesn't exist, return NULL
	if (segment == NULL) 
	{
		printf ( "MoveSegment> Failed\n");
		return NULL;
	}
	
	// Storing the orinal size of the unallocated segment
	int unalloSize = segment->size;
	
	// Switch the values of allocated segment and unallocated segment
	segment->allocated = 1;
	segment->size = segment->next->size;
	segment->next->allocated = 0;
	// Update the unallocated segments start address
	segment->next->start = segment->start + segment->size;
	segment->next->size = unalloSize;
	
	printf ( "MoveSegment> End\n");
	return segment->start;
}

/*
// Moves a segment given by a memory address and moves the content in memory
void * moveSegment ( Segment_t * list, void * ptr )
{
	printf ( "\nMoveSegment> Start\n");
	
	// Find the segment to be moved
	Segment_t *segment = findSegment(list, ptr);

	// If the segment doesn't exist, return NULL
	if (segment == NULL) 
	{
		printf ( "MoveSegment> Failed\n");
		return NULL;
	}

	// Storing the orinal size of the unallocated segment
	int unalloSize = segment->size;
	
	// Switch the values of allocated segment and unallocated segment
	segment->allocated = 1;
	segment->size = segment->next->size;
	segment->next->allocated = 0;
	// Update the unallocated segments start address
	segment->next->start = segment->start + segment->size;
	segment->next->size = unalloSize;
	
	printf ( "MoveSegment> End\n");
	return segment->start;
}
*/
/*
// Deletes a segment given by a memory address
int delSegment ( Segment_t * list, void * ptr )
{
	printf ( "\nDelSegment> Start\n");

	// Find the segment to delete
	Segment_t *ptrDelSeg = findSegment(list, ptr);

	// If the segment doesn't exist, return NULL
	if (ptrDelSeg == NULL) 
	{
		printf ( "\nDelSegment> Failed\n");
		return 0;
	}

	// Clear memory so that data is no longer stored
	memset(ptrDelSeg->start, '\0', ptrDelSeg->size);

	// Deallocate that segment
	myfree(ptrDelSeg->start);

	printf ( "\nDelSegment> End\n");
	return 1;
}	
*/

int isPrintable ( int c )
{
  if ( c >= 0x20 && c <= 0x7e ) return c ;
  return 0 ;
}

void printmemory()
{
	// Traverse through every byte in my memory
	for (int i = 0; i < MAXMEM; i++)
	{
		// Monitor if we need to create a new line
		if (i % 10 == 0)
		{
			// Before we make a new line, print the char from the last 10 bytes, but not if we're working with the first byte of memory
			if (i != 0)
			{
				printf("\t| ");
				// Run through the past 10 elements
				for (int j = i - 10; j < i; j++)
				{
					// Check if char is printable. If return = 0, then print '.'. If return = c, then print.
					int value = isPrintable(mymemory[j]);
					if (value == 0)
					{
						printf(".");
					}
					else
					{
						printf("%c", (char)value);
					}
				}
			}
			// Print the new line
			printf("\n[%d]\t", i);
		}
		// Print the hex of memory byte
		printf("%02x ", mymemory[i]);
	}
}

void printsegmenttable()
{
	// Create a pointer to the current segment and set it to the first element in segmenttable
	Segment_t *ptrSeg = segmenttable;
	//  Track which segment in the table you are working with
	int count = 0;
	// Loop through the list of segment discriptors and print them
	while (ptrSeg != NULL)
	{
		printf("Segment %d:\n", count);
		printsegmentdescriptor(ptrSeg);
		// Set the pointer to the next segment discriptor
		ptrSeg = ptrSeg->next;
		count++;
	}
}

void printsegmentdescriptor ( Segment_t * descriptor )
{
	printf ( "\tallocated = %s\n" , (descriptor->allocated == FALSE ? "FALSE" : "TRUE" ) ) ;
	printf ( "\tstart     = %p\n" , descriptor->start ) ;
	printf ( "\tsize      = %lu\n", descriptor->size  ) ;
}
