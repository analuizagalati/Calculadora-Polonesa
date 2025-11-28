#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "expressao.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Conversão para radianos, usada nas funções trigonométricas
static float paraRad(float graus) {
    return (graus * (float)M_PI) / 180.0f;
}


static int ehOperadorBin(const char *tk) {
    return (!strcmp(tk,"+") || !strcmp(tk,"-") ||
            !strcmp(tk,"*") || !strcmp(tk,"/") ||
            !strcmp(tk,"%") || !strcmp(tk,"^"));
}

static int ehFuncao1Arg(const char *tk) {
    return (!strcmp(tk,"sen") || !strcmp(tk,"cos") ||
            !strcmp(tk,"tg")  || !strcmp(tk,"log") ||
            !strcmp(tk,"raiz"));
}

static float aplicaFuncao(const char *nome, float v) {
    if (!strcmp(nome,"sen"))  return sinf(paraRad(v));
    if (!strcmp(nome,"cos"))  return cosf(paraRad(v));
    if (!strcmp(nome,"tg"))   return tanf(paraRad(v));
    if (!strcmp(nome,"log"))  return log10f(v);
    if (!strcmp(nome,"raiz")) return sqrtf(v);
    return 0.0f;
}

float getValorPosFixa(char *entrada) {

    if (!entrada || *entrada == '\0')
        printf("Erro: dados invalidos\n");
        return 0.0f;

    float stack[256];
    int topo = 0;
    char temp[64];

    const char *p = entrada;

    while (*p) {

        // Ignorar espaços
        if (*p == ' ') {
            p++;
            continue;
        }

        // Extrair token
        int i = 0;
        while (*p && *p != ' ' && i < 63) {
            temp[i++] = *p++;
        }
        temp[i] = '\0';

        // Token é operador binário?
        if (ehOperadorBin(temp)) {
            if (topo < 2) return 0;

            float y = stack[--topo];
            float x = stack[--topo];

            switch (temp[0]) {
                case '+': stack[topo++] = x + y; break;
                case '-': stack[topo++] = x - y; break;
                case '*': stack[topo++] = x * y; break;
                case '/': stack[topo++] = x / y; break;
                case '%': stack[topo++] = fmodf(x, y); break;
                case '^': stack[topo++] = powf(x, y); break;
            }
        }

        // Token é função?
        else if (ehFuncao1Arg(temp)) {
            if (topo < 1) return 0;
            float x = stack[--topo];
            stack[topo++] = aplicaFuncao(temp, x);
        }

        // Token é número
        else {
            stack[topo++] = atof(temp);
        }
    }

    if (topo != 1) return 0;

    return stack[0];
}

static int prioridade(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': case '%': return 2;
        case '^': return 3;
    }
    return 9;
}

static int func1Arg(const char *t) {
    return (!strcmp(t,"sen") || !strcmp(t,"cos") ||
            !strcmp(t,"tg")  || !strcmp(t,"log") ||
            !strcmp(t,"raiz"));
}

char *getFormaInFixa(char *strPos) {

    if (!strPos) return NULL;

    char *pilhaExp[256];
    int   pilhaPrio[256];
    int topo = 0;

    char tok[64];
    const char *p = strPos;

    while (*p) {

        if (*p == ' ') { p++; continue; }

        int k = 0;
        while (*p && *p != ' ' && k < 63)
            tok[k++] = *p++;
        tok[k] = '\0';

        // operador binário
        if (strlen(tok)==1 && strchr("+-*/%^", tok[0])) {

            if (topo < 2) return NULL;

            // retirar operandos corretamente
            char *right = pilhaExp[--topo];
            int pr = pilhaPrio[topo];

            char *left = pilhaExp[--topo];
            int pl = pilhaPrio[topo];

            int prioOp = prioridade(tok[0]);

            char aBuf[512], bBuf[512];

            // colocar parenteses se necessário
            sprintf(aBuf, (pl < prioOp) ? "(%s)" : "%s", left);

            sprintf(bBuf,
                (pr < prioOp || (pr == prioOp && (tok[0]=='-' || tok[0]=='/')))
                ? "(%s)" : "%s", right
            );

            char resultado[1024];
            sprintf(resultado, "%s%s%s", aBuf, tok, bBuf);

            free(left);
            free(right);

            pilhaExp[topo] = strdup(resultado);
            pilhaPrio[topo] = prioOp;
            topo++;
        }

        else if (func1Arg(tok)) {

            if (topo < 1) return NULL;

            char *x = pilhaExp[--topo];

            char aux[1024];
            sprintf(aux, "%s(%s)", tok, x);
            free(x);

            pilhaExp[topo] = strdup(aux);
            pilhaPrio[topo] = 8; // prioridade alta
            topo++;
        }

        // número
        else {
            pilhaExp[topo] = strdup(tok);
            pilhaPrio[topo] = 9;
            topo++;
        }
    }

    if (topo != 1) return NULL;

    return pilhaExp[0];
}
