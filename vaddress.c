#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define BITS 12

int main (int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "參數長度錯誤\n");
        return -1;
    }

    char str[12];
    uint32_t virtual_address = atoi(argv[1]);
    sprintf(str, "%u", virtual_address);
    if (strlen(argv[1]) != strlen(str) || virtual_address < 0) {
        fprintf(stderr, "address 超出範圍\n");
        return -1;
    }
    
    uint32_t page_number = virtual_address >> BITS;
    uint32_t offset = virtual_address % (2 << (BITS - 1));
    printf("The address %u contains:\n", virtual_address);
    printf("page number = %u\n", page_number);
    printf("offset = %u\n", offset);

    return 0;
}
