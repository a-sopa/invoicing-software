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
#define NO_NIF_ERR ": no such nif" /*Erro: NIF inválido*/
#define NAME_ERR "invalid name" /*Erro: nome inválido*/
#define NO_CL_ERR ": no such client" /*Erro: Cliente inexistente*/

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
        
        /*Ordenar os IVAS por ordem alfabética
                ***Insertion Sort***      */
        int i, j;
        IVA temp;
        
        for (i = 1; i < *numero_ivas; i++){
            temp = tabela_iva[i];
            j = i - 1;

            while (j >= 0 && tabela_iva[j].c > temp.c){
                tabela_iva[j+1] = tabela_iva[j];
                j--;
            }
            tabela_iva[j+1] = temp;
        }
        return;
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
    unsigned char primeira = (unsigned char)descricao[0];
    if (descricao[0] < 'A' || descricao[0] > 'Z')
        if (primeira <= 128)
            return 1;

    /*Verifica se a descricao tem menos de 50 bytes*/
    if (strlen(descricao) >= DESC_MAX)
        return 1;

    return 0;
}

/*Função para adicionar o produto à tabela de produtos*/
int addProduto(Produto tabela_prod[], int *num, char linha[], IVA tabela_iva[], int *numero_ivas, Cesto **cesto, int *num_cesto){ 
    /*Introduz um novo produto no sistema (key = p)
      p <ean> <iva> <preco> <quantidade> <descricao>  */

    /*Inicializar variáveis*/
    char comando, ean[EAN_MAX], iva, descricao[STR_MAX];
    double preco;
    int quantidade, i, j;

    /*Obter o resto da linha*/
    sscanf(linha, " %c %s %c %lf %d %[^\n]", &comando, ean, &iva, &preco, &quantidade, descricao);
     
    /*TESTES*/
    if (testaEAN(ean)){
        /*O código EAN é inválido*/
        puts(EAN_ERR);
        return 0;
    }
    if (testaIVA(iva, tabela_iva, numero_ivas)){
        /*O caracter do IVA é inválido*/
        puts(IVA_ERR);
        return 0;
    }
    if (preco <= 0){
        /*O preço é inferior/igual a 0*/
        puts(PRICE_ERR);
        return 0;
    }
    if (quantidade < 0){
        /*A quantidade introduzida é negativa*/
        puts(QUANT_ERR);
        return 0;
    }
    if (testaDescricao(descricao)){
        /*A descrição ultrapassa o tamanho máximo*/
        puts(DESC_ERR);
        return 0;
    }
    for (i = 0; i < *num; i++){
        if(strcmp(tabela_prod[i].ean, ean) == 0){
            /*Verificar se o preço do cesto é igual ao introduzido*/
            if (tabela_prod[i].preco != preco){
                for (j = 0; j < *num_cesto; j++){
                    if (strcmp((*cesto)[j].ean, ean) == 0){
                        puts(PROD_IN_USE_ERR);
                        return 0;
                    }
                }
        }
        /*Atualizar o stock*/
        tabela_prod[i].preco = preco;
        tabela_prod[i].quantidade += quantidade;
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
                verifica = EXISTE;
            }
        }
        if (verifica == 0)
            /*Imprimir mensagem de erro*/
            printf("*"NO_PROD_ERR"\n");
        return;
    }

    while (token != NULL){
        /*Reiniciar a variável*/
        verifica = 0;
        for (i = 0; i < *num; i++){
            /*Verifica se existe stock e se o EAN corresponde ao token*/
            if (tabela_produtos[i].quantidade > 0 && testaToken(token, tabela_produtos[i].ean)){
                imprimeProduto(i, tabela_produtos);
                /*Atualizar a variável de verificação*/
                verifica = EXISTE;
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
    int valor_iva = 0, i;
    char iva_char = tabela_produtos[indice].iva;
    
    /*Descobrir o valor do iva*/
    for (i = 0; i < *num_ivas; i++){
        if (tabela_iva[i].c == iva_char)
            valor_iva = tabela_iva[i].num;
    }

    /*Calcular o preco dos produtos com a taxa de iva correspondente*/
    return preco * quant * (1.0 + (valor_iva / 100.0));
}

/*Função para adicionar produtos ao cesto*/
void addCesto(Produto tabela_produtos[], int *num_prod, char linha[], Cesto **cesto, int *num_cesto, IVA tabela_iva[], int *num_ivas, int *capacidade){
    /*Adiciona ao cesto a quantidade de produto indicado (key = a)*/
    int quant = 0, input = 0, indice_tabela, i, j, quant_cesto = 0, indice_cesto = NAO_EXISTE, q_atual_cesto = 0;
    char comando, ean[EAN_MAX], arg1[EAN_MAX], arg2[EAN_MAX]; 
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
            if (indice_tabela != NAO_EXISTE){
                preco_com_iva = precoIVA(tabela_produtos, tabela_iva, indice_tabela, num_ivas, (*cesto)[i].quantidade);
                /*Apresentar as informações*/
                printf("%c %.2f %d %.2f %s\n", 
                tabela_produtos[indice_tabela].iva,
                tabela_produtos[indice_tabela].preco,
                (*cesto)[i].quantidade,
                preco_com_iva,
                tabela_produtos[indice_tabela].descricao);
            }
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
            (*capacidade) *= 2; /*Aumentar a capacidade*/
            *cesto = realloc(*cesto, (*capacidade) * sizeof(Cesto)); /*Alocar memória*/
        }
        /*Inserir os dados no array cesto*/
        strcpy((*cesto)[*num_cesto].ean, ean);
        (*cesto)[*num_cesto].quantidade = quant;
        q_atual_cesto = quant;

        (*num_cesto)++;
    }
    
    preco_com_iva = precoIVA(tabela_produtos, tabela_iva, indice_tabela, num_ivas, q_atual_cesto);
    
    if (indice_cesto == NAO_EXISTE){
        /*Se o item for novo no cesto é necessário guardá-lo no indice correspondente*/
        (*cesto)[*num_cesto-1].preco = preco_com_iva;
    }
    else{
        /*Se o produto já existia, basta apenas atualizar o valor do indice*/
        (*cesto)[indice_cesto].preco = preco_com_iva;
    }

    printf("%c %.2f %d %.2f %s\n", 
            tabela_produtos[indice_tabela].iva,
            tabela_produtos[indice_tabela].preco,
            q_atual_cesto,
            preco_com_iva,
            tabela_produtos[indice_tabela].descricao);

    return;
}

