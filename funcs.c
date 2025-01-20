#include <stdbool.h>

bool ultimo_digito(char *digitos){
    int add_dig = 0;
    int Num_dig;
    int ulti_dig = -1;
    int mult = 10, contador = 1;

    for (int i = 1; i <= 7; ++i){
        if ( i % 2 == 0){
            Num_dig = digitos[i - 1] - '0';
            add_dig += 1 * Num_dig;
        }
        else{
            Num_dig = digitos[i - 1] - '0';
            add_dig += 3 * Num_dig;
        }
    }

    while ( ulti_dig < 0){
        ulti_dig = mult - add_dig;
        ++contador;
        mult = 10 * contador;
    }

    ulti_dig = ulti_dig + '0';

    if ( ulti_dig == digitos[7])
        return true;
    else
        return false;
}