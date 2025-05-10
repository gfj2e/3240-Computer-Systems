#include <stdio.h>

// Function to print binary representation of a number
void printBinary(unsigned int n);
extern long MysteryFunction1(long a, int b);
long C_MysteryFunction1(long a, int b);
extern unsigned int MysteryFunction2(unsigned int num);
unsigned int C_MysteryFunction2(unsigned int num);
extern long MysteryFunction3(long *a, int n);
long C_MysteryFunction3(long *a, int n);
extern int MysteryFunction4(unsigned long n);
int C_MysteryFunction4(unsigned long n);
extern unsigned int MysteryFunction5(unsigned int A, unsigned int B);
unsigned int C_MysteryFunction5(unsigned int A, unsigned int B);

int main(void) {

   // long arr[5] = {1, 2, 5, 4, 3};

   printf("MysteryFunction5: %u\n", MysteryFunction5(134, 128));
   printf("C Function: %u\n", C_MysteryFunction5(134, 128));

   // printBinary(7);
   // printBinary(MysteryFunction2(7));

    return 0;
}

void printBinary(unsigned int n) {
    // Number of bits in an int
    int numBits = sizeof(unsigned int) * 8;
    for (int i = numBits - 1; i >= 0; i--) {
        // Check if the ith bit is set
        unsigned int bit = (n >> i) & 1;
        printf("%u", bit);
    }
    printf("\n");
}

long C_MysteryFunction1(long a, int b){

   //TODO
   //check assembly code for MysteryFunction1 in project2.s
   long result = 1;
   while (b > 0) {
      result *= a;
      b--;
   }
   return result;
}

unsigned int C_MysteryFunction2(unsigned int num)
{
    unsigned int result = 0; // Initialize the result to 0.
    unsigned int i = 0;      // Initialize the loop counter.

    // Iterate over each bit of the 32-bit integer.
    while (i <= 31)
    {
        unsigned int mask = 1 << i; // Create a mask to check the ith bit of `num`.

        // If the ith bit of `num` is set, set the corresponding bit in `result`.
        if (num & mask)
        {
            unsigned int shifted = (1U << 31) >> i; // Calculate the position of the bit in the reversed result.
            result |= shifted;                      // Set the bit in the result.
        }

        i++; // Move to the next bit.
    }

    return result; // Return the reversed bit pattern.

}

long C_MysteryFunction3(long *a, int n){
   //TODO
   //check assembly code for MysteryFunction3 in project2.s
   long largest = *a;
   for (int i = 1; i < n; i++) {
      if (largest < *(a + i)) {
         largest = *(a + i);
      }
   }
   return largest;
}

int C_MysteryFunction4(unsigned long n)
{
    int count = 0; // Initialize the count of set bits to 0.

    // Iterate through each bit of `n`.
    while (n != 0)
    {
        count += (n & 1); // If the least significant bit is set, increment the count.
        n >>= 1;          // Shift right to check the next bit.
    }

    return count; // Return the total count of set bits.
}

unsigned int C_MysteryFunction5(unsigned int A, unsigned int B)
{
    unsigned int xor_result = A ^ B; // XOR of A and B to find differing bits.
    unsigned int count = 0;          // Initialize the count of differing bits to 0.

    // Iterate through each bit of the XOR result.
    while (xor_result != 0)
    {
        count += (xor_result & 1); // If the least significant bit is set, increment the count.
        xor_result >>= 1;          // Shift right to check the next bit.
    }

    return count; // Return the Hamming distance.
}