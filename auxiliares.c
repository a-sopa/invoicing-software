/**
 * @file auxiliares.c
 * @author Afonso Miguel Loureiro Sopa ist1118098
 * @brief Funções auxiliares do projeto (não reconhecidas pelo main)
 */

/*Importar bibliotecas*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "estruturas.h" /*Importar biblioteca de estruturas*/
#include "constantes.h" /*Importar biblioteca de constantes*/

/**
 * @brief Inicializa todas as variáveis globais do projeto.
 * @param sys Ponteiro para a estrutura principal do Sistema*/
void inicializaSistema(Sys *sys){
    /*Inicializar cada um dos parâmetros de Sys*/
    sys->cesto = NULL; /*Criar a tabela com os espaços para os produtos no cesto*/
    sys->lista_faturas = NULL; /*Criar a tabela para listar as faturas*/
    /*Inicializar todos os contadores a 0*/    
    sys->total_faturado = 0.0;
    sys->num_produtos = 0;
    sys->num_ivas = 0;
    sys->num_cesto = 0;
    sys->capacidade_cesto = 0;
    sys->num_faturas = 0; 
    sys->capacidade_faturas = 0;
    sys->total_itens_vend = 0;
    sys->num_faturas_emit = 0;
}

/**
 * @brief Recebe um preço (double) e arredonda-o para CASAS casas decimais.
 * @param preco valor a arredondar.
 * @return Retorna o preço arredondado.
 */
double arredonda(double preco){
    long long centimos, i;
    double preco_final, multiplicador = 1.0;

    /*Obter o multiplicador de acordo com as casas decimais desejadas*/
    for (i = 0; i < CASAS; i++){
        multiplicador *= 10.0;
    }
    /*Calcular o valor dos cêntimos (Ficar apenas com a parte inteira)*/
    centimos = (long long)(preco * multiplicador + 0.5 + 1e-6); /*Somar 1e-9 para evitar certos erros (5.50 = 5.49999)*/
    preco_final = centimos / multiplicador;

    return preco_final;
}

/**
 * @brief Cria a tabela dos ivas (com base num ficheiro ou default)
 * *A função verifica se o programa recebe um argumento:
 * Caso receba um argumento -> Assume que será um ficheiro. Tenta abri-lo e 
 *  extrair o seu conteúdo (só lê conteúdo com um formato específico). Quando
 *  o ficheiro terminar, a função ordena por ordem alfabética os IVAs
 * Caso não receba argumentos -> Assume 4 valores default para os IVAs.
 * @param numero_palavras Número de palavras na linha de input do terminal
 * @param lista_palavras Array com o que o utilizador introduziu no terminal
 * @param tabela_iva Array no qual são guardados os valores do IVA
 * @param numero_ivas Contador: número de IVAs que o programa tem 
 */
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

/**
 * @brief Testar a validade do código EAN.
 * *Testes: 
 * - O tamanho do ean tem de ser 8 ou 13 caracteres
 * - Verifica se o dígito de segurança é válido
 * @param ean String de caracteres
 * @return A validade do ean (VALIDO/INVALIDO)
 */
int testaEAN(char ean[]){
    int tamanho = strlen(ean), i, soma = 0, digitoVer, valor, digitoCalc;
    /*Verificar o tamanho válido*/
    if (tamanho != 13 && tamanho != 8)
        return INVALIDO;

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
        return INVALIDO;

    return VALIDO;
}

/**
 * @brief Testar a validade do IVA.
 * *Testes:
 *  - O caracter introduzido tem de estar no array
 * @param iva Char correspondente à taxa de IVA
 * @param tabela_iva Array com todos os ivas
 * @param numero_ivas Contador: Número de ivas totais
 * @return Retorna a validade do IVA (VALIDO/INVALIDO)
 */
int testaIVA(char iva, IVA tabela_iva[], int numero_ivas){
    int i;
    for (i = 0; i < numero_ivas; i++){
        if (iva == tabela_iva[i].c){
            return VALIDO;
        }
    }
    return INVALIDO;
}

