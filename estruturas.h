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

    double total_faturado;

    int produtos_por_ean[PROD_MAX]; /*Guarda os indices ordenados por ean*/
    int num_produtos;
    int num_ivas;
    int num_cesto;
    int num_faturas;
    int num_faturas_emit;
    int capacidade_cesto;
    int capacidade_faturas;
    int total_itens_vend;
} Sys;

#endif