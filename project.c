/* iaed26 - ist1118098 - project */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define EAN_MAX 14 /* Número máx de caracteres do EAN */
#define DESC_MAX 51 /* Número máx de caracteres da descrição*/
#define STR_MAX 65536 /* Número máx de caracteres das strings*/
#define PROD_MAX 10000 /* Número máx de produtos*/
#define IVA_MAX 26 /* Número máx de taxas de IVA */
#define CONVERSAO '0' /* Constante de conversão de char para int (dígitos)*/

#define EAN_ERR "invalid ean" /* Erro: EAN inválido*/
#define IVA_ERR "invalid iva" /* Erro: IVA inválido*/
#define PRICE_ERR "invalid price" /* Erro: preço negativo*/
#define QUANT_ERR "invalid quantity" /* Erro: quantidade negativa*/
#define DESC_ERR "invalid description" /* Erro: descrição demasiado longa*/
#define PROD_IN_USE_ERR "product in use" /*Erro: produto em utilização*/
#define PROD_ERR "invalid product" /* Erro: */

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

void carregaIVA(int numero_palavras, char *lista_palavras[], IVA tabela_iva[], int *numero_ivas){
    /*Função para carregar os valores dos IVAs*/
    FILE *ficheiro_ivas = NULL; /*Assumir que o ficheiro está vazio*/

    if (numero_palavras > 1){
        /*Caso o utilizador tenha inserido algum ficheiro externo*/
        ficheiro_ivas = fopen(lista_palavras[1], "r"); /*Abrir o ficheiro*/
    }
    /*Verificar se o programa não está vazio*/
    if (ficheiro_ivas != NULL){
        char letra;
        int valor;
        
        /*fscanf retorna o número de argumentos lidos*/
        while (fscanf(ficheiro_ivas, " %c %d", &letra, &valor) == 2){
            tabela_iva[*numero_ivas].c = letra;
            tabela_iva[*numero_ivas].num = valor;
            (*numero_ivas)++;
            }
    fclose(ficheiro_ivas); /*Fechar o ficheiro*/
    }
    else{
        /*Caso não tenha sido inserido nenhum ficheiro*/
        tabela_iva[0].c = 'A'; tabela_iva[0].num = 0;
        tabela_iva[1].c = 'B'; tabela_iva[1].num = 6;
        tabela_iva[2].c = 'C'; tabela_iva[2].num = 13;
        tabela_iva[3].c = 'D'; tabela_iva[3].num = 23;
        *numero_ivas = 4; 
    }
}

int testaEAN(char ean[]){
    int tamanho = strlen(ean), i, soma = 0, digitoVer, valor, digitoCalc;
    /*Verificar o tamanho válido*/
    if (tamanho != 13 && tamanho != 8)
        return 1;

    /*Converter o dígito para inteiro*/
    digitoVer = ean[tamanho - 1] - CONVERSAO;

    /*Verificar o dígito de verificação*/
    for (i = 0; i < tamanho - 1; i++){
        /*Obter o dígito*/
        valor = ean[i] - CONVERSAO;

        if (i % 2 == 0) 
            /*Caso o indíce seja par*/
            soma += valor;
        
        else 
            /*Caso o indíce seja impar*/
            soma += (valor*3);
    }
    /*Calcular o dígito de verificação*/
    digitoCalc = (10 - (soma % 10)) % 10;
    
    /*Comparar o dígito calculado com o dígito introduzido*/
    if (digitoCalc != digitoVer)
        return 1;

    return 0;
}

int testaIVA(char iva, IVA tabela_iva[], int *numero_ivas){
    int i;
    for (i = 0; i < *numero_ivas; i++){
        if (iva == tabela_iva[i].c){
            return 0;
        }
    }
    return 1;
}

int testaDescricao(char descricao[]){
    /*Verifica se começa com maiuscula*/
    if (descricao[0] < 'A' || descricao[0] > 'Z')
        return 1;

    /*Verifica se a descricao tem menos de 50 bytes*/
    else if (strlen(descricao) > DESC_MAX)
        return 1;

    return 0;
}

int addProduto(Produto tabela_prod[], int *num, char linha[], IVA tabela_iva[], int *numero_ivas){ 
    /*Introduz um novo produto no sistema (key = p)
      p <ean> <iva> <preco> <quantidade> <descricao>  */

    /*Inicializar variáveis*/
    char comando, ean[EAN_MAX], iva, descricao[DESC_MAX];
    float preco;
    int quantidade, i;

    /*Obter o resto da linha*/
    sscanf(linha, " %c %s %c %f %d%[^\n]", &comando, ean, &iva, &preco, &quantidade, descricao);
     
    /*TESTES*/
    if (testaEAN(ean)){
        /*O código EAN é inválido*/
        puts(EAN_ERR);
        return 0;
    }
    else if (testaIVA(iva, tabela_iva, numero_ivas)){
        /*O caracter do IVA é inválido*/
        puts(IVA_ERR);
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
    else if (testaDescricao(descricao)){
        /*A descrição ultrapassa o tamanho máximo*/
        puts(DESC_ERR);
        return 0;
    }
    
    /*Verificar se o produto já existe na tabela de produtos*/
    for (i = 0; i < *num; i++){
        if (strcmp(tabela_prod[i].ean, ean)){
            /*Atualizar o stock*/
            tabela_prod[i].preco = preco;
            tabela_prod[i].quantidade = quantidade;
            tabela_prod[i].iva = iva;
            strcpy(tabela_prod[*num].descricao, descricao);
            return 0;
        }
    }

    /*TESTE*/
    if (*num >= PROD_MAX){
        /*A quantidade de produtos excede o máximo*/
        puts(PROD_ERR);
        return 0;
    }

    /*Adicionar os valores à tabela de produtos*/
    tabela_prod[*num].preco = preco;
    tabela_prod[*num].quantidade = quantidade;
    tabela_prod[*num].iva = iva;
    /*Copiar as string para a tabela */
    strcpy(tabela_prod[*num].ean, ean);  
    strcpy(tabela_prod[*num].descricao, descricao); 
    
    (*num)++;
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


int main(int num_palavras, char *lista_palavras[]){
    Produto tabela_prod[PROD_MAX]; /*Criar a tabela de produtos*/
    IVA tabela_iva[IVA_MAX]; /*Criar a tabela com os valores de IVA*/
    char input[STR_MAX]; /*Criar tabela para recolher o input*/
    int numero_produtos = 0, numero_ivas = 0;

    carregaIVA(num_palavras, lista_palavras, tabela_iva, &numero_ivas);

    while(fgets(input, STR_MAX, stdin)){
        switch(input[0]){
            case 'q': return 0;
            case 'p': addProduto(tabela_prod, &numero_produtos, input, tabela_iva, &numero_ivas); break;
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
