#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include "constantes.h" /*Importar a biblioteca de constantes*/

typedef struct {
    double preco; 
    int quantidade;
    int vendido; /*Quantidade vendida*/
    char iva;
    char ean[EAN_MAX];
    char descricao[DESC_MAX];
} Produto;

typedef struct {
    int num;
    char c;
} IVA;

typedef struct {
    double preco;
    int quantidade;
    char ean[EAN_MAX];
} Cesto;

typedef struct{
    double valor_total;
    int id_fatura;
    int total_itens;
    int nif;
    char *nome_cliente;
} Fatura;

typedef struct{
    int nif;
    char nome;
} Cliente;

typedef struct{
    Produto tabela_produtos[PROD_MAX];
    IVA tabela_iva[IVA_MAX];
    Cesto *cesto;
    Fatura *lista_faturas;

    double total_faturado; /*Armazena o valor total faturado*/

    int produtos_por_ean[PROD_MAX]; /*Armazena os indices ordenados por ean*/
    int num_produtos; /*Armazena o número de diferentes produtos em stock*/
    int num_ivas; /*Armazena o total de valores de ivas*/
    int num_cesto; /*Armazena o número total de produtos diferentes no cesto*/
    int num_faturas; /*Armazena o número total de faturas existentes*/
    int num_faturas_emit; /*Armazena o número total de faturas já emitidas*/
    int capacidade_cesto; /*Armazena a capacidade atual do array dinamico cesto*/
    int capacidade_faturas; /*Armazena a capacidade atual do array dinamico lista_faturas*/
    int total_itens_vend; /*Armazeno o total de itens vendidos*/
} Sys;

#endif