/* iaed26 - ist1118098 - project */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define EAN_MAX 14 /* Numero max de caracteres do EAN */
#define DESC_MAX 51 /* Numero max de caracteres do*/
#define STR_MAX 65536 /* Numero max de caractere das strings*/
#define PROD_MAX 10000 /* Numero max de produtos*/
#define CONVERSAO '0'

#define EAN_ERR "invalid ean"
#define IVA_ERR "invalid iva"
#define PRICE_ERR "invalid price"
#define QUANT_ERR "invalid quantity"
#define DESC_ERR "invalid description"
#define PROD_IN_USE_ERR "product in use"
#define PROD_ERR "invalid product"

typedef struct {
    double preco;
    int quantidade;
    char iva;
    char ean[EAN_MAX];
    char descricao[DESC_MAX];
} Produto;

typedef struct {
    char c;
    int num;
} IVA;


int testaEAN(char ean[]){
    int tamanho = strlen(ean), i, soma = 0, digitoVer, digitoCalc;
    /*Verificar o tamanho válido*/
    if (tamanho != 13 && tamanho != 8)
        return 1;

    digitoVer = ean[tamanho - 1] - CONVERSAO;

    /*Verificar o dígito de verificação*/
    for (i = 0; i < tamanho - 1; i++){
        int valor = ean[i] - CONVERSAO;

        if (i % 2 == 0) 
            /*Caso o digito seja par*/
            soma += valor;
        
        else 
            /*Caso o digito seja impar*/
            soma += (valor*3);
    }
    digitoCalc = (10 - (soma % 10)) % 10;
    
    if (digitoCalc != digitoVer)
        return 1;

    return 0;
}

int addProduto(Produto lista_prod[], int *num, char linha[]){ 
    /*Introduz um novo produto no sistema (key = p)
      p <ean> <iva> <preco> <quantidade> <descricao>  */

    char comando, ean[EAN_MAX], iva, descricao[DESC_MAX];
    float preco;
    int quantidade;

    /*Obter o resto da linha*/
    sscanf(linha, " %c %s %c %f %d %[^\n]", &comando, ean, &iva, &preco, &quantidade, descricao);
     
    /*TESTES*/
    if (testaEAN(ean)){
        /*O código EAN é inválido*/
        puts(EAN_ERR);
        return 0;
    }
    else if (preco <= 0){
        /*O preço é inferior/igual a 0*/
        puts(PRICE_ERR);
        return 0;
    }
    else if (quantidade <= 0){
        /*A quantidade introduzida é negativa*/
        puts(QUANT_ERR);
        return 0;
    }
    else if (strlen(descricao) > DESC_MAX){
        /*A descrição ultrapassa o tamanho máximo*/
        puts(DESC_ERR);
        return 0;
    }
    else if (*num >= PROD_MAX){
        /*A quantidade de produtos excede o máximo*/
        puts(PROD_ERR);
        return 0;
    }
    
    
    num++;
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
    Produto lista_prod[PROD_MAX];
    char input[STR_MAX];
    int numero_produtos = 0;

    while(fgets(input, PROD_MAX, stdin)){
        switch(input[0]){
            case 'q': return 0;
            case 'p': addProduto(lista_prod, &numero_produtos, input); break;
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