/**
 * @brief Testa a validade da descrição.
 * *Testes:
 *  - A primeira letra é maiúscula
 *  - O comprimento é menor que DESC_MAX 
 * @param descricao String composta por caracteres e espaços
 * @return Retorna a validade da descricao (VALIDO/INVALIDO) 
 */
int testaDescricao(char descricao[]){
    /*Verifica se começa com maiuscula*/
    unsigned char primeira = (unsigned char)descricao[0];
    if (descricao[0] < 'A' || descricao[0] > 'Z')
        if (primeira <= 128)
            return INVALIDO;

    /*Verifica se a descricao tem menos de 50 bytes*/
    if (strlen(descricao) >= DESC_MAX)
        return INVALIDO;

    return VALIDO;
}

/**
 * @brief Pesquisa na tabela_produtos
 * * Esta função utiliza pesquisa binária para procurar o produto.
 * @param sys Ponteiro para a estrutura principal do Sistema
 * @param ean String de caracteres
 * @return indice do produto (-1 caso não exista)
 */
int procuraProduto(Sys *sys, char ean[]){
    int esq = 0; 
    int dir = sys->num_produtos - 1;
    int meio, indice, comparacao;

    while (esq <= dir){
        /*Obter o meio*/
        meio = (esq + dir) / 2;

        /*Retirar o indice (da ordem cronologica) do valor do meio*/
        indice = sys->produtos_por_ean[meio];

        comparacao = strcmp(sys->tabela_produtos[indice].ean, ean);

        /*Caso os ean sejam iguais*/
        if (comparacao == 0){
            return indice;
        }
        /*Caso o ean seja maior que o meio*/
        else if (comparacao < 0){
            esq = meio + 1;
        }
        /*Caso o ean seja menor que o meio*/
        else{
            dir = meio - 1;
        }
    }
    return NAO_EXISTE;
}

/**
 * @brief Tenta corresponder os tokens com codigos ean.
 * * O caracter '?' pode ser qualquer número que esteja naquela posição
     O caracter '*' ignora o que vem a seguir até encontrar uma correspondência
        exata ao que está à direita dessa instrução
 * @param token String de caracteres correspondente a parte do input
 * @param ean String à qual queremos corresponder
 */
int testaToken(char *token, char *ean){
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
            return INVALIDO;
        }
    }
    /*Ignorar asteriscos que estejam no fim do token*/
    while (token[j] == '*'){
        j++;
    }
    /*Se o programa chegou ao fim do token, houve correspondência*/
    return (token[j] == '\0');
}

/**
 * @brief Imprime os produtos da tabela_produtos.
 * @param indice Inteiro correspondente ao índice na tabela
 * @param tabela_produtos Array que contém a informação a ser impressa
 */
void imprimeProduto(int indice, Produto tabela_produtos[]){
    printf("%s %c %.2Lf %d %d %s\n",
        tabela_produtos[indice].ean,
        tabela_produtos[indice].iva,
        tabela_produtos[indice].preco,
        tabela_produtos[indice].vendido,
        tabela_produtos[indice].quantidade,
        tabela_produtos[indice].descricao);
}

/**
 * @brief Calcula o preco de um produto com IVA.
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém a tabela_produtos
 * @param indice Inteiro correspondente ao indice no array
 * @param quant Inteiro correspondente à quantidade de produto
 * @return Valor arredondado do preco já com o IVA
 */
double precoIVA(Sys *sys, int indice, int quant){
    double preco = sys->tabela_produtos[indice].preco;
    int valor_iva = 0, i;
    char iva_char = sys->tabela_produtos[indice].iva;
    
    /*Descobrir o valor do iva*/
    for (i = 0; i < sys->num_ivas; i++){
        if (sys->tabela_iva[i].c == iva_char)
            valor_iva = sys->tabela_iva[i].num;
    }

    /*Calcular o preco dos produtos com a taxa de iva correspondente*/
    return arredonda(preco * quant * (1.0 + (valor_iva / 100.0)));
}

