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

#define NAO_EXISTE -1 /*Estado para a não existência de um item num array*/
#define EXISTE 1 /*Estado para a existência de um item num array*/

#define EAN_ERR "invalid ean" /* Erro: EAN inválido*/
#define IVA_ERR "invalid iva" /* Erro: IVA inválido*/
#define PRICE_ERR "invalid price" /* Erro: preço negativo*/
#define QUANT_ERR "invalid quantity" /* Erro: quantidade negativa*/
#define DESC_ERR "invalid description" /* Erro: descrição demasiado longa*/
#define PROD_IN_USE_ERR "product in use" /*Erro: produto em utilização*/
#define PROD_ERR "invalid product" /* Erro: */
#define NO_PROD_ERR ": no such product" /*Erro: produto não encontrado*/
#define NO_STOCK_ERR "no stock" /*Erro: não existe stock*/

typedef struct {
    double preco; 
    int quantidade;
    int vendido; /*Quantidade vendida*/
    char iva;
    char ean[EAN_MAX];
    char descricao[DESC_MAX];
} Produto;

typedef struct {
    char c;
    int num;
} IVA;

typedef struct {
    int quantidade;
    char ean[EAN_MAX];
} Cesto;

/*Função que cria a tabela dos ivas (com base num ficheiro ou default)*/
void carregaIVA(int numero_palavras, char *lista_palavras[], IVA tabela_iva[], int *numero_ivas){
    /*Função para carregar os valores dos IVAs*/
    FILE *ficheiro_ivas = NULL; /*Assumir que o ficheiro está vazio*/

    if (numero_palavras > 1){
        /*Caso o utilizador tenha inserido algum ficheiro como argumento*/
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

/*Função auxiliar para testar a validade do código EAN*/
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

/*Função auxiliar para testar o valor do IVA*/
int testaIVA(char iva, IVA tabela_iva[], int *numero_ivas){
    int i;
    for (i = 0; i < *numero_ivas; i++){
        if (iva == tabela_iva[i].c){
            return 0;
        }
    }
    return 1;
}

/*Função auxiliar para testar a validade da descrição*/
int testaDescricao(char descricao[]){
    /*Verifica se começa com maiuscula*/
    if (descricao[0] < 'A' || descricao[0] > 'Z')
        return 1;

    /*Verifica se a descricao tem menos de 50 bytes*/
    if (strlen(descricao) >= DESC_MAX)
        return 1;

    return 0;
}

/*Função para adicionar o produto à tabela de produtos*/
int addProduto(Produto tabela_prod[], int *num, char linha[], IVA tabela_iva[], int *numero_ivas){ 
    /*Introduz um novo produto no sistema (key = p)
      p <ean> <iva> <preco> <quantidade> <descricao>  */

    /*Inicializar variáveis*/
    char comando, ean[EAN_MAX], iva, descricao[STR_MAX];
    float preco;
    int quantidade, i;

    /*Obter o resto da linha*/
    sscanf(linha, " %c %s %c %f %d %[^\n]", &comando, ean, &iva, &preco, &quantidade, descricao);
     
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
    else if (quantidade < 0){
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
        if (strcmp(tabela_prod[i].ean, ean) == 0){
            /*Atualizar o stock*/
            tabela_prod[i].preco = preco;
            tabela_prod[i].quantidade = quantidade;
            tabela_prod[i].iva = iva;
            strcpy(tabela_prod[i].descricao, descricao);
            printf("%d\n", quantidade);
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
    tabela_prod[*num].vendido = 0;
    tabela_prod[*num].iva = iva;
    /*Copiar as string para a tabela */
    strcpy(tabela_prod[*num].ean, ean);  
    strcpy(tabela_prod[*num].descricao, descricao); 
    
    (*num)++;

    printf("%d\n", quantidade);
    return 0;
}

/*Função auxiliar que tenta corresponder os tokens com codigos ean*/
int testaToken(char *token, char *ean){
    /*Caso base (o token e o código têm de terminar em simultâneo)*/
    if (*token == '\0')
        return (*ean == '\0');

    /*Caso o token seja '*'*/
    if (*token == '*')
        /* O token avança (1ª hipótese) ou o '*' salta um dígito do ean (2ª hipótese).
           Testa sempre a 1ª hipótese e caso essa seja inválida testa a 2ª*/
        return (testaToken(token + 1, ean) || 
                (*ean != '\0' && testaToken(token, ean + 1)));
    
    /*Se o token for um '?' ou se o token corresponder ao digito do ean*/
    if (*token != '\0' && (*token == '?' || *token == *ean))
        /*Avançar um espaço para ambos*/
        return testaToken(token + 1, ean + 1);
    return 0;
}

/*Função auxiliar para imprimir os produtos da tabela*/
void imprimeProduto(int indice, Produto tabela_produtos[]){
    printf("%s %c %.2f %d %d %s\n",
        tabela_produtos[indice].ean,
        tabela_produtos[indice].iva,
        tabela_produtos[indice].preco,
        tabela_produtos[indice].vendido,
        tabela_produtos[indice].quantidade,
        tabela_produtos[indice].descricao);
}

/*Função para imprimir os produtos listados pelo utilizador*/
void listaProduto(Produto tabela_produtos[], int *num, char linha[]){
    /*Imprime a info de todos os produtos (key = l)*/
    char *token, delimitadores[] = " \n\t";
    int i, verifica = 0;

    /*Primeiro corte na string (caracter l)*/
    token = strtok(linha, delimitadores);
    /*Segundo corte na string (tentar apanhar uma wildcard, se introduzida)*/
    token = strtok(NULL, delimitadores);

    /*Caso o utilizador apenas tenha inserido 'l' */
    if (token == NULL){
        for (i = 0; i < *num; i++){
            /*Apenas imprime se houver stock*/
            if (tabela_produtos[i].quantidade > 0){
                imprimeProduto(i, tabela_produtos);
                /*Atualizar a variável de verificação*/
                verifica = 1;
            }
        }
        if (verifica == 0)
            /*Imprimir mensagem de erro*/
            printf("*"NO_PROD_ERR"\n");
        return;
    }

    while (token != NULL){
        for (i = 0; i < *num; i++){
            /*Verifica se existe stock e se o EAN corresponde ao token*/
            if (tabela_produtos[i].quantidade > 0 && testaToken(token, tabela_produtos[i].ean)){
                imprimeProduto(i, tabela_produtos);
                /*Atualizar a variável de verificação*/
                verifica = 1;
            }
        }
        if (verifica == 0)
            printf("%s"NO_PROD_ERR"\n", token);
        
        token = strtok(NULL, delimitadores);
    }
}

/*Função auxiliar para procurar/verificar se existe um produto com um ean específico*/
int procuraProduto(Produto tabela_produtos[], int num_prod, char ean[]){
    int i;
    for (i = 0; i < num_prod; i++){
        if (strcmp(tabela_produtos[i].ean, ean) == 0) {
            return i;
        }
    }
    return NAO_EXISTE;
}

/*Função auxiliar para calcular o preco de um produto com IVA*/
double precoIVA(Produto tabela_produtos[], IVA tabela_iva[], int indice, int *num_ivas, int quant){
    double preco = tabela_produtos[indice].preco;
    int valor_iva, i;
    char iva_char = tabela_produtos[indice].iva;
    
    /*Descobrir o valor do iva*/
    for (i = 0; i < *num_ivas; i++){
        if (tabela_iva[i].c == iva_char)
            valor_iva = tabela_iva[i].num;
    }

    /*Calcular o preco dos produtos com a taxa de iva correspondente*/
    return preco * quant * (1.0 + (valor_iva / 100.0));
}

void addCesto(Produto tabela_produtos[], int *num_prod, char linha[], Cesto **cesto, int *num_cesto, IVA tabela_iva[], int *num_ivas, int *capacidade){
    /*Adiciona ao cesto a quantidade de produto indicado (key = a)*/
    int quant, input = 0, indice_tabela, i, j, quant_cesto = 0, indice_cesto = NAO_EXISTE, q_atual_cesto = 0;
    char comando, ean[EAN_MAX], arg1[EAN_MAX], arg2[EAN_MAX], delimitadores[] = " \n\t"; 
    double preco_com_iva;
    
    input = sscanf(linha, " %c %s %s", &comando, arg1, arg2);

    if (input == 1){
        Cesto temp;
        /*InsertionSort*/
        for (i = 1; i < *num_cesto; i++){
            temp = (*cesto)[i];
            j = i - 1;
            
            /*Verifica se a string da esquerda é maior do que a da direita*/
            while (j >= 0 && strcmp((*cesto)[j].ean, temp.ean) > 0){
                /*Trocar de posições*/
                (*cesto)[j+1] = (*cesto)[j];
                j--;
            }
            /*Inserir o item temp na posição certa*/
            (*cesto)[j+1] = temp;
        }

        for (i = 0; i < *num_cesto; i++){
            /*Obter o indice do produto*/
            indice_tabela = procuraProduto(tabela_produtos, *num_prod, (*cesto)[i].ean);

            /*Caso o indice exista calcular o preco com IVA*/
            if (indice_tabela != NAO_EXISTE)
                preco_com_iva = precoIVA(tabela_produtos, tabela_iva, indice_tabela, num_ivas, (*cesto)[i].quantidade);
            
            /*Apresentar as informações*/
            printf("%c %.2f %d %.2f %s\n", 
            tabela_produtos[indice_tabela].iva,
            tabela_produtos[indice_tabela].preco,
            (*cesto)[i].quantidade,
            preco_com_iva,
            tabela_produtos[indice_tabela].descricao);
        }
        return;
    }
    else if (input == 2){
        /*O utilizador não inseriu a quantidade*/
        quant = 1; /*Quantidade assume o valor 1*/
        strcpy(ean, arg1); /*Copiar o arg1 para o ean*/
    }
    else if (input == 3){
        /*O utilizador colocou os dados como esperado*/
        quant = atoi(arg1); /*Converter o arg1 para inteiro*/
        strcpy(ean, arg2); /*Copiar o arg2 para o ean*/
    }

    /*TESTES*/
    /*Testa se o EAN é válido*/
    if (testaEAN(ean)){
        puts(EAN_ERR);
        return;
    }
    
    /*Procura se o EAN existe no cesto*/
    for (i = 0; i < *num_cesto; i++){
        /*Comparar os EAN's do cesto com o introduzido*/
        if (strcmp((*cesto)[i].ean, ean) == 0){
            /*Obter os valores*/
            quant_cesto = (*cesto)[i].quantidade; 
            indice_cesto = i;
            break;
        }
    }

    /*Testa se existem artigos suficientes no cesto para serem removidos*/
    if (quant < 0 && quant_cesto < quant*(-1)){
        puts(QUANT_ERR); 
        return;
    }

    /*Obter o indíce do produto no armazém*/
    indice_tabela = procuraProduto(tabela_produtos, *num_prod, ean);

    /*Testa se o produto existe na lista_produtos*/
    if (indice_tabela == NAO_EXISTE){
        printf("%s"NO_PROD_ERR"\n", ean);
        return;
    }
    /*Testa se o produto existe em stock na lista_produtos*/
    if (quant > 0 && tabela_produtos[indice_tabela].quantidade < quant){
        puts(NO_STOCK_ERR);
        return;
    }

    /*-------------------------------------------*/
    /*Atualizar stock na tabela_produtos (remover quant)*/
    tabela_produtos[indice_tabela].quantidade -= quant;
    /*Atualizar a quantidade vendida*/
    tabela_produtos[indice_tabela].vendido += quant;

    /*Caso o item já esteja no cesto*/
    if (indice_cesto != NAO_EXISTE){
        /*Atualizar a quantidade*/
        (*cesto)[indice_cesto].quantidade += quant;
        q_atual_cesto = (*cesto)[indice_cesto].quantidade;
    }
    /*Caso o item não exista no cesto*/
    else if (indice_cesto == NAO_EXISTE){
        /*Caso a capacidade do array não seja suficiente para o número de valores no cesto*/
        if (*capacidade <= *num_cesto){
            if (*capacidade == 0)
                *capacidade = 2;
            /*Aumentar o tamanho do array "*cesto" para o dobro dos espaços*/
            *cesto = realloc(*cesto, (*capacidade) * 2 * sizeof(Cesto));
        }
        /*Inserir os dados no array cesto*/
        strcpy((*cesto)[*num_cesto].ean, ean);
        (*cesto)[*num_cesto].quantidade = quant;
        q_atual_cesto = quant;

        (*num_cesto)++;
    }
    
    preco_com_iva = precoIVA(tabela_produtos, tabela_iva, indice_tabela, num_ivas, q_atual_cesto);

    printf("%c %.2f %d %.2f %s\n", 
            tabela_produtos[indice_tabela].iva,
            tabela_produtos[indice_tabela].preco,
            q_atual_cesto,
            preco_com_iva,
            tabela_produtos[indice_tabela].descricao);

    return;
}

void resumoFat(){
    /*Resumo da Faturação (key = r)*/
    printf("Resumo\n");
}

void fatura(){
    /*Fatura os produtos no cesto (key = f)*/
    printf("Fatura\n");
}

void listaFat(){
    /*Lista as faturas do sistema (key = c)*/
    printf("Lista as faturas\n");    
}

void apagaRegisto(){
    /*Apaga o registo de um produto ou fatura (key = d)*/
    printf("Apaga o Registo\n");
}


int main(int num_palavras, char *lista_palavras[]){
    Produto tabela_prod[PROD_MAX]; /*Criar a tabela de produtos*/
    IVA tabela_iva[IVA_MAX]; /*Criar a tabela com os valores de IVA*/
    Cesto *cesto = NULL; /*Criar a tabela com os espaços para os produtos no cesto*/
    char input[STR_MAX]; /*Criar tabela para recolher o input*/
    int numero_produtos = 0, numero_ivas = 0, num_itens_cesto = 0, capacidade = 0;

    carregaIVA(num_palavras, lista_palavras, tabela_iva, &numero_ivas);

    while(fgets(input, STR_MAX, stdin)){
        switch(input[0]){
            case 'q': free(cesto); return 0;
            case 'p': addProduto(tabela_prod, &numero_produtos, input, tabela_iva, &numero_ivas); break;
            case 'l': listaProduto(tabela_prod, &numero_produtos, input); break;
            case 'a': addCesto(tabela_prod, &numero_produtos, input, &cesto, &num_itens_cesto, tabela_iva, &numero_ivas, &capacidade); break;
            case 'r': resumoFat(); break;
            case 'f': fatura(); break;
            case 'c': listaFat(); break;
            case 'd': apagaRegisto(); break;
            default: break;
        }
    }
    return 0;
}
