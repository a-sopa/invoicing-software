/* iaed26 - ist1118098 - project */

/**
 * @file project.c
 * @author Afonso Miguel Loureiro Sopa ist1118098
 * @brief Função main()
 */

/*Importar bibliotecas*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "constantes.h" /*Importar a biblioteca de constantes*/
#include "estruturas.h" /*Importar a biblioteca de estruturas*/
#include "auxiliares.h" /*Importar a biblioteca de funções auxiliares*/
#include "comandos.h" /*Importar a biblioteca das funções de comando*/


int main(int num_palavras, char *lista_palavras[]){
    Sys sys; /*Inicializar sys*/
    char input[STR_MAX]; /*Criar array para recolher o input*/
    
    /*Inicializar cada um dos parâmetros de Sys*/
    sys.cesto = NULL; /*Criar a tabela com os espaços para os produtos no cesto*/
    sys.lista_faturas = NULL; /*Criar a tabela para listar as faturas*/
    /*Inicializar todos os contadores a 0*/    
    sys.total_faturado = 0.0;
    sys.num_produtos = 0;
    sys.num_ivas = 0;
    sys.num_cesto = 0;
    sys.capacidade_cesto = 0;
    sys.num_faturas = 0; 
    sys.capacidade_faturas = 0;
    sys.total_itens_vend = 0;
    sys.num_faturas_emit = 0;

    /*Obter os valores dos IVAs*/
    carregaIVA(num_palavras, lista_palavras, sys.tabela_iva, &sys.num_ivas);

    while(fgets(input, STR_MAX, stdin)){
        switch(input[0]){
            case 'q':
                limpaMemoria(&sys); 
                return 0;
            case 'p': 
                addProduto(&sys, input);
                break;
            case 'l': 
                listaProduto(&sys, input); 
                break;
            case 'a': 
                addCesto(&sys, input);
                break;
            case 'r': 
                resumoFat(&sys ,input);
                break;
            case 'f':
                fatura(&sys, input); 
                break;
            case 'c':
                listaFat(&sys, input); 
                break;
            case 'd':
                apagaRegisto(&sys, input); 
                break;
            default: 
                break;
        }
    }
    return 0;
}