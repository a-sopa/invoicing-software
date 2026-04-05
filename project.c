/* iaed26 - ist1118098 - project */

/**
 * @file project.c
 * @author Afonso Miguel Loureiro Sopa ist1118098
 * @brief Função main() do projeto
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

/**
 * @brief Função principal (main) do programa. 
 * *Inicializa as variáveis e estruturas do Sistema, carrega os IVAs através de um ficheiro 
 * (se não existir assume valores default) e processa os comandos do utilizador
 * @param num_palavras (int) Armazena o número de palavras escritas no terminal
 * @param lista_palavras Array de strings que contém os argumentos do terminal (possivelmente um ficheiro)
 * @return Retorna '0' após a execução bem sucedida e libertação de memória
 */
int main(int num_palavras, char *lista_palavras[]){
    Sys sys; /*Inicializar sys*/
    char input[STR_MAX]; /*Criar array para recolher o input*/
    
    /*Inicializar o Sistema*/
    inicializaSistema(&sys);
    /*Obter os valores dos IVAs*/
    carregaIVA(num_palavras, lista_palavras, sys.tabela_iva, &sys.num_ivas);

    while(fgets(input, STR_MAX, stdin)){
        switch(input[0]){
            case 'q':
                /*Apagar a memória e sair do programa*/
                limpaMemoria(&sys); 
                return 0;
            case 'p':
                /*Adicionar produto ao array*/ 
                addProduto(&sys, input);
                break;
            case 'l':
                /*Listar todos os produtos do array*/
                listaProduto(&sys, input); 
                break;
            case 'a': 
                /*Adicionar produto ao cesto*/
                addCesto(&sys, input);
                break;
            case 'r':
                /*Resumir as faturas*/ 
                resumoFat(&sys ,input);
                break;
            case 'f':
                /*Faturar os produtos no cesto*/
                fatura(&sys, input); 
                break;
            case 'c':
                /*Lista as faturas do cliente*/
                listaFat(&sys, input); 
                break;
            case 'd':
                /*Apaga produto do array/cesto*/
                apagaRegisto(&sys, input); 
                break;
            default: 
                break;
        }
    }
    return 0;
}