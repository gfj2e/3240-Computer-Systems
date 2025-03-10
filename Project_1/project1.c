/*
Name: Garrett Johnson
CSCI-3240 Project 1 Hex Dump
Dr. Sainju
Due ate: 02/20/2025
*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

typedef unsigned char BYTE;     // Unsigned char represents bytes to be read
#define SIZE_BYTES 16
#define GROUP_BYTES 2

void xxd(FILE *file);           // Function to replicate xxd command in unix

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *file_arg = fopen(argv[1], "rb");                          // Open file entered in from command line
    if (!file_arg) {
        fprintf(stderr, "Error opening %s file!\n", argv[1]);       // If file does not exist, then error
        exit(EXIT_FAILURE);    
    }

    xxd(file_arg);              // Call the function to replicate xxd

    fclose(file_arg);

    return 0;
}

void xxd(FILE *file) {
    size_t counter = 0;             // Counter to keep track of the lines of bytes
    size_t bytes_read;              // Var to keep track of the num of bytes read
    BYTE hex_value[SIZE_BYTES];     // byte array to store the bytes

    while ((bytes_read = fread(hex_value, sizeof(hex_value[0]), SIZE_BYTES, file)) > 0) {
        printf("%08lx: ", counter);
        size_t i;

        // ! Print hex values in groupings of two bytes, use size_t for portability purposes
        for (i = 0; i < bytes_read; i++) {
            printf("%02x", hex_value[i]);
            if (i % GROUP_BYTES == 1) {     // For every second byte printed, print a space 
                printf(" ");
            }
        }

        // Padding to align the characters section
        for (i = bytes_read; i < SIZE_BYTES; i++) {
            printf("  ");
            if (i % GROUP_BYTES == 1) {         
                printf(" ");
            }
        }

        printf(" ");

        // Print the character representation with conditional if the character is printable or not
        for (i = 0; i < bytes_read; i++) {
            if (isprint(hex_value[i])) {
                printf("%c",  hex_value[i]);
            } else {
                printf(".");
            }
        }

        printf("\n");           // Print new line
        counter += SIZE_BYTES;  // Increment counter by 16
    }
}