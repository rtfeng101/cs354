///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2023 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission FALL 2023, CS354-deppeler
//
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "p3Heap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block.
 */
typedef struct blockHeader {           

    /*
     * 1) The size of each heap block must be a multiple of 8
     * 2) heap blocks have blockHeaders that contain size and status bits
     * 3) free heap block contain a footer, but we can use the blockHeader 
     *.
     * All heap blocks have a blockHeader with size and status
     * Free heap blocks have a blockHeader as its footer with size only
     *
     * Status is stored using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit 
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     * 
     * Start Heap: 
     *  The blockHeader for the first block of the heap is after skip 4 bytes.
     *  This ensures alignment requirements can be met.
     * 
     * End Mark: 
     *  The end of the available memory is indicated using a size_status of 1.
     * 
     * Examples:
     * 
     * 1. Allocated block of size 24 bytes:
     *    Allocated Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 25
     *      If the previous block is allocated p-bit=1 size_status would be 27
     * 
     * 2. Free block of size 24 bytes:
     *    Free Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 24
     *      If the previous block is allocated p-bit=1 size_status would be 26
     *    Free Block Footer:
     *      size_status should be 24
     */
    int size_status;

} blockHeader;         

/* Global variable - DO NOT CHANGE NAME or TYPE. 
 * It must point to the first block in the heap and is set by init_heap()
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;     

/* Size of heap allocation padded to round to nearest page size.
 */
int alloc_size; //treated as the remaining leftover free space

/*
 * Additional global variables may be added as needed below
 * TODO: add global variables needed by your function
 */

/* Size of the entire heap
*/
int total_alloc_size; //treated as the total allocated space by init_heap

/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if size < 1 
 * - Determine block size rounding up to a multiple of 8 
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split 
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements 
 *       - Update all heap block header(s) and footer(s) 
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 *   Return if NULL unable to find and allocate block for required size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the 
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* balloc(int size) {     
    //TODO: Your code goes in here.

    //checks
    if (size < 1) return NULL;
    if (heap_start == NULL) return NULL;

    int true_size = size + sizeof(blockHeader);
    if (true_size % 8 != 0) true_size += (8 - (true_size % 8));

    //find the best fit ==============================================
    blockHeader* curr = heap_start; //current block being checked

    
    //if this is the first allocation, set the alloc_size to the heap size
    if (alloc_size == 0) {
        alloc_size = curr -> size_status & -4;
        total_alloc_size = alloc_size;
    }
    
    //return null if we do not have enough room
    if (true_size >= alloc_size) return NULL;

    blockHeader* store = NULL; //stored block with best fit
    int store_size = 0; //size of the block with best fit

    while (curr -> size_status != 1) { //run until end mark where size_status = 1
        int curr_size = curr -> size_status & -4; //curr size

        if (((curr -> size_status) & 1) == 0) { //if the curr block is free
            //printf("free block found\n");
            
            if (curr_size >= true_size) { //if the current block size is bigger than target
                
                if (curr_size < store_size || store_size == 0) { //if the curr size is less than our best size
                    //fit has been found
                    //printf("fit found\n");
                    store = curr;
                    store_size = curr_size;
                }
            }
        } 

        //increment curr
        curr = (blockHeader*)((void*)curr + curr_size); //to get next block, add the size of curr block to the header to get the next block
    }

    //if a best block wasn't found
    if (store == NULL) {
        return NULL;
    }
     
    //splitting
    if (store_size > true_size) { //if the store block is bigger than our target size, split
        int splitter_size = store_size - true_size;
        blockHeader* splitter = (blockHeader*)((void*)store + true_size);
        splitter -> size_status = splitter_size;
        splitter -> size_status |= 2;

        //footer
        blockHeader* footer = (blockHeader*)((void*)splitter + splitter_size - sizeof(blockHeader));
        footer -> size_status = splitter_size;

        //update the allocated block's p bit
        int save_bits = store -> size_status & 2;
        store -> size_status = true_size + save_bits;
    }
    else if (store_size == true_size) { //or if it the store block is the perfect size, set next block's p bit
        blockHeader* next_block = (blockHeader*)((void*)store + store_size);
        next_block -> size_status |= 2; //set p bit to 1
    } else {
        return NULL;
    }
    //disp_heap();
    //update the allocated block's a bit
    store -> size_status += 1;

    alloc_size -= true_size;

    //printf("return\n");
    return (void*) store + sizeof(blockHeader);
} 

