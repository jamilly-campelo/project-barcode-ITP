#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef DEF
#define DEF

const char l_code[10][8] = {
    "0001101", 
    "0011001", 
    "0010011", 
    "0111101", 
    "0100011", 
    "0110001", 
    "0101111", 
    "0111011", 
    "0110111", 
    "0001011"
};

const char r_code[10][8] = {
    "1110010", 
    "1100110", 
    "1101100", 
    "1000010", 
    "1011100", 
    "1001110", 
    "1010000", 
    "1000100", 
    "1001000", 
    "1110100"
};

bool ultimo_digito(char *digitos);

#endif


