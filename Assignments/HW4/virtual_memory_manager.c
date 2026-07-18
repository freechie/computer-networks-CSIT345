/*
 * Dr. Daeyoung Kim
 * Operating Systems
 * CSIT345_02SP23
 *
 * Richie Molina
 * May 5, 2023
 * HW 4
 *
 * This program implements a Virtual Memory Manager as described in the
 * assignment specifications. It translates logical addresses to physical
 * addresses, simulates a TLB, and handles page faults using demand paging.
 * The program reads a list of logical addresses from a file, translates them
 * to their corresponding physical addresses, and outputs the value of the
 * signed byte stored at the translated physical address.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Define constants for the memory manager
#define PAGE_OFFSET_BITS 8
#define PAGE_SIZE 256
#define NUM_FRAMES 256
#define NUM_PAGES 256
#define TLB_SIZE 16
#define LOGICAL_ADDRESS_SIZE 16
#define LOGICAL_ADDRESS_MASK 0xFFFF
#define PAGE_NUMBER_MASK 0xFF00
#define OFFSET_MASK 0xFF
#define BACKING_STORE_FILENAME "BACKING_STORE.bin"
#define INPUT_FILENAME "addresses.txt"

// Page table entry structure
typedef struct
{
    int page_number;
    int frame_number;
    bool valid;
} PageTableEntry; // 4 bytes

// TLB entry structure
typedef struct
{
    int page_number;
    int frame_number;
    int timestamp;
} TLBEntry; // 12 bytes

// Declare global data structures and variables
PageTableEntry page_table[NUM_PAGES];
TLBEntry tlb[TLB_SIZE];
char physical_memory[NUM_FRAMES * PAGE_SIZE];
bool frame_allocated[NUM_FRAMES];
int tlb_tail = 0;
int page_faults = 0;
int tlb_hits = 0;
int tlb_misses = 0;
int total_addresses = 0;
// Handles a page fault for the given page number
int handle_page_fault(int page_number)
{
    int frame_number;

    // Find an unallocated frame
    for (frame_number = 0; frame_number < NUM_FRAMES; frame_number++)
    {
        if (!frame_allocated[frame_number])
        {
            frame_allocated[frame_number] = true;
            break;
        }
    }

    // If no free frame is available, exit with an error
    if (frame_number == NUM_FRAMES)
    {
        fprintf(stderr, "No free frames available\n");
        exit(1);
    }

    // Read the 256-byte page from backing store
    FILE *backing_store = fopen(BACKING_STORE_FILENAME, "rb");
    if (!backing_store)
    {
        fprintf(stderr, "Error opening backing store file\n");
        exit(1);
    }

    fseek(backing_store, page_number * PAGE_SIZE, SEEK_SET);
    fread(&physical_memory[frame_number * PAGE_SIZE], PAGE_SIZE, 1, backing_store);
    fclose(backing_store);

    // Update the page table
    page_table[page_number].frame_number = frame_number;
    page_table[page_number].valid = true;

    frame_allocated[frame_number] = true;
    page_faults++;

    return frame_number;
}

// Searches the TLB for the given page number
int search_tlb(int page_number)
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        if (tlb[i].page_number == page_number)
        {
            return tlb[i].frame_number;
        }
    }

    return -1;
}

// Updates the TLB with the given page number and frame number
void update_tlb(int page_number, int frame_number)
{
    tlb[tlb_tail].page_number = page_number;
    tlb[tlb_tail].frame_number = frame_number;
    tlb_tail = (tlb_tail + 1) % TLB_SIZE;
}

// Extracts the page number and offset from the logical address
void extract_page_number_and_offset(int logical_address)
{
    int page_number = (logical_address & 0x00FF) >> 8;
    int offset = logical_address & 0x00FF;

    printf("Logical Address: %d, Page Number: %d\n", logical_address, page_number);
}
// Main function of the program
int main(int argc, char *argv[])
{
    // Check if the correct number of arguments are provided
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        exit(1);
    }

    // Open the file containing the logical addresses
    FILE *address_file = fopen(argv[1], "r");
    if (!address_file)
    {
        fprintf(stderr, "Error opening address file\n");
        exit(1);
    }

    int logical_address;
    // Loop through each logical address in the file
    while (fscanf(address_file, "%d", &logical_address) != EOF)
    {
        // Extract the page number and page offset from the logical address
        int page_number = (logical_address & PAGE_NUMBER_MASK) >> PAGE_OFFSET_BITS;
        int page_offset = logical_address & OFFSET_MASK;

        // Search for the page number in the TLB
        int frame_number = search_tlb(page_number);
        if (frame_number == -1) // TLB miss
        {
            // Check if the page is in the page table
            if (page_table[page_number].valid)
            {
                frame_number = page_table[page_number].frame_number;
            }
            else // Page fault
            {
                frame_number = handle_page_fault(page_number);
            }
            // Update the TLB with the new page number and frame number
            update_tlb(page_number, frame_number);
            tlb_misses++;
        }
        // Calculate the physical address using the frame number and page offset
        int physical_address = (frame_number << PAGE_OFFSET_BITS) | page_offset;
        signed char value = physical_memory[physical_address];

        // Output the results
        printf("Logical address: %d, Physical address: %d, Value: %d\n", logical_address, physical_address, value);
    }

    // Close the address file
    fclose(address_file);

    // Calculate and output the page fault rate and TLB hit rate
    printf("Page fault rate: %.2f%%\n", (double)page_faults * 100 / (page_faults + tlb_misses));
    printf("TLB hit rate: %.2f%%\n", (double)tlb_misses - page_faults * 100 / (page_faults + tlb_misses));

    return 0;
}