/*Função que imprime um resumo da faturação ao utilizador*/
void resumoFat(Produto tabela_produtos[], IVA tabela_iva[], char linha[], int *num_prod, int *num_ivas, int total_itens_vendidos, int total_faturas, double valor_faturado){
    /*Resumo da Faturação (key = r)*/
    char comando, ean[EAN_MAX];
    int input = 0, indice, i = 0;
    input = sscanf(linha, "%c %s", &comando, ean);

    /*Caso o utilizador não introduza argumentos */
    if (input == 1){
        /*num total itens comprados | numero total faturas emitidas | valor total faturado*/
        printf("%d %d %.2f\n", total_itens_vendidos, total_faturas, valor_faturado);
        /*Imprimir a tabela de ivas*/
        for (i = 0; i < *num_ivas; i++){
            printf("%c %d%%\n", 
            tabela_iva[i].c,
            tabela_iva[i].num);
        }
        return;
    } 

    /*Caso o utilizador introduza argumentos*/
    else{
        /*TESTES*/
        if (testaEAN(ean)){
            puts(EAN_ERR);
            return;
        }
        /*Obter o indice do produto*/
        indice = procuraProduto(tabela_produtos, *num_prod, ean);

        /*Verificar se o produto existe*/
        if (indice == NAO_EXISTE){
            printf("%s"NO_PROD_ERR"\n", ean);
            return;
        }
        /*Imprimir os dados sobre o produto*/
        printf("%d %d %s\n", 
            tabela_produtos[indice].quantidade,
            tabela_produtos[indice].vendido,
            tabela_produtos[indice].descricao
        );
        return;
    }   
}

