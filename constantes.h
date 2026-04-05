/**
 * @file constantes.h
 * @author Afonso Miguel Loureiro Sopa ist1118098
 * @brief Definição de limites, estados e mensagens de erros necessárias no projeto
 */

#ifndef CONSTANTES_H 
#define CONSTANTES_H

/***LIMITES E VALORES MÁXIMOS***/
#define EAN_MAX 14 /**< Número máx de caracteres do EAN */
#define DESC_MAX 51 /**< Número máx de caracteres da descrição*/
#define STR_MAX 65536 /**< Número máx de caracteres das strings*/
#define PROD_MAX 10000 /**< Número máx de produtos*/
#define IVA_MAX 26 /**< Número máx de taxas de IVA */
#define CONVERSAO '0' /**< Constante de conversão de char para int (dígitos)*/
#define CASAS 2 /**< Número de casas decimais restantes de arredondamentos*/

/***ESTADOS***/
#define NAO_EXISTE -1 /**< Estado para a não existência de um item num array*/
#define EXISTE 1 /**< Estado para a existência de um item num array*/
#define VALIDO 1 /**< Estado para confirmar a validade */
#define INVALIDO 0 /**< Estado para marcar uma variável como inválido */

/***MENSAGENS DE ERRO***/
#define EAN_ERR "invalid ean" /**< Erro: EAN inválido*/
#define IVA_ERR "invalid iva" /**< Erro: IVA inválido*/
#define PRICE_ERR "invalid price" /**< Erro: preço negativo*/
#define QUANT_ERR "invalid quantity" /**< Erro: quantidade negativa*/
#define DESC_ERR "invalid description" /**< Erro: descrição demasiado longa*/
#define PROD_IN_USE_ERR "product in use" /**< Erro: produto em utilização*/
#define PROD_ERR "invalid product" /**< Erro: quantidade excedente*/
#define NO_PROD_ERR ": no such product" /**< Erro: produto não encontrado*/
#define NO_STOCK_ERR "no stock" /**< Erro: não existe stock*/
#define NO_NIF_ERR ": no such nif" /**< Erro: NIF inválido*/
#define NAME_ERR "invalid name" /**< Erro: nome inválido*/
#define NO_CL_ERR ": no such client" /**< Erro: Cliente inexistente*/
#define NO_INVOICE ": no such invoice" /**< Erro: Não existe uma fatura com o id indicado*/

#endif