/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 *
 * If free results in two or more adjacent free blocks,
 * they will be immediately coalesced into one larger free block.
 * so free blocks require a footer (blockHeader works) to store the size
 *
 * TIP: work on getting immediate coalescing to work after your code 
 *      can pass the tests in partA and partB of tests/ directory.
 *      Submit code that passes partA and partB to Canvas before continuing.
 */                    
int bfree(void *ptr) {    
    
    //if ptr is not aligned or NULL
    if (ptr == NULL || ((int) ptr) % 8 != 0) {
        printf("null or unaligned\n");
        return -1;
    }

    //block header of the block
    blockHeader* curr_block = (blockHeader*)ptr - 1;
    blockHeader* curr_footer = (blockHeader*)((void*)curr_block + (curr_block -> size_status & -4) - sizeof(blockHeader));
    
    //if the block is already free
    if ((curr_block -> size_status & 1) == 0) {
        printf("block is freed\n");
        return -1;
    }

    curr_block -> size_status--; //free the current size
    curr_footer -> size_status = curr_block -> size_status & -4; //footer

    blockHeader* next_block = (blockHeader*)((void*)curr_footer + sizeof(blockHeader));

    if (next_block -> size_status != 1) { //if the next block is not the end, set p bit to 0 b/c we freed the p block
        next_block -> size_status -=2;
    }

    //coalesce time
    
    //previous block
    //we want to use prev block as curr if prev is free so we allocate var inside
    //format <prev><curr> so we want to put our variables into prev if we can
    if ((curr_block -> size_status & 2) == 0) {
        blockHeader* prev_footer = (blockHeader*)((void*)curr_block - sizeof(blockHeader)); //destory after loop
	    blockHeader* prev_block = (blockHeader*)((void*)curr_block - prev_footer->size_status); //destroy after loop

		prev_block -> size_status = ((prev_block -> size_status & -4) + (curr_block -> size_status & -4)) | 2;
            //add curr size to adding size
            //assume that the previous block to the previous block is allcoated due to the nature of free

		curr_block = prev_block; //move the previous block into our current one b/c formating

        //set footer to be right size
		curr_footer -> size_status = (curr_block -> size_status & -4);
	}

	//next block
    next_block = (blockHeader*)((void*)curr_block + (curr_block -> size_status & -4)); 
    //format <prev + curr coalesced><next> OR <curr><next>, either way, we use curr
	if (next_block -> size_status != 1) { //if next block is not the end
		
		if ((next_block -> size_status & 1) == 0) { //next is free
			
			blockHeader* next_footer = (blockHeader*)((void*)next_block + (next_block -> size_status & -4) - sizeof(blockHeader));
                //destroy after loop
			
			curr_footer = next_footer; //set the footer to the very end of the entire coalesced block
				
			curr_block -> size_status = ((curr_block -> size_status & -4) + (next_block -> size_status & -4)) | 2;
                //add curr size + the adding size
                //assume that the previous block to the previous block is allcoated due to the nature of free

            //set the footer the be the right size
			curr_footer -> size_status = (curr_block -> size_status & -4);
		}
	}

    // printf("curr footer size is %i\n", curr_footer -> size_status & -4);
    // printf("curr block size is %i\n", curr_block -> size_status & -4);
    // printf("curr block state is  %i\n", curr_block -> size_status & 3);
    // disp_heap();

    alloc_size += curr_footer -> size_status & -4; //how many free bytes there are, opens up the very max capacity of allocs
        //used in balloc
    return 0;

    //solution 1 - doesn't work
    /*
    
    
    //free the current block, footer will set later
    curr_block -> size_status--;   

    //coallesce the free blocks
    //our previous block header will be the one we use because we cannot move the address of current
    int curr_size = curr_block -> size_status & -4;
    //coalese the prev block
    blockHeader* prev_footer = (blockHeader*)((void*)curr_block - sizeof(blockHeader*));
    int prev_size = prev_footer -> size_status & -4;

    blockHeader* prev_block = (blockHeader*)((void*)curr_block - prev_size);
    int prev_pbit;

    if (curr_block != heap_start && (prev_block -> size_status & 1) == 0) { //coalese
        printf("prev clearing\n");
        int prev_pbit = prev_block -> size_status & 2;
        prev_block -> size_status += (curr_size);
        prev_block -> size_status |= prev_pbit;
    } else { //don't coalesce
        if ((prev_block -> size_status & 1) != 0)
            prev_block = curr_block;
    }

    //coallese the next block
    blockHeader* next_block = (blockHeader*)((void*) curr_block + (curr_size));
    
    if ((next_block -> size_status & 1) == 0) {
        printf("next clearing\n");
        prev_block -> size_status += next_block -> size_status & -4;
    }

    blockHeader* footer = (blockHeader*)((void*)prev_block + (prev_size) - sizeof(blockHeader));
    footer -> size_status = prev_size;

    //set p bit of the next block to 0
    next_block = (blockHeader*)((void*)prev_block + (prev_size));
    if (next_block -> size_status != 1) { //not the end of heap
        next_block -> size_status &= ~(1 >> 1); //p bit = 0
    }

    

    disp_heap();
    alloc_size += prev_block -> size_status & -4;
    return 0;
    */

} 