/**
 * @brief Retira o nome da linha de input
 * * A função avança na string a partir de um índice inicial, ignorando espaços em branco. 
 * Se o nome estiver delimitado por aspas (""), extrai apenas o conteúdo interno. 
 * Caso as aspas não sejam fechadas pelo utilizador, invalida a leitura (retornando "!"). 
 * Se o nome não contiver aspas, copia o restante texto até ao fim da linha.  
 * Se não encontrar nenhum argumento retorna uma string vazia.
 * @param linha String com a linha de input completa introduzida pelo utilizador
 * @param indice_i Inteiro com o indice inicial
 * @param nome_final String onde se vai armazenar o nome
 */
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

/**
 * @brief Testa a validade do nif
 * *Testes:
 *   - O nif só pode ter dígitos
 *   - O comprimento do nif tem de ser 9
 *   - O nif não pode começar por '0'
 * @param nif String que inclui o nif
 * @return Validade do nif (VALIDO/INVALIDO)
 */
int testanif(char nif[]){
    int i = 0;
    /*Verificar se o nif tem algum caracter que não seja um digito*/
    for (i = 0; nif[i] != '\0'; i++){
        if (!isdigit(nif[i])){
            return INVALIDO;
        }
    }
    /*O NIF tem de ter obrigatoriamente 9 dígitos*/
    if (strlen(nif) != 9 || nif[0] == '0'){
        return INVALIDO;
    }
    return VALIDO;
}

/**
 * @brief Pesquisa binária segundo o id de fatura
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém as faturas
 * @param id_input ID que é preciso encontrar
 * @return Retorna o indíce no array (NAO_EXISTE caso não haja correspondência)*/ 
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

/**
 * @brief Remove uma fatura com um certo id do array
 * *Esta função testa também se o id introduzido é válido
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém as faturas
 * @param id_fatura ID que é preciso corresponder
 */
void removeFatura(Sys *sys, int id_fatura){
    int i, indice_fat;
    
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

/**
 * @brief Remove uma quantidade de produto com um certo ean do array
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém os produtos
 * @param ean String que contém o ean a encontrar
 * @param quant Inteiro correpondente à quantidade de produto a ser removida
 */
void removeProduto(Sys *sys, char ean[], int quant){
    /*Atribuir os valores dos argumentos às variáveis*/
    int indice_prod, i, indice_rem = 0;

    /*Testar a validade do EAN*/
    if (testaEAN(ean) == INVALIDO){
        printf(EAN_ERR"\n");
        return;
    }
    /*Testar se o ean corresponde a algum produto*/
    indice_prod = procuraProduto(sys, ean);

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
    
    /*Caso o stock seja zero*/
    if (sys->tabela_produtos[indice_prod].quantidade == 0){
        /*Remover o produto da tabela_produtos*/
        for (i = indice_prod; i < sys->num_produtos - 1; i++){
            sys->tabela_produtos[i] = sys->tabela_produtos[i+1];
        }
        /*Encontrar o indice no array ordenado*/
        for (i = 0; i < sys->num_produtos; i++){
            if (sys->produtos_por_ean[i] == indice_prod){
                indice_rem = i;
                break;
            }
        }
        /*Remover o item e passar todos os maiores 1 indice para a esquerda*/
        for (i = indice_rem; i < sys->num_produtos - 1; i++){
            sys->produtos_por_ean[i] = sys->produtos_por_ean[i+1];
        }

        /*Atualizar os indices: Os indices maiores do que o removido têm de subtrair 1*/
        for (i = 0; i < sys->num_produtos - 1; i++){
            if (sys->produtos_por_ean[i] > indice_prod){
                sys->produtos_por_ean[i]--;
            }
        }

        sys->num_produtos--; /*Reduzir o número total de produtos*/
    }
}

/**
 * @brief Liberta a memória dinâmica alocada antes de terminar o programa
 * *Liberta a memória de todas as faturas, nomes de clientes e do cesto
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém os arrays dinâmicos 
 */
void limpaMemoria(Sys *sys){
    int i;
    /*Libertar a memória dos nomes dos clientes*/
    for (i = 0; i < sys->num_faturas; i++){
        free(sys->lista_faturas[i].nome_cliente);
    }
    free(sys->lista_faturas); /*Libertar a memória da lista de faturas*/
    free(sys->cesto); /*Libertar a memória do cesto*/
}
