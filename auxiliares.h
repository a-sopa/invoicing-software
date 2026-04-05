#ifndef AUXILIARES_H
#define AUXILIARES_H

#include "estruturas.h"

double arredonda(double preco);
void carregaIVA(int numero_palavras, char *lista_palavras[], IVA tabela_iva[], int *numero_ivas);
int testaEAN(char ean[]);
void extraiNome(char linha[], int inicio, char nome[]);
int testaIVA(char iva, IVA tabela_iva[], int numero_ivas);
int testaDescricao(char descricao[]);
int procuraProduto(Sys *sys, char ean[]);
int testaToken(char *token, char *ean);
void imprimeProduto(int indice, Produto tabela_produtos[]);
double precoIVA(Sys *sys, int indice, int quant);
void extraiNome(char linha[], int indice_i, char nome_final[]);
int testanif(char nif[]);
int pesquisaFat(Sys *sys, int id_input);
void limpaMemoria(Sys *sys);

#endif