void extraiNome(char linha[], int indice_i, char nome_final[]){
    int i = indice_i;
    int p = 0;
    
    while (isspace(linha[i])){
        i++; /*Incrementar o i até encontrar algo que não seja espaço*/
    }
    /*A linha não tem argumentos*/
    if (linha[i] != '\0'){
        nome_final[0] = '\0';
        return;
        }

    /*Caso o primeiro caractér seja uma aspa (")*/
    if (linha[i] == '"'){
        i++; /*Andar um espaço*/
        while (linha[i] != '"' && linha[i] != '\0'){
            nome_final[p] = linha[i];
            p++;
            i++;
        }
        nome_final[p] = '\0'; /*Fechar a string*/
    }
        else{
            /*Caso o nome não esteja delimitado por aspas*/
            strcpy(nome_final, &linha[i]);

            /*Limpar o fim da string*/
            int len = strlen(nome_final);
            if (len > 0 && nome_final[len-1] == '\n')
                nome_final[len-1] = '\0';
        }
    }

void fatura(Produto tabela_produtos[], Cesto **cesto, Fatura **lista_faturas, char linha[], int num_prod, int *num_cesto, int *num_faturas, int *capacidade_fat, int *total_itens, double *total_valor){
    /*Fatura os produtos no cesto (key = f)*/
    char nif_str[STR_MAX] = "999999999";
    char nome[STR_MAX] = "Cliente final";
    int i = 1; /* Começa no indíce 1 para ignorar o f*/
    int j, total_itens_fatura = 0, nif_num;
    double valor_fatura = 0.0;

    /*Encontrar os inputs corretos*/

    while (isspace(linha[i])){
        i++; /*Incrementar o i até encontrar algo que não seja espaço*/
    }
    /*Verificar se o input tem algum valor*/
    if (linha[i] != '\0'){
        /*Se o primeiro valor for um dígito será um nif*/
        if (isdigit(linha[i])) {
            sscanf(&linha[i], "%s", nif_str);
            i += strlen(nif_str);
        }
    }
    /*Chamar a função auxiliar para extrair o nome*/
    extraiNome(linha, i, nome);
    /*TESTES*/
    /*O NIF tem de ter obrigatoriamente 9 dígitos*/
    if (strlen(nif_str) != 9){
        printf("%s"NO_NIF_ERR"\n", nif_str);
        return;
    }
    /*"Converter" o NIF para inteiro*/
    nif_num = atoi(nif_str);

    /*O primeiro elemento do nome de cliente tem de ser uma letra*/
    if (!isalpha(nome[0])){
        puts(NAME_ERR);
        return;
    }

    /*Caso o nome seja "error"*/
    if (strcmp(nome, "error") == 0){
        for (j = 0; j < *num_cesto; j++){
            /*Procurar o indice do produto na tabela*/
            int id_prod = procuraProduto(tabela_produtos, num_prod, (*cesto)[j].ean);
            if (id_prod != NAO_EXISTE){
                tabela_produtos[id_prod].vendido -= (*cesto)[j].quantidade; /*Remover as vendas*/
                tabela_produtos[id_prod].quantidade += (*cesto)[j].quantidade; /*Repor o stock*/
            }
        }
        /*Reiniciar o contador de itens no cesto*/
        (*num_cesto) = 0;
        return;
    }

    /*Calcular os valores totais*/
    for (i = 0; i < *num_cesto; i++){
        total_itens_fatura += (*cesto)[i].quantidade;
        valor_fatura += (*cesto)[i].preco;
    }

    /*Reservar memória*/
    if (*num_faturas >= *capacidade_fat){
        if (*capacidade_fat == 0){
            *capacidade_fat += 2;
        }
        /*Realocar novo espaço de memória*/
        (*capacidade_fat) *= 2; /*Atualizar a capacidade*/
        *lista_faturas = realloc(*lista_faturas, (*capacidade_fat) * sizeof(Fatura)); /*Alocar espaço de memória*/
    }

    /*Guardar Fatura*/
    (*lista_faturas)[*num_faturas].nif = nif_num; /*Guardar NIF*/

    /*Reservar espaço de memória para o nome cliente*/
    (*lista_faturas)[*num_faturas].nome_cliente = malloc(strlen(nome) + 1); 
    strcpy((*lista_faturas)[*num_faturas].nome_cliente, nome); /*Guardar nome do cliente (array dinâmico)*/

    (*lista_faturas)[*num_faturas].total_itens = total_itens_fatura; /*Guardar o numero total de itens na fatura*/
    (*lista_faturas)[*num_faturas].valor_total = valor_fatura; /*Guardar o valor monetário da fatura*/
    (*lista_faturas)[*num_faturas].id_fatura = *num_faturas + 1; /*Atualizar o id da fatura*/

    /*Atualizar variáveis globais*/
    *total_itens += total_itens_fatura; /*Atualizar o total de itens em faturas*/
    *total_valor += valor_fatura; /*Atualizar o valor total monetário em faturas*/

    /*Emitir Fatura*/
    printf("%d %.2f %d\n", total_itens_fatura, valor_fatura, (*num_faturas) + 1);

    (*num_faturas)++;
    (*num_cesto) = 0; /*Esvaziar o cesto*/
    return;
}

