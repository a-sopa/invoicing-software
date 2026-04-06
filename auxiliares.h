#ifndef AUXILIARES_H
#define AUXILIARES_H

#include "estruturas.h" /*Importar a biblioteca de estruturas*/

void inicializaSistema(Sys *sys);
double arredonda(double preco);
void carregaIVA(int numero_palavras, char *lista_palavras[], IVA tabela_iva[], int *numero_ivas);
int testaEAN(char ean[]);
void extraiNome(char linha[], int inicio, char nome[]);
int testaIVA(char iva, IVA tabela_iva[], int numero_ivas);
int testaDescricao(char descricao[]);
int procuraProduto(Sys *sys, char ean[]);
int validaInputProd(Sys *sys, char ean[], char iva, double preco, int quantidade, char descricao[]);
void atualizaProduto(Sys *sys, int indice_prod, char ean[], char iva, double preco, int quantidade, char descricao[]);
void insereProduto(Sys *sys, char ean[], char iva, double preco, int quantidade, char descricao[]);
int testaToken(char *token, char *ean);
void imprimeProduto(int indice, Produto tabela_produtos[]);
void listaTodosProdutos(Sys *sys);
void processaToken(Sys *sys, char token[]);
void listaCesto(Sys *sys, int indice_tabela, int quantidade_cesto, double preco_com_iva);
int validaCesto(Sys *sys, char ean[], int quant, int *indice_tabela, int *indice_cesto);
double precoIVA(Sys *sys, int indice, int quant);
void atualizaCesto(Sys *sys, int indice_tabela, int indice_cesto, int quant);
void insereCesto(Sys *sys, int indice_tabela, char ean[], int quant);
void extraiNome(char linha[], int indice_i, char nome_final[]);
int testanif(char nif[]);
int testaInputFatura(char nif_str[], char nome[]);
void apagaCesto(Sys *sys);
void emiteFatura(Sys *sys, int nif_num, char nome[], int total_itens, double valor_fatura);
int pesquisaFat(Sys *sys, int id_input);
void removeFatura(Sys *sys, int id_fatura);
void removeProduto(Sys *sys, char ean[], int quant);
void limpaMemoria(Sys *sys);

#endif