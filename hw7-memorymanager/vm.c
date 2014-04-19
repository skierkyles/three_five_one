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

#define OF_MASK                0x00FF

FILE    *address_file;
FILE    *backing_store;

// how we store reads from input file
char    address[BUFFER_SIZE];

int     logical_address;

int     page_number = 0;
int     offset = 0;

int     page_table[CHUNK];
int     frame_table[CHUNK];

int     frame_counter = 0;
int     physical_counter = 0;

// the buffer containing reads from backing store
signed char     buffer[CHUNK];

// This is pretty much the "Physical memory" the system has. 
signed char     *physical_memory[CHUNK*CHUNK];

// the value of the byte (signed char) in memory
signed char     value;

int main(int argc, char *argv[])
{
    // perform basic error checking
    if (argc != 3) {
        fprintf(stderr,"Usage: ./a.out [backing store] [input file]\n");
        return -1;
    }

    int x;
    for (x = 0; x < 256; x++) {
        page_table[x] = -1;
        frame_table[x] = -1;
    }

    for (x = 0; x < 256; x++) {
        // x*chunk is the frame number
        frame_table[x] = x*CHUNK;
    }

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
        logical_address = atoi(address);
        page_number = (logical_address >> 8);
        offset = (logical_address & OF_MASK);

        value = 0;


        // The value isn't in the page table. It's a fault! 
        // ToDo: Check if it's in the TLB.
        if (page_table[page_number] == -1) {    
            // first seek to byte CHUNK * page_number in the backing store
            // SEEK_SET in fseek() seeks from the beginning of the file
            if (fseek(backing_store, page_number*CHUNK, SEEK_SET) != 0) {
                fprintf(stderr, "Error seeking in backing store\n");
                return -1;
            }

            // now read CHUNK bytes from the backing store to the buffer
            if (fread(buffer, sizeof(signed char), CHUNK, backing_store) == 0) {
                fprintf(stderr, "Error reading from backing store\n");
                return -1;
            }
             


            // printf("PN %d ", page_number);
            // Add the item to the page_table
            int frame_number = frame_table[frame_counter];
            frame_counter++;
            // printf("FN %d ", frame_number);
            int physical_address = (frame_number | offset);
            // printf("Phy Addr %d \n", physical_address);

            physical_memory[frame_number] = buffer;

            page_table[page_number] = frame_number;

            // Get the value we are going for. 
            signed char *v = physical_memory[frame_number];
            value = v[offset];


            // printf("Page_table Value: %d BValue: %d \n", page_table[page_number], value);



            // Last result should be:
            // Virtual address: 12107 Physical address: 2635 Value: -46
            printf("FA: Logical Address: %d \t FN: %d Physical Address: %d \t Value: %d\n",logical_address, frame_number, physical_address, value);
        } 

        else {
            // printf("PN %d ", page_number);
            int frame_number = page_table[page_number];
            // printf("FN %d ", frame_number);
            // printf("Frame #: %d \n", frame_number);
            int physical_address = (frame_number | offset);
            // printf("PA %d \n", physical_address);
            signed char *v = physical_memory[frame_number];
            // printf("After V \n");

            value = v[offset]; 
            
            printf("PT: Logical Address: %d \t FN: %d Physical Address: %d \t Value: %d\n",logical_address, frame_number, physical_address, value);
        }


    }


    fclose(address_file);
    fclose(backing_store);

    return 0;
}

