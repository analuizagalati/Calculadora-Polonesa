#include <stdio.h>
#include <stdlib.h>
#include "expressao.h"

int main(){
    float conta = getValorPosFixa("");
    char *in = getFormaInFixa("");
    printf("Resultado: %.2f\n", conta);
    printf("Infixa: %s", in);
}