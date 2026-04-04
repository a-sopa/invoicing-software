#ifndef AUXILIARES_H
#define AUXILIARES_H

#include "estruturas.h"

double arredonda2(double preco);
void carregaIVA(int numero_palavras, char *lista_palavras[], IVA tabela_iva[], int *numero_ivas);
void extraiNome(char linha[], int inicio, char nome[]);
 

#endif