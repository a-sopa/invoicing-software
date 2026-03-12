/* iaed26 - ist1118098 - project */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define EAN_MAX 14 /* Numero max de caracteres do EAN */
#define DESC_MAX 51 /* Numero max de caracteres do*/
#define STR_MAX 65536 /* Numero max de caractere das strings*/
#define VEC_MAX 10000 /* Numero max de produtos*/


int addProduto(){ 
    /*Adiciona/Atualiza um produto (key = p)*/
    char resto_linha[STR_MAX];

    fgets(resto_linha, sizeof(resto_linha), stdin);
    printf("%s", resto_linha);
    printf("Adicionou Produto\n");
}

int listaProduto(){
    /*Imprime a info de todos os produtos (key = l)*/
    printf("Lista de Produtos\n");
}

int addCesto(){
    /*Adiciona ao cesto a quantidade de produto indicado (key = a)*/
    printf("Adicionou ao Cesto\n");
}

int resumoFat(){
    /*Resumo da Faturação (key = r)*/
    printf("Resumo\n");
}

int fatura(){
    /*Fatura os produtos no cesto (key = f)*/
    printf("Fatura\n");
}

int listaFat(){
    /*Lista as faturas do sistema (key = c)*/
    printf("Lista as faturas\n");    
}

int apagaRegisto(){
    /*Apaga o registo de um produto ou fatura (key = d)*/
    printf("Apaga o Registo\n");
}


int main(){
    char input[STR_MAX];

    while(1){
        scanf(" %c", input);
        switch(input[0]){
            case 'q': return 0;
            case 'p': addProduto(); break;
            case 'l': listaProduto(); break;
            case 'a': addCesto(); break;
            case 'r': resumoFat(); break;
            case 'f': fatura(); break;
            case 'c': listaFat(); break;
            case 'd': apagaRegisto(); break;
            default: break;
        }
        
    }
    return 0;
}
