#include "ex2.h"

/**
 * Return the int representation of the magnitude received.
 */
int mag_to_int(magnitude m)
{
    // number of bytes in int
    unsigned int int_size = sizeof(unsigned int);
    int i = 0;
    unsigned int msb_bit_mask = 1 << ((BYTE * int_size) - 1);
    i = m & (~msb_bit_mask);
    // if the MSB is on, the int is negative
    if ((m & msb_bit_mask) != 0)
    {
        i *= -1;
    }
    return i;
}

/**
 * Return the magnitude representation of the int received.
 */
magnitude int_to_mag(int i)
{
    // number of bytes in int
    unsigned int int_size = sizeof(unsigned int);
    magnitude m;
    if (i == 0)
    {
        m = 0;
    }
    else if (i < 0)
    {
        m = (-i) + (1 << ((BYTE * int_size) - 1));
    }
    else
    {
        m = i;
    }
    return m;
}

/**
 * Return magnitude m with a flipped sign bit (MSB).
 */
magnitude flip_sign(magnitude m)
{
    // number of bytes in int
    unsigned int int_size = sizeof(unsigned int);
    unsigned int msb_bit_mask = 1 << ((BYTE * int_size) - 1);
    return (m ^ msb_bit_mask);
}

magnitude add(magnitude a, magnitude b)
{
    int a_i = mag_to_int(a);
    int b_i = mag_to_int(b);
    int result_i = a_i + b_i;
    magnitude result = int_to_mag(result_i);

    // check if got overflow, and fix the sign bit appropriately
    if (a_i > 0 && b_i > 0 && result_i < 0)
    {
        result = flip_sign(result_i);
    }
    else if (a_i < 0 && b_i < 0 && result_i > 0)
    {
        result = int_to_mag(flip_sign(result_i));
    }

    return result;
}
magnitude sub(magnitude a, magnitude b)
{
    int a_i = mag_to_int(a);
    int b_i = mag_to_int(b);
    // use the addition function
    return add(a, flip_sign(b));
}
magnitude multi(magnitude a, magnitude b)
{
    int a_i = mag_to_int(a);
    int b_i = mag_to_int(b);
    int result_i = a_i * b_i;
    magnitude result = int_to_mag(result_i);

    // check if got overflow, and fix the sign bit appropriately
    if (a_i > 0 && b_i > 0 && result_i < 0)
    {
        result = flip_sign(result_i);
    }
    else if (a_i < 0 && b_i < 0 && result_i < 0)
    {
        result = int_to_mag(flip_sign(result_i));
    }
    else if (((a_i < 0 && b_i > 0) || (a_i > 0 && b_i < 0)) && result_i > 0)
    {
        result = int_to_mag(flip_sign(result_i));
    }

    return result;
}

int equal(magnitude a, magnitude b)
{
    int a_i = mag_to_int(a);
    int b_i = mag_to_int(b);
    return (a_i == b_i);
}
int greater(magnitude a, magnitude b)
{
    int a_i = mag_to_int(a);
    int b_i = mag_to_int(b);
    return (a_i > b_i);
}