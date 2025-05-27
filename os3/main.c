#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define PHYSICAL_MEMORY_SIZE 65536
#define BACKING_STORE "BACKING_STORE.bin"

typedef struct {
    int page_number;
    int frame_number;
} TLBEntry;

signed char physical_memory[PHYSICAL_MEMORY_SIZE];
int page_table[PAGE_TABLE_SIZE];
TLBEntry tlb[TLB_SIZE];

int tlb_index = 0;
int next_free_frame = 0;

int total_addresses = 0;
int page_faults = 0;
int tlb_hits = 0;

int get_page_number(int logical_address) {
    return (logical_address >> 8) & 0xFF;
}

int get_offset(int logical_address) {
    return logical_address & 0xFF;
}

int search_tlb(int page_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].page_number == page_number) {
            return tlb[i].frame_number;
        }
    }
    return -1;
}

void add_to_tlb(int page_number, int frame_number) {
    tlb[tlb_index % TLB_SIZE].page_number = page_number;
    tlb[tlb_index % TLB_SIZE].frame_number = frame_number;
    tlb_index++;
}

int get_frame_number(int page_number, FILE *backing_store) {
    int frame_number = search_tlb(page_number);
    if (frame_number != -1) {
        tlb_hits++;
        return frame_number;
    }

    if (page_table[page_number] != -1) {
        frame_number = page_table[page_number];
    } else {
        // page fault
        page_faults++;
        fseek(backing_store, page_number * FRAME_SIZE, SEEK_SET);
        fread(&physical_memory[next_free_frame * FRAME_SIZE], sizeof(signed char), FRAME_SIZE, backing_store);
        frame_number = next_free_frame;
        page_table[page_number] = frame_number;
        next_free_frame++;
    }

    add_to_tlb(page_number, frame_number);
    return frame_number;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./vm_manager addresses.txt\n");
        exit(1);
    }

    FILE *address_file = fopen(argv[1], "r");
    if (!address_file) {
        perror("Error opening address file");
        exit(1);
    }

    FILE *backing_store = fopen(BACKING_STORE, "rb");
    if (!backing_store) {
        perror("Error opening BACKING_STORE.bin");
        exit(1);
    }

    memset(page_table, -1, sizeof(page_table));
    memset(tlb, -1, sizeof(tlb));

    int logical_address;
    while (fscanf(address_file, "%d", &logical_address) == 1) {
        total_addresses++;
        int page_number = get_page_number(logical_address);
        int offset = get_offset(logical_address);
        int frame_number = get_frame_number(page_number, backing_store);
        int physical_address = (frame_number << 8) | offset;
        signed char value = physical_memory[physical_address];
        printf("Logical address: %d Physical address: %d Value: %d\n", logical_address, physical_address, value);
    }

    printf("\nPage Faults = %d\n", page_faults);
    printf("Page Fault Rate = %.3f\n", (float)page_faults / total_addresses);
    printf("TLB Hits = %d\n", tlb_hits);
    printf("TLB Hit Rate = %.3f\n", (float)tlb_hits / total_addresses);

    fclose(address_file);
    fclose(backing_store);
    return 0;
}
