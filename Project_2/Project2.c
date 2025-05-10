/* 
 * CSCI3240: Project 2
 * Garrett Johnson
 * 
 * Project2.c - Source file with your solutions to the Lab.
 *          This is the file you will submit in the D2L -> Project2 dropbox.
 *
 *   Please make sure you don't include the main function in this file. Graders will use a separate `.c` file containing the `main` function to test your Mystery functions.  
 *	 If you'd like to test your code, you can create a separate `main.c` file that includes the `main` function, which calls the Mystery functions defined in this `Project2.c` file.
 *   You don't need to submit the 'main.c' file.
 *	 Make sure you have insightful comments in your code. 
 *   
 * 
 */

// Finds a to the power of b, or a^b
long MysteryFunction1(long a, int b){

   //TODO
   //check assembly code for MysteryFunction1 in project2.s
   long result = 1;  // Set result to 1
   while (b > 0) {   // While b greater than 0
      result *= a;   // Multiply a * a
      b--;           // Decrement b
   }
   return result;    // Return the power of a number
}

// Mirrors the bits of a number that is an unsigned int.
unsigned int MysteryFunction2(unsigned int num)
{
     //TODO
   //check assembly code for MysteryFunction2 in project2.s
   unsigned int result = 0;                  // Result for storing the mirrored bits
   for (int i = 0; i <= 31; i++) {           // While i is less than or equal to 31
      if (num & (1 << i)) {                  // If the %ecx ith bit is set 
         result = result | (1 << (31 - i));  // Set corresponding bit in result
      }
   }
   return result;
}

// Finds the largest number in an array
long MysteryFunction3(long *a, int n){
   //TODO
   //check assembly code for MysteryFunction3 in project2.s
   long largest = *a;            // Set arr[0] to largest 
   for (int i = 1; i < n; i++) {     // For loop to iterate through the array
      if (largest < *(a + i)) {  // If largest is less than next index
         largest = *(a + i);     // Then set largest to that index
      }
   }
   return largest;               // Return the largest value in the array
}     

// Counts the number of set bits(1s) in an unsigned long number
int MysteryFunction4(unsigned long n)
{
      //TODO
   //check assembly code for MysteryFunction4 in project2.s
   int bits = 0;                 // Variable for number of bits
   while (n) {                   // While n is not shifted to 0
      bits = bits + (n & 1);     // If least significant bit is 1, add 1
      n = n >> 1;                // Shift right 1
   }
   return bits;                  // Return number of bits
}

// Determines the number of differing bits between two numbers
unsigned int MysteryFunction5(unsigned int A, unsigned int B)
{
   //TODO
   //check assembly code for MysteryFunction5 in project2.s
   A = A ^ B;                                   // Xor differing bits
   unsigned int num_xor_bits = 0;               // Num of differing bits
   unsigned int xor_bit = 0;                    // Xor'd bits
   while (A) {                                  // While A is not shifted to 0
      unsigned int xor_bit = A;                 // Assign A to xor_bit bit
      xor_bit = (xor_bit & 1);                  // If least significant bit, make xor_bit 1
      num_xor_bits = num_xor_bits + xor_bit;    // Add xor_bit to num_xor_bits
      A = A >> 1;                               // Shift A right by 1
   }
   return num_xor_bits;                         // Return number of xor bits
}

// Do not add a main function here.

