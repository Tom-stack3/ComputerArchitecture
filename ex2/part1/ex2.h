#ifndef UNTITLED2_EX2_H
#define UNTITLED2_EX2_H
#define BYTE 8
#define TRUE 1
#define FALSE 0

#include <stdio.h>

typedef int magnitude;

magnitude add(magnitude a, magnitude b);   // a + b
magnitude sub(magnitude a, magnitude b);   // a - b
magnitude multi(magnitude a, magnitude b); // a * b

// true = 1, false = 0
int equal(magnitude a, magnitude b);   // a = b
int greater(magnitude a, magnitude b); // a > b

int mag_to_int(magnitude m);
magnitude int_to_mag(int i);

#endif // UNTITLED2_EX2_H
