/**
 * Demonstration C program illustrating how to perform file I/O for vm assignment.
 *
 * Input file contains logical addresses
 * 
 * Backing Store represents the file being read from disk (the backing store.)
 *
 * We need to perform random input from the backing store using fseek() and fread()
 *
 * This program performs nothing of meaning, rather it is intended to illustrate the basics
 * of I/O for the vm assignment. Using this I/O, you will need to make the necessary adjustments
 * that implement the virtual memory manager.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


// number of characters to read for each line from input file
#define BUFFER_SIZE         10

// number of bytes to read
#define CHUNK               256
#define TABLE_SIZE          256
#define MEMORY_SIZE         256

#define OF_MASK                0x00FF

FILE    *address_file;
FILE    *backing_store;

// how we store reads from input file
char    address[BUFFER_SIZE];

int     logical_address;

int     page_number = 0;
int     offset = 0;

int     page_table[TABLE_SIZE];
int     frame_table[TABLE_SIZE];

int     frame_counter = 0;
int     physical_counter = 0;

// Keep track of our faults and such. 
float     faults = 0;
float     total_lookups = 0;
float     tlb_hits = 0;


// This is pretty much the "Physical memory" the system has. 
signed char     physical_memory[MEMORY_SIZE*MEMORY_SIZE];

// the value of the byte (signed char) in memory
signed char     value;
char            loc[2];
int             frame_number;

// This index will match the frame. The value in here will be the logical address.
int     tlb_refs[16];
// The value in here will be the frame in physical memory that addr is in 
int     tlb_frame_numbers[16];


int     current_fifo_index_tlb = 0;
int     current_fifo_index_pt = 0;

int     items_in_pt = 0;


int add_to_tlb(int pn, int fn) {
    tlb_refs[current_fifo_index_tlb] = pn;
    tlb_frame_numbers[current_fifo_index_tlb] = fn;

    current_fifo_index_tlb++;
    if (current_fifo_index_tlb > 15) {
        current_fifo_index_tlb = 0;
        return current_fifo_index_tlb;
    }

    return current_fifo_index_tlb-1;
}

// Return the index of the 
int check_tlb(int pn) {
    int x;
    for (x = 0; x < 16; x++) {
        if (tlb_refs[x] == pn) {
            return x;
        }
    }

    return -1;
}

int add_page_frame(int pn, int fn) {
    page_table[pn] = fn;
}

int main(int argc, char *argv[])
{
    // perform basic error checking
    if (argc != 3) {
        fprintf(stderr,"Usage: ./a.out [backing store] [input file]\n");
        return -1;
    }

    // Initalize the arrays for the page table. 
    int x;
    for (x = 0; x < TABLE_SIZE; x++) {
        page_table[x] = -1;
    }

    for (x = 0; x < TABLE_SIZE; x++) {
        // x*chunk is the frame number
        frame_table[x] = x*CHUNK;
    }
    // #######################################


    // open the file containing the backing store
    backing_store = fopen(argv[1], "rb");
    
    if (backing_store == NULL) { 
        fprintf(stderr, "Error opening %s\n",argv[1]);
        return -1;
    }

    // open the file containing the logical addresses
    address_file = fopen(argv[2], "r");

    if (address_file == NULL) {
        fprintf(stderr, "Error opening %s\n",argv[2]);
        return -1;
    }

    // read through the input file and output each logical address
    while ( fgets(address, BUFFER_SIZE, address_file) != NULL) {
        total_lookups++;

        logical_address = atoi(address);
        page_number = (logical_address >> 8);
        offset = (logical_address & OF_MASK);

        value = 0;


        int check_tlb_index = check_tlb(page_number);
        // The value isn't in the page table. It's a fault! 
        // ToDo: Check if it's in the TLB.
        if ((page_table[page_number] == -1) && (check_tlb_index == -1)) {    
            faults++;

            // first seek to byte CHUNK * page_number in the backing store
            // SEEK_SET in fseek() seeks from the beginning of the file
            if (fseek(backing_store, page_number*CHUNK, SEEK_SET) != 0) {
                fprintf(stderr, "Error seeking in backing store\n");
                return -1;
            }

            // if (physical_counter >= ((MEMORY_SIZE*MEMORY_SIZE)-CHUNK)) {
            //     physical_counter = 0;                
            // }

            // now read CHUNK bytes from the backing store to the "physical memory",
            // The & is needed to say to start reading to the address at the physical counter variable. 
            // In other words, add starting from the last element of the array. 
            if (fread(&physical_memory[physical_counter], sizeof(signed char), CHUNK, backing_store) == 0) {
                fprintf(stderr, "Error reading from backing store\n");
                return -1;
            }
             
            // Tell the address thing above where it should now be. 
            physical_counter = physical_counter + CHUNK;


            frame_number = frame_table[frame_counter];
            frame_counter++;

            // Add the item to the page_table
            add_page_frame(page_number, frame_number);            

            // Add to the TLB.
            add_to_tlb(page_number, frame_number);

            // Tell the printf statement where it came from. 
            strcpy(loc, "BS");
        } 

        else {
            if (check_tlb_index != -1) {
                strcpy(loc, "TB");

                tlb_hits++;

                frame_number = tlb_frame_numbers[check_tlb_index];

            } else {
                strcpy(loc, "PT");

                add_to_tlb(page_number, frame_number);

                frame_number = page_table[page_number];
            }
        }
        
        int physical_address = (frame_number | offset);
        value = physical_memory[frame_number+offset];

        printf("Logical Address: %d \t Physical Address: %d \t Value: %d\n", logical_address, physical_address, value);
    }

    printf("\nTotal Lookups = %0.0f\n", total_lookups);
    printf("Page Faults = %0.0f\n", faults);
    printf("Page Fault Rate = %0.3f\n", (faults/total_lookups));
    printf("TLB Hits = %0.0f\n", tlb_hits);
    printf("TLB Hit Rate = %0.3f\n", (tlb_hits/total_lookups));

    fclose(address_file);
    fclose(backing_store);

    return 0;
}