void listaFat(Fatura lista_faturas[], char linha[], int num_faturas){
    /*Lista as faturas do sistema (key = c)*/
    char nome[DESC_MAX];
    int i, argumento = NAO_EXISTE, validacao = NAO_EXISTE;

    /*Utilizar a função auxiliar para extrair o nome*/
    extraiNome(linha, 1, nome);

    /*Verificar se a função tem realmente nome*/
    if (strlen(nome) > 0){
        argumento = EXISTE;
    }

    /*Caso o utilizador tenha inserido o nome como argumento*/
    if (argumento == EXISTE){
        /*TESTES*/
        if (!isalpha(nome[0])){
        puts(NAME_ERR);
        return;
        }
        /*Procurar o cliente na lista de faturação*/
        for (i = 0; i < num_faturas; i++){
            if (strcmp(lista_faturas[i].nome_cliente, nome) == 0){
                printf("%d %.2f %s\n", 
                        lista_faturas[i].id_fatura,
                        lista_faturas[i].valor_total,
                        lista_faturas[i].nome_cliente);
                validacao = EXISTE;
            }
        }
        /*Verificar se o cliente existe*/
        if (validacao == NAO_EXISTE){
            printf("%s"NO_CL_ERR"\n", nome);
            return;
        }
    }
    /*Listar todas as faturas por ordem:
        1.ª Alfabeticamente
        2.ª Cronologicamente*/
    
    return;    
}

void apagaRegisto(){
    /*Apaga o registo de um produto ou fatura (key = d)*/
    printf("Apaga o Registo\n");
}

int main(int num_palavras, char *lista_palavras[]){
    Produto tabela_prod[PROD_MAX]; /*Criar a tabela de produtos*/
    IVA tabela_iva[IVA_MAX]; /*Criar a tabela com os valores de IVA*/
    Cesto *cesto = NULL; /*Criar a tabela com os espaços para os produtos no cesto*/
    Fatura *lista_faturas = NULL;
    char input[STR_MAX]; /*Criar tabela para recolher o input*/
    double valor_total_faturado = 0.0;
    int numero_produtos = 0, numero_ivas = 0, num_itens_cesto = 0;
    int capacidade = 0, numero_faturas = 0, capacidade_faturas = 0;
    int total_itens_vendidos = 0;

    carregaIVA(num_palavras, lista_palavras, tabela_iva, &numero_ivas);

    while(fgets(input, STR_MAX, stdin)){
        switch(input[0]){
            case 'q':{
                int i;
                /*Libertar a memória dos arrays dinâmicos*/
                for (i = 0; i < numero_faturas; i++){
                    free(lista_faturas[i].nome_cliente);
                }
                free(lista_faturas);
                free(cesto); 
                return 0;}
            case 'p': 
                addProduto(tabela_prod, &numero_produtos, input, 
                            tabela_iva, &numero_ivas, &cesto, &num_itens_cesto);
                break;
            case 'l': 
                listaProduto(tabela_prod, &numero_produtos, input); 
                break;
            case 'a': 
                addCesto(tabela_prod, &numero_produtos, input, &cesto, 
                        &num_itens_cesto, tabela_iva, &numero_ivas, &capacidade);
                break;
            case 'r': 
                resumoFat(tabela_prod, tabela_iva, input, &numero_produtos, &numero_ivas, 
                        total_itens_vendidos, numero_faturas, valor_total_faturado);
                break;
            case 'f': fatura(tabela_prod, &cesto, &lista_faturas, input, 
                            numero_produtos, &num_itens_cesto, &numero_faturas, 
                            &capacidade_faturas, &total_itens_vendidos, &valor_total_faturado); 
                break;
            case 'c': listaFat(lista_faturas, input, numero_faturas); break;
            case 'd': apagaRegisto(); break;
            default: break;
        }
    }
    return 0;
}