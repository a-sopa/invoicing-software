/* iaed26 - ist1118098 - project */
/*Importar bibliotecas*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "constantes.h" /*Importar a biblioteca de constantes*/
#include "estruturas.h" /*Importar a biblioteca de estruturas*/


/*Função auxiliar para o arredondamento de floats*/
double arredonda2(double preco){
    long long centimos, i;
    double preco_final, multiplicador = 1.0;

    /*Obter o multiplicador de acordo com as casas decimais desejadas*/
    for (i = 0; i < CASAS; i++){
        multiplicador *= 10.0;
    }
    /*Calcular o valor dos cêntimos (Ficar apenas com a parte inteira)*/
    centimos = (long long)(preco * multiplicador + 0.5 + 1e-9); /*Somar 1e-9 para evitar certos erros (5.50 = 5.49999)*/
    preco_final = centimos / multiplicador;

    return preco_final;
}

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
int testaIVA(char iva, IVA tabela_iva[], int numero_ivas){
    int i;
    for (i = 0; i < numero_ivas; i++){
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
void addProduto(Sys *sys, char linha[]){ 
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
        printf(EAN_ERR"\n");
        return;
    }
    if (testaIVA(iva, sys->tabela_iva, sys->num_ivas)){
        /*O caracter do IVA é inválido*/
        printf(IVA_ERR"\n");
        return;
    }
    if (preco <= 0){
        /*O preço é inferior/igual a 0*/
        printf(PRICE_ERR"\n");
        return;
    }
    if (quantidade < 0){
        /*A quantidade introduzida é negativa*/
        printf(QUANT_ERR"\n");
        return;
    }
    if (testaDescricao(descricao)){
        /*A descrição ultrapassa o tamanho máximo*/
        printf(DESC_ERR"\n");
        return;
    }
    for (i = 0; i < sys->num_produtos; i++){
        if(strcmp(sys->tabela_produtos[i].ean, ean) == 0){
            /*Verificar se o preço do cesto é igual ao introduzido*/
            if (sys->tabela_produtos[i].preco != preco){
                for (j = 0; j < sys->num_cesto; j++){
                    if (strcmp((sys->cesto)[j].ean, ean) == 0){
                        printf(PROD_IN_USE_ERR"\n");
                        return;
                    }
                }
        }
        /*Atualizar o stock*/
        sys->tabela_produtos[i].preco = preco;
        sys->tabela_produtos[i].quantidade += quantidade;
        sys->tabela_produtos[i].iva = iva;
        strcpy(sys->tabela_produtos[i].descricao, descricao);
        printf("%d\n", sys->tabela_produtos[i].quantidade);
        return;
        }       
    }  

    /*TESTE*/
    if (sys->num_produtos >= PROD_MAX){
        /*A quantidade de produtos excede o máximo*/
        printf(PROD_ERR"\n");
        return;
    }

    /*Adicionar os valores à tabela de produtos*/
    sys->tabela_produtos[sys->num_produtos].preco = preco;
    sys->tabela_produtos[sys->num_produtos].quantidade = quantidade;
    sys->tabela_produtos[sys->num_produtos].vendido = 0;
    sys->tabela_produtos[sys->num_produtos].iva = iva;
    /*Copiar as string para a tabela */
    strcpy(sys->tabela_produtos[sys->num_produtos].ean, ean);  
    strcpy(sys->tabela_produtos[sys->num_produtos].descricao, descricao); 
    
    sys->num_produtos++;

    printf("%d\n", quantidade);
    return;
}

/*Função auxiliar que tenta corresponder os tokens com codigos ean*/
int testaToken(char *token, char *ean){
    /*O caracter '?' pode ser qualquer número que esteja naquela posição
      O caracter '*' ignora o que vem a seguir até encontrar uma correspondência
        exata ao que está à direita dessa instrução*/
    int i = 0;
    int j = 0;
    int verificacao = 0;
    int indice = NAO_EXISTE;

    while (ean[i] != '\0'){
        /*Caso os caracteres sejam iguais ou o token tenha um '?'*/
        if (token[j] != '\0' && (token[j] == '?' || token[j] == ean[i])){
            i++;
            j++;
        }
        /*Caso o token seja um '*', guarda-se a posicao e incrementa-se o token*/
        else if (token[j] == '*'){
            indice = j;
            verificacao = i;
            j++;
        }
        /*Caso não haja correspondência mas tenha sido introduzido um '*' anteriormente*/
        else if (indice != NAO_EXISTE){
            /*Fazer o backtracking*/
            j = indice + 1;
            verificacao++;
            i = verificacao;
        }
        /*Caso não haja nenhuma correspondência*/
        else {
            return 0;
        }
    }
    /*Ignorar asteriscos que estejam no fim do token*/
    while (token[j] == '*'){
        j++;
    }
    /*Se o programa chegou ao fim do token, houve correspondência*/
    return (token[j] == '\0');
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
void listaProduto(Sys *sys, char linha[]){
    /*Imprime a info de todos os produtos (key = l)*/
    char *token, delimitadores[] = " \n\t";
    int i, verifica = 0;

    /*Primeiro corte na string (caracter l)*/
    token = strtok(linha, delimitadores);
    /*Segundo corte na string (tentar apanhar uma wildcard, se introduzida)*/
    token = strtok(NULL, delimitadores);

    /*Caso o utilizador apenas tenha inserido 'l' */
    if (token == NULL){
        for (i = 0; i < sys->num_produtos; i++){
            /*Apenas imprime se houver stock*/
            if (sys->tabela_produtos[i].quantidade > 0){
                imprimeProduto(i, sys->tabela_produtos);
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
        for (i = 0; i < sys->num_produtos; i++){
            /*Verifica se existe stock e se o EAN corresponde ao token*/
            if (sys->tabela_produtos[i].quantidade > 0 && testaToken(token, sys->tabela_produtos[i].ean)){
                imprimeProduto(i, sys->tabela_produtos);
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
double precoIVA(Produto tabela_produtos[], IVA tabela_iva[], int indice, int num_ivas, int quant){
    double preco = tabela_produtos[indice].preco;
    int valor_iva = 0, i;
    char iva_char = tabela_produtos[indice].iva;
    
    /*Descobrir o valor do iva*/
    for (i = 0; i < num_ivas; i++){
        if (tabela_iva[i].c == iva_char)
            valor_iva = tabela_iva[i].num;
    }

    /*Calcular o preco dos produtos com a taxa de iva correspondente*/
    return arredonda2(preco * quant * (1.0 + (valor_iva / 100.0)));
}

/*Função para adicionar produtos ao cesto*/
void addCesto(Sys *sys, char linha[]){
    /*Adiciona ao cesto a quantidade de produto indicado (key = a)*/
    int quant = 0, input = 0, indice_tabela, i, j, quant_cesto = 0, indice_cesto = NAO_EXISTE, q_atual_cesto = 0;
    char comando, ean[EAN_MAX], arg1[EAN_MAX], arg2[EAN_MAX]; 
    double preco_com_iva;
    Cesto temp, *cesto_ordenado;
    
    input = sscanf(linha, " %c %s %s", &comando, arg1, arg2);

    if (input == 1){
        /*InsertionSort*/
        cesto_ordenado = malloc(sys->num_cesto * sizeof(Cesto)); /*Criar uma cópia para ordenar o cesto*/
        
        for (i = 0; i < sys->num_cesto; i++){
            cesto_ordenado[i] = sys->cesto[i];
        }

        for (i = 1; i < sys->num_cesto; i++){
            temp = cesto_ordenado[i];
            j = i - 1;
            /*Verifica se a string da esquerda é maior do que a da direita*/
            while (j >= 0 && strcmp(cesto_ordenado[j].ean, temp.ean) > 0){
                /*Trocar de posições*/
                cesto_ordenado[j+1] = cesto_ordenado[j];
                j--;
            }
            /*Inserir o item temp na posição certa*/
            cesto_ordenado[j+1] = temp;
        }

        for (i = 0; i < sys->num_cesto; i++){
            /*Obter o indice do produto*/
            indice_tabela = procuraProduto(sys->tabela_produtos, sys->num_produtos, cesto_ordenado[i].ean);

            /*Caso o indice exista calcular o preco com IVA*/
            if (indice_tabela != NAO_EXISTE){
                preco_com_iva = precoIVA(sys->tabela_produtos, sys->tabela_iva, indice_tabela, sys->num_ivas, cesto_ordenado[i].quantidade);
                /*Apresentar as informações*/
                printf("%c %.2f %d %.2f %s\n", 
                sys->tabela_produtos[indice_tabela].iva,
                sys->tabela_produtos[indice_tabela].preco,
                cesto_ordenado[i].quantidade,
                preco_com_iva,
                sys->tabela_produtos[indice_tabela].descricao);
            }
        }
        free(cesto_ordenado); /*Libertar a memória do cesto_ordenado*/
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
        printf(EAN_ERR"\n");
        return;
    }
    
    /*Procura se o EAN existe no cesto*/
    for (i = 0; i < sys->num_cesto; i++){
        /*Comparar os EAN's do cesto com o introduzido*/
        if (strcmp(sys->cesto[i].ean, ean) == 0){
            /*Obter os valores*/
            quant_cesto = sys->cesto[i].quantidade; 
            indice_cesto = i;
            break;
        }
    }

    /*Testa se existem artigos suficientes no cesto para serem removidos*/
    if (quant < 0 && quant_cesto < quant*(-1)){
        printf(QUANT_ERR"\n"); 
        return;
    }

    /*Obter o indíce do produto no armazém*/
    indice_tabela = procuraProduto(sys->tabela_produtos, sys->num_produtos, ean);

    /*Testa se o produto existe na lista_produtos*/
    if (indice_tabela == NAO_EXISTE){
        printf("%s"NO_PROD_ERR"\n", ean);
        return;
    }
    /*Testa se o produto existe em stock na lista_produtos*/
    if (quant > 0 && sys->tabela_produtos[indice_tabela].quantidade < quant){
        printf(NO_STOCK_ERR"\n");
        return;
    }

    /*-------------------------------------------*/
    /*Atualizar stock na tabela_produtos (remover quant)*/
    sys->tabela_produtos[indice_tabela].quantidade -= quant;
    /*Atualizar a quantidade vendida*/
    sys->tabela_produtos[indice_tabela].vendido += quant;

    /*Caso o item já esteja no cesto*/
    if (indice_cesto != NAO_EXISTE){
        /*Atualizar a quantidade*/
        sys->cesto[indice_cesto].quantidade += quant;
        q_atual_cesto = sys->cesto[indice_cesto].quantidade;
    }
    /*Caso o item não exista no cesto*/
    else if (indice_cesto == NAO_EXISTE){
        /*Caso a capacidade do array não seja suficiente para o número de valores no cesto*/
        if (sys->capacidade_cesto <= sys->num_cesto){
            if (sys->capacidade_cesto == 0)
                sys->capacidade_cesto = 2;
            /*Aumentar o tamanho do array "*cesto" para o dobro dos espaços*/
            sys->capacidade_cesto *= 2; /*Aumentar a capacidade*/
            sys->cesto = realloc(sys->cesto, sys->capacidade_cesto * sizeof(Cesto)); /*Alocar memória*/
        }
        /*Inserir os dados no array cesto*/
        strcpy(sys->cesto[sys->num_cesto].ean, ean);
        sys->cesto[sys->num_cesto].quantidade = quant;
        q_atual_cesto = quant;

        sys->num_cesto++;
    }
    
    preco_com_iva = precoIVA(sys->tabela_produtos, sys->tabela_iva, indice_tabela, sys->num_ivas, q_atual_cesto);
    
    if (indice_cesto == NAO_EXISTE){
        /*Se o item for novo no cesto é necessário guardá-lo no indice correspondente*/
        sys->cesto[sys->num_cesto - 1].preco = preco_com_iva;
    }
    else{
        /*Se o produto já existia, basta apenas atualizar o valor do indice*/
        sys->cesto[indice_cesto].preco = preco_com_iva;
    }

    printf("%c %.2f %d %.2lf %s\n", 
            sys->tabela_produtos[indice_tabela].iva,
            sys->tabela_produtos[indice_tabela].preco,
            q_atual_cesto,
            preco_com_iva,
            sys->tabela_produtos[indice_tabela].descricao);

    return;
}

/*Função que imprime um resumo da faturação ao utilizador*/
void resumoFat(Sys *sys, char linha[]){
    /*Resumo da Faturação (key = r)*/
    char comando, ean[EAN_MAX];
    int input = 0, indice, i = 0;
    input = sscanf(linha, "%c %s", &comando, ean);

    /*Caso o utilizador não introduza argumentos */
    if (input == 1){
        /*num total itens comprados | numero total faturas emitidas | valor total faturado*/
        printf("%d %d %.2f\n", sys->total_itens_vend, sys->num_faturas_emit, sys->total_faturado);
        /*Imprimir a tabela de ivas*/
        for (i = 0; i < sys->num_ivas; i++){
            printf("%c %d%%\n", 
            sys->tabela_iva[i].c,
            sys->tabela_iva[i].num);
        }
        return;
    } 

    /*Caso o utilizador introduza argumentos*/
    else{
        /*TESTES*/
        if (testaEAN(ean)){
            printf(EAN_ERR"\n");
            return;
        }
        /*Obter o indice do produto*/
        indice = procuraProduto(sys->tabela_produtos, sys->num_produtos, ean);

        /*Verificar se o produto existe*/
        if (indice == NAO_EXISTE){
            printf("%s"NO_PROD_ERR"\n", ean);
            return;
        }
        /*Imprimir os dados sobre o produto*/
        printf("%d %d %s\n", 
            sys->tabela_produtos[indice].quantidade,
            sys->tabela_produtos[indice].vendido,
            sys->tabela_produtos[indice].descricao
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
    if (linha[i] == '\0'){
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
        /*Caso o utilizador não tenha fechado aspas*/
        if (linha[i] == '\0'){
            /*Invalidar a string*/
            strcpy(nome_final, "!");
            return;
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

void fatura(Sys *sys, char linha[]){
    /*Fatura os produtos no cesto (key = f)*/
    char nif_str[STR_MAX] = "999999999";
    char nome[STR_MAX];
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

    if (strlen(nome) == 0 && strcmp(nif_str, "999999999") == 0){
        strcpy(nome, "Cliente final");
    }

    /*TESTES*/
    /*O NIF tem de ter obrigatoriamente 9 dígitos*/
    if (strlen(nif_str) != 9 || nif_str[0] == '0'){
        printf("%s"NO_NIF_ERR"\n", nif_str);
        return;
    }
    /*"Converter" o NIF para inteiro*/
    nif_num = atoi(nif_str);

    /*O primeiro elemento do nome de cliente tem de ser uma letra*/
    if (!isalpha(nome[0])){
        printf(NAME_ERR"\n");
        return;
    }

    /*Caso o nome seja "error"*/
    if (strcmp(nome, "error") == 0){
        for (j = 0; j < sys->num_cesto; j++){
            /*Procurar o indice do produto na tabela*/
            int id_prod = procuraProduto(sys->tabela_produtos, sys->num_produtos, sys->cesto[j].ean);
            if (id_prod != NAO_EXISTE){
                sys->tabela_produtos[id_prod].vendido -= sys->cesto[j].quantidade; /*Remover as vendas*/
                sys->tabela_produtos[id_prod].quantidade += sys->cesto[j].quantidade; /*Repor o stock*/
            }
        }
        /*Reiniciar o contador de itens no cesto*/
        sys->num_cesto = 0;
        return;
    }

    /*Calcular os valores totais*/
    for (i = 0; i < sys->num_cesto; i++){
        if (sys->cesto[i].quantidade > 0){
            total_itens_fatura += sys->cesto[i].quantidade;
            valor_fatura += sys->cesto[i].preco;
        }
    }

    /*Reservar memória*/
    if (sys->num_faturas >= sys->capacidade_faturas){
        if (sys->capacidade_faturas == 0){
            sys->capacidade_faturas += 2;
        }
        /*Realocar novo espaço de memória*/
        (sys->capacidade_faturas) *= 2; /*Atualizar a capacidade*/
        sys->lista_faturas = realloc(sys->lista_faturas, sys->capacidade_faturas * sizeof(Fatura)); /*Alocar espaço de memória*/
    }

    /*Guardar Fatura*/
    sys->lista_faturas[sys->num_faturas].nif = nif_num; /*Guardar NIF*/

    /*Reservar espaço de memória para o nome cliente*/
    sys->lista_faturas[sys->num_faturas].nome_cliente = malloc(strlen(nome) + 1); 
    strcpy(sys->lista_faturas[sys->num_faturas].nome_cliente, nome); /*Guardar nome do cliente (array dinâmico)*/

    sys->lista_faturas[sys->num_faturas].total_itens = total_itens_fatura; /*Guardar o numero total de itens na fatura*/
    sys->lista_faturas[sys->num_faturas].valor_total = valor_fatura; /*Guardar o valor monetário da fatura*/
    sys->lista_faturas[sys->num_faturas].id_fatura = sys->num_faturas + 1; /*Atualizar o id da fatura*/

    /*Atualizar variáveis globais*/
    sys->total_itens_vend += total_itens_fatura; /*Atualizar o total de itens em faturas*/
    sys->total_faturado += valor_fatura; /*Atualizar o valor total monetário em faturas*/

    /*Emitir Fatura*/
    printf("%d %.2f %d\n", total_itens_fatura, valor_fatura, (sys->num_faturas) + 1);

    /*Aumentar o número de faturas existentes e emitidas*/
    sys->num_faturas++;
    sys->num_faturas_emit++;
    sys->num_cesto = 0; /*Esvaziar o cesto*/
    return;
}

void listaFat(Sys *sys, char linha[]){
    /*Lista as faturas do sistema (key = c)*/
    char nome[DESC_MAX];
    int i, j, argumento = NAO_EXISTE, validacao = NAO_EXISTE;
    Fatura *copia, temp;

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
        printf(NAME_ERR"\n");
        return;
        }
        /*Procurar o cliente na lista de faturação*/
        for (i = 0; i < sys->num_faturas; i++){
            if (strcmp(sys->lista_faturas[i].nome_cliente, nome) == 0){
                printf("%d %.2f %s\n", 
                        sys->lista_faturas[i].id_fatura,
                        sys->lista_faturas[i].valor_total,
                        sys->lista_faturas[i].nome_cliente);
                validacao = EXISTE;
            }
        }
        /*Verificar se o cliente existe*/
        if (validacao == NAO_EXISTE){
            printf("%s"NO_CL_ERR"\n", nome);
            return;
        }
    }
    else{
    /*Listar todas as faturas por ordem:
        1.ª Alfabeticamente
        2.ª Cronologicamente*/
        copia = malloc(sys->num_faturas * sizeof(Fatura));

        /*Copiar as faturas*/
        for (i = 0; i < sys->num_faturas; i++){
            copia[i] = sys->lista_faturas[i];
        }

        /*Insertion Sort por ordem alfabética*/
        for (i = 1; i < sys->num_faturas; i++){
            j = i - 1;
            temp = copia[i];
            while (j >= 0 && strcmp(copia[j].nome_cliente, temp.nome_cliente) > 0){
                copia[j+1] = copia[j];
                j--;
            }
            copia[j+1] = temp;
        }
        /*Apresentar as faturas*/
        for (i = 0; i < sys->num_faturas; i++){
            printf("%d %.2f %s\n", 
                copia[i].id_fatura,
                copia[i].valor_total,
                copia[i].nome_cliente);
        }
        
        free(copia);
    }
    return;    
}

/*Função auxiliar que realiza pesquisa binária segundo o id de fatura*/
int pesquisaFat(Sys *sys, int id_input){
    int esq = 0, dir = sys->num_faturas - 1;
    int meio;

    while (esq <= dir){
        /*Definir o meio*/
        meio = (esq + dir) / 2;

        if (sys->lista_faturas[meio].id_fatura == id_input){
            /*Caso os ids sejam correspondentes*/
            return meio;
        }

        if (sys->lista_faturas[meio].id_fatura < id_input){
            /*Caso o id procurado seja maior, logo tem de estar à direita*/
            esq = meio + 1;
        }
        else{
            /*Caso o id procurado seja menor, logo tem de estar à esquerda*/
            dir = meio - 1;
        }
    }
    
    return NAO_EXISTE; /*Caso o id não esteja na lista*/
}

void apagaRegisto(Sys *sys, char linha[]){
    /*Apaga o registo de um produto ou fatura (key = d)*/
    int input, quant, id_fatura, arg2, indice_fat, i, indice_prod;
    char comando, arg1[EAN_MAX], ean[EAN_MAX];
    input = sscanf(linha, " %c %s %d", &comando, arg1, &arg2);

    if (input == 2){
        /*Apenas remover nos itens comprados*/
        id_fatura = atoi(arg1);
        indice_fat = pesquisaFat(sys, id_fatura);

        if (indice_fat == NAO_EXISTE){
            printf("%d"NO_INVOICE"\n", id_fatura);
            return;
        }
        /*Apresentar os dados da fatura removida*/
        printf("%.2f %d %s\n", 
            sys->lista_faturas[indice_fat].valor_total,
            sys->lista_faturas[indice_fat].nif,
            sys->lista_faturas[indice_fat].nome_cliente);
        
        sys->total_itens_vend -= sys->lista_faturas[indice_fat].total_itens;
        sys->total_faturado -= sys->lista_faturas[indice_fat].valor_total;
        free(sys->lista_faturas[indice_fat].nome_cliente);
        
        /*Remover a fatura do array*/
        for (i = indice_fat; i < sys->num_faturas - 1; i++){
            sys->lista_faturas[i] = sys->lista_faturas[i+1];
        }

        sys->num_faturas--; /*Diminuir o número de faturas existentes*/
    }
    else if (input == 3){
        /*Atribuir os valores dos argumentos às variáveis*/
        strcpy(ean, arg1);
        quant = arg2;

        /*Testar a validade do EAN*/
        if (testaEAN(ean)){
            printf(EAN_ERR"\n");
            return;
        }
        /*Testar se o ean corresponde a algum produto*/
        indice_prod = NAO_EXISTE;
        for (int i = 0; i < sys->num_produtos; i++){
            if (strcmp(sys->tabela_produtos[i].ean, ean) == 0){
                indice_prod = i;
                break;
            }
        }
        /*Imprimir mensagem de erro caso o ean não corresponda a um produto*/
        if (indice_prod == NAO_EXISTE){
            printf("%s"NO_PROD_ERR"\n", ean);
            return;
        }
        /*Testar se o produto está no cesto*/
        for (int i = 0; i < sys->num_cesto; i++){
            if (strcmp(sys->cesto[i].ean, ean) == 0){
                printf(PROD_IN_USE_ERR"\n");
                return;
            }
        }

        /*Testar se a quantidade é válida */
        if (quant <= 0 || quant > sys->tabela_produtos[indice_prod].quantidade){
            printf(QUANT_ERR"\n");
            return;
        }
        
        /*Retirar a quantidade indicada*/
        sys->tabela_produtos[indice_prod].quantidade -= quant;

        /*Imprimir os dados atualizados*/
        printf("%d %s\n", 
                sys->tabela_produtos[indice_prod].quantidade,
                sys->tabela_produtos[indice_prod].descricao);
        
        /*Caso o stock seja zero, remover o produto da tabela_produtos*/
        if (sys->tabela_produtos[indice_prod].quantidade == 0){
            for (i = indice_prod; i < sys->num_produtos; i++){
                sys->tabela_produtos[i] = sys->tabela_produtos[i+1];
            }
            sys->num_produtos--; /*Reduzir o número total de produtos*/
        }
    }
}

//CORRIGIR ERRO f JOAO MOUTINHO -> é necessário aspas e tem de dar invalid nif
//CORRIGIR ERRO f cliente cliente -> invalid nif
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
            case 'q':{
                int i;
                /*Libertar a memória dos arrays dinâmicos*/
                for (i = 0; i < sys.num_faturas; i++){
                    free(sys.lista_faturas[i].nome_cliente);
                }
                free(sys.lista_faturas);
                free(sys.cesto); 
                return 0;}
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
            case 'f': fatura(&sys, input); 
                break;
            case 'c': listaFat(&sys, input); 
                break;
            case 'd': apagaRegisto(&sys, input); 
                break;
            default: 
                break;
        }
    }
    return 0;
}