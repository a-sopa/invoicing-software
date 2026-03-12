/* iaed26 - ist1118098 - project */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define EAN_MAX 14 /* Numero max de caracteres do EAN */
#define DESC_MAX 51 /* Numero max de caracteres do*/
#define STR_MAX 65536 /* Numero max de caractere das strings*/
#define PROD_MAX 10000 /* Numero max de produtos*/

typedef struct {
    char ean[EAN_MAX];
    int iva;
    float preco;
    int quantidade;
    char descricao[DESC_MAX];
} produto;


int testaEAN(char ean[]){
    int tamanho = strlen(ean);
    if (tamanho != 13 && tamanho != 8)
        return 1;

    return 0;
}

int addProduto(produto lista_prod[], int *num){ 
    /*Introduz um novo produto no sistema (key = p)
      p <ean> <iva> <preco> <quantidade> <descricao>  */
    if (testaEAN){
        printf("invalid ean");
        return 1;
    }
    if (*num >= PROD_MAX){
        printf("invalid product");
        return 1;
    }


    char ean[EAN_MAX];
    char resto_linha[STR_MAX];

    fgets(resto_linha, sizeof(resto_linha), stdin);
    
    num++;
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
    produto lista_prod[PROD_MAX];
    char input[STR_MAX];
    int numero_produtos = 0;

    while(1){
        scanf(" %c", input);
        switch(input[0]){
            case 'q': return 0;
            case 'p': addProduto(lista_prod, &numero_produtos); break;
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
