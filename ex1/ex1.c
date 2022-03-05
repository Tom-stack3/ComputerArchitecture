#include "ex1.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define BYTE 8

/*
 *  @name: is_big_endian
 *  @author: Tommy Zaft
 *  @return: 1 if the program was compiled and executed on a machine uses Big-Endian, 0 if the machine uses Little-Endian.
 *  @brief: In order to detect big or little endian, we create an int16_t (16 bit interger) variable named int16.
 *    Then using a pointer of type int8_t*, meaning it's a pointer to an 8 bits integer, we point to the first 8 bits of int16.
 *    We check its value, if the pointer points to the 8 smaller bits , it means the machine is working on little-endian,
 *    otherwise, the machine works on big-endian.
 */
int is_big_endian()
{
  int is_big_endian = 0;
  int16_t int16 = 0b0000000010000000;
  // points only to the first 8 bits from int16.
  int8_t *ptr8 = (int8_t *)&int16;

  /*
  The original number (int16) was: 00000000 10000000 (bytes separated by space)
  If the first 8 bits from int16 were 10000000, it means the machine is working on little-endian.
  That is because the smaller byte (10000000) was stored first in the memory, therefore little-endian.
  We compare to -128 because 0b10000000 == -128 (in a signed int)
  */
  if (*ptr8 == -128)
  {
    // the machine uses little endian.
    is_big_endian = 0;
  }
  else
  {
    // the machine uses big endian.
    is_big_endian = 1;
  }

  return is_big_endian;
}

/*
 *  @name: merge_bytes
 *  @author: Tommy Zaft
 *  @return: an unsigned long, the result of a merge between y's first half (with LSB) and x's second half (with MSB).
 *  @brief: In order to merge both x and y, we use two variables (x_half and y_half) to store the wanted halves, and in the end return their sum.
 *    First, we start in y's LSB (which location changes whether it's little or big endian), and each step move to the next byte and add it to y_half (shifted left appropriately).
 *    We do so until we get to the middle of y. After we finished with y, we start working on x's half.
 *    We go to the middle of x (which location changes whether it's little or big endian), and each step move to the next byte and add it to x_half (shifted left appropriately).
 *    In the end, we sum both halves and return it.
 */
unsigned long merge_bytes(unsigned long x, unsigned long int y)
{
  unsigned long result = 0;
  unsigned long x_half = 0;
  unsigned long y_half = 0;
  int i;
  // a pointer to the current byte being handled in y.
  uint8_t *ptr_y;
  // a pointer to the current byte being handled in x.
  uint8_t *ptr_x;
  unsigned long temp;
  // the difference between one byte to a bigger one:
  // - in little-endian the difference = +1 because the LSB is stored first.
  // - in big-endian the difference = -1, because the MSB is stored first in memory.
  int next_byte_difference;

  // if the machine runs on little-endian
  if (!is_big_endian())
  {
    next_byte_difference = +1;
    // points to the LSB of y. because the machine runs on little-endian, the begining is where the LSB is stored.
    ptr_y = (uint8_t *)&y;
    // points to the middle of x.
    ptr_x = (uint8_t *)&x + sizeof(unsigned long) / 2;
  }
  // else, the machine runs on big-endian
  else
  {
    next_byte_difference = -1;
    // points to the LSB of y. because the machine runs on big-endian, the end is where the LSB is stored.
    ptr_y = (uint8_t *)&y + sizeof(unsigned long) - 1;
    // points to the middle of x.
    ptr_x = (uint8_t *)&x + sizeof(unsigned long) / 2 - 1;
  }

  // get y's first half (with LSB)
  for (i = 0; i < sizeof(unsigned long) / 2; i++, ptr_y += next_byte_difference)
  {
    temp = (unsigned long)*ptr_y;
    temp = temp << BYTE * i;
    y_half += temp;
  }

  // get x's second half (with MSB)
  for (i = sizeof(unsigned long) / 2; i < sizeof(unsigned long); i++, ptr_x += next_byte_difference)
  {
    temp = (unsigned long)*ptr_x;
    temp = temp << BYTE * i;
    x_half += temp;
  }

  // sum halves and return result
  result = x_half + y_half;
  return result;
}

/*
 *  @name: merge_bytes
 *  @author: Tommy Zaft
 *  @return: an unsigned long, the result of replacing the i-th byte in x.
 *  @brief: In order to replace the i-th byte in x, we use a variable result, which we use to create the ubsigned long to return.
 *    First we add all the bytes from x before the i-th index, meaning bytes 0,1,...,i-1 to the variable result (shifted left appropriately).
 *    Then we add the new byte received to result, also (shifted left appropriately).
 *    Finally, we add to result the remaining bytes from x excluding the i-th byte, meaning indexes i+1,..., and return the result.
 *    Of course the location in memory of the index 0 byte depends on whether it's little or big endian, and also the way we advance from byte k to byte k+1.
 *    That is what we handle in the if-else statment with two variables: ptr and next_byte_difference.
 */
unsigned long put_byte(unsigned long x, unsigned char b, int i)
{
  // real index to replace = (num_of_bits_in_long/8) - i - 1 = sizeof(long) - i - 1
  i = sizeof(unsigned long) - i - 1;
  // if the index is out of range
  if (i < 0 || i >= sizeof(unsigned long))
  {
    // return the original x received.
    return x;
  }
  unsigned long result = 0;
  int j;
  // a pointer to the current byte being handled, starts at index 0 of the unsigned long.
  uint8_t *ptr;
  unsigned long temp;
  // the difference between one byte to a bigger one:
  // - in little-endian the difference = +1 because the LSB is stored first.
  // - in big-endian the difference = -1, because the MSB is stored first in memory.
  int next_byte_difference;

  // if the machine runs on little-endian
  if (!is_big_endian())
  {
    // pointer to the begining of x. because the machine runs on little-endian, the begining is where the LSB is stored, meaning index 0.
    ptr = (uint8_t *)&x;
    next_byte_difference = +1;
  }
  // else, the machine runs on big-endian
  else
  {
    // pointer to the end of x. because the machine runs on big-endian, the end is where the LSB is stored, meaning index 0.
    ptr = (uint8_t *)&x + sizeof(unsigned long) - 1;
    next_byte_difference = -1;
  }

  // add to result x's first half (with LSB) before the i-index byte.
  // (indexes 0,1,...,i-1)
  for (j = 0; j < i; j++, ptr += next_byte_difference)
  {
    temp = (unsigned long)*ptr;
    result += temp << BYTE * j;
  }
  // add the new received byte in the i-index
  temp = (unsigned long)b;
  result += temp << BYTE * j;
  j++;
  ptr += next_byte_difference;

  // add to result x's second half (with MSB) after the i-index byte.
  // (indexes i+1,...)
  for (; j < sizeof(unsigned long); j++, ptr += next_byte_difference)
  {
    temp = (unsigned long)*ptr;
    result += temp << BYTE * j;
  }

  return result;
}