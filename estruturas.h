/**
 * @file estruturas.h
 * @author Afonso Miguel Loureiro Sopa ist1118098
 * @brief Ficheiro ".h" dedicado ao armazenamento de todas as 
 * estruturas necessárias para a realização do projeto
 */

#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include "constantes.h" /*Importar a biblioteca de constantes*/

/**
 * @brief Estrutura que representa um produto no sistema
 */
typedef struct {
    long double preco; /**< Preço do produto */
    int quantidade; /**< Quantidade em stock do produto */
    int vendido; /**< Quantidade vendida do produto */
    char iva; /**< Taxa de IVA (caractér correspondente) */
    char ean[EAN_MAX]; /**< Código EAN correspondente ao produto */
    char descricao[DESC_MAX]; /**< Descrição do produto */
} Produto;

/**
 * @brief Estrutura que representa os IVAs
 */
typedef struct {
    int num; /**< Taxa de IVA (percentagem) */
    char c; /**< Caractér correspondente à taxa */
} IVA;

/**
 * @brief Estrutura que representa o cesto
 */
typedef struct {
    double preco; /**< Preco por unidade*/
    int quantidade; /**< Quantidade de produto no cesto */
    char ean[EAN_MAX]; /**< Código EAN do produto */
} Cesto;

/**
 * @brief Estrutura que representa a fatura
 */
typedef struct{
    double valor_total; /**< Valor total da fatura */
    int id_fatura; /**< ID da fatura */
    int total_itens; /**< Total de itens da fatura */
    int nif; /**< NIF do cliente */
    char *nome_cliente; /**< Nome do cliente (Alocado dinamicamente)*/
} Fatura;

/**
 * @brief Estrutura que representa os clientes
 */
typedef struct{
    int nif; /**< NIF do cliente */
    char nome; /**< Nome do cliente */
} Cliente;

/**
 * @brief Estrutura que representa o Sistema
 * *Tem como função juntar as variáveis globais (reconhecidas pelo main) e
 * facilitar a passagem de informação para dentro das funções comando.
 */
typedef struct{
    Produto tabela_produtos[PROD_MAX]; /**< Array com todos os produtos registados */
    IVA tabela_iva[IVA_MAX]; /**< Array com todas as taxas de IVA*/
    Cesto *cesto; /**< Array com todos os produtos no cesto */
    Fatura *lista_faturas; /**< Array com todas as faturas emitidas */

    double total_faturado; /**< Armazena o valor total faturado*/

    int produtos_por_ean[PROD_MAX]; /**< Armazena os indices ordenados por ean*/
    int num_produtos; /**< Armazena o número de diferentes produtos em stock*/
    int num_ivas; /**< Armazena o total de valores de ivas*/
    int num_cesto; /**< Armazena o número total de produtos diferentes no cesto*/
    int num_faturas; /**< Armazena o número total de faturas existentes*/
    int num_faturas_emit; /**< Armazena o número total de faturas já emitidas*/
    int capacidade_cesto; /**< Armazena a capacidade atual do array dinamico cesto*/
    int capacidade_faturas; /**< Armazena a capacidade atual do array dinamico lista_faturas*/
    int total_itens_vend; /**< Armazeno o total de itens vendidos*/
} Sys;

#endif