/* 
 * Initializes the memory allocator.
 * Called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int init_heap(int sizeOfRegion) {    

    static int allocated_once = 0; //prevent multiple myInit calls

    int   pagesize; // page size
    int   padsize;  // size of padding when heap size not a multiple of page size
    void* mmap_ptr; // pointer to memory mapped area
    int   fd;

    blockHeader* end_mark;

    if (0 != allocated_once) {
        fprintf(stderr, 
                "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }

    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize from O.S. 
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }

    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heap_start = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    end_mark = (blockHeader*)((void*)heap_start + alloc_size);
    end_mark->size_status = 1;

    // Set size in header
    heap_start->size_status = alloc_size;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heap_start->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((void*)heap_start + alloc_size - 4);
    footer->size_status = alloc_size;

    return 0;
} 

/* STUDENTS MAY EDIT THIS FUNCTION, but do not change function header.
 * TIP: review this implementation to see one way to traverse through
 *      the blocks in the heap.
 *
 * Can be used for DEBUGGING to help you visualize your heap structure.
 * It traverses heap blocks and prints info about each block found.
 * 
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void disp_heap() {     

    int    counter;
    char   status[6];
    char   p_status[6];
    char * t_begin = NULL;
    char * t_end   = NULL;
    int    t_size;

    blockHeader *current = heap_start;
    counter = 1;

    int used_size =  0;
    int free_size =  0;
    int is_used   = -1;

    fprintf(stdout, 
            "*********************************** HEAP: Block List ****************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, 
            "---------------------------------------------------------------------------------\n");

    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;

        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "alloc");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "FREE ");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "alloc");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "FREE ");
        }

        if (is_used) 
            used_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status, 
                p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, 
            "---------------------------------------------------------------------------------\n");
    fprintf(stdout, 
            "*********************************************************************************\n");
    fprintf(stdout, "Total used size = %4d\n", used_size);
    fprintf(stdout, "Total free size = %4d\n", free_size);
    fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
    fprintf(stdout, 
            "*********************************************************************************\n");
    fflush(stdout);

    return;  
} 


// end p3Heap.c (Fall 2023)                                         


