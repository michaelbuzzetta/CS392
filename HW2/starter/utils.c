/*******************************************************************************
 * Name        : utils.c
 * Author      : Michael Buzzetta
 * Pledge      : I pledge my honor that I have abided by the Stevens honor system
 ******************************************************************************/
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

/*
Recieved help from CAs during office hours:
Eddison So
Daniel Zamloot
Rudolph Sedlin
*/

//This function compares two ints, if the first value is larger, the fuc tion returns 1, 
//if it is smaller, the function returns -1 and if they are even the function returns 0
int cmpr_int(void* a, void* b) 
{
    int intA = *((int*)a);
    int intB = *((int*)b);

    if (intA > intB) 
    {
        return 1;
    }
    else if (intA < intB) 
    {
        return -1;
    }
    else 
    {
        return 0;
    }
}

//This function compares two floats, if the first value is larger, the fuc tion returns 1, 
//if it is smaller, the function returns -1 and if they are even the function returns 0
int cmpr_float(void* a, void* b) 
{
    float floatA = *((float*)a);
    float floatB = *((float*)b);

    if (floatA > floatB)
    {
        return 1;
    }
    else if (floatA < floatB)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

//This function prints the data of an integer
void print_int(void* data) 
{
    int num = *((int*)data);
    printf("%d ", num);
}

//This function prints the data of an float
void print_float(void* data) 
{
    float num = *((float*)data);
    printf("%f ", num);
}

