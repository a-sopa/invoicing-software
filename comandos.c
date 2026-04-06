/* iaed26 - ist1118098 - project */
/**
 * @file comandos.c
 * @author Afonso Miguel Loureiro Sopa ist1118098
 * @brief Este ficheiro contém os comandos principais
 */

/*Importar bibliotecas*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "comandos.h" /*Importar a biblioteca das funções de comando*/
#include "auxiliares.h" /*Importar a biblioteca de funções auxiliares*/
#include "constantes.h" /*Importar a biblioteca de constantes*/
#include "estruturas.h" /*Importar a biblioteca de estruturas*/

/**
 * @brief Comando: 'p' -> Adiciona o produto à tabela de produtos.
 * * A função divide a linha (input) em vários argumentos: EAN, IVA, preço, quantidade e 
 * descrição do produto. Realiza testes para validar todos os parâmetros, devolvendo mensagens
 * de erro caso sejam inválidos, para depois guardar o produto no array tabela_produtos.
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém a tabela_produtos 
 * @param linha String com a linha de input completa introduzida pelo utilizador*/
void addProduto(Sys *sys, char linha[]){ 
    char comando, ean[STR_MAX], iva, descricao[STR_MAX];
    double preco;
    int quantidade, indice_prod;

    /*Obter o resto da linha*/
    sscanf(linha, " %c %s %c %lf %d %[^\n]", &comando, ean, &iva, &preco, &quantidade, descricao);
     
    if (validaInputProd(sys, ean,  iva,  preco, quantidade, descricao) == INVALIDO){
        return;
    }
    /*Verificar se o produto já existe na tabela_produtos*/
    indice_prod = procuraProduto(sys, ean);
    
    /*O Produto já existe na tabela*/
    if (indice_prod != NAO_EXISTE){
        atualizaProduto(sys, indice_prod, ean, iva, preco, quantidade, descricao);
        return;
    } 
    /*O Produto não existe na tabela*/
    insereProduto(sys, ean, iva, preco, quantidade, descricao);
    return;
}

/**
 * @brief Comando: 'l' -> Imprime os produtos listados pelo utilizador.
 * *Se a função apenas recebe o 'l' sem argumentos imprime todos os produtos da 
 * tabela_produtos com quantidade superior a zero. Caso seja inserido um ou mais códigos 
 * EAN como argumento (podendo incluir os wildcards '?' e '*'), a função procura e imprime
 * apenas os produtos com os códigos EAN correspondentes. 
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém a tabela_produtos 
 * @param linha String com a linha de input completa introduzida pelo utilizador*/
void listaProduto(Sys *sys, char linha[]){
    char *token, delimitadores[] = " \n\t";

    /*Primeiro corte na string (caracter l)*/
    token = strtok(linha, delimitadores);
    /*Segundo corte na string (tentar apanhar uma wildcard, se introduzida)*/
    token = strtok(NULL, delimitadores);

    /*Caso o utilizador apenas tenha inserido 'l' */
    if (token == NULL){
        listaTodosProdutos(sys);
        return;
    }

    while (token != NULL){
        processaToken(sys, token);
        /*Voltar a cortar a string à procura de mais argumentos*/
        token = strtok(NULL, delimitadores);
    }
}

/**
 * @brief Comando: 'a' -> Adiciona produtos ao cesto.
 * *A função processa a string de entrada do utilizador e extrai o EAN e a quantidade (quando não 
 * introduzida adiciona-se 1 unidade). De seguida, verifica se o produto existe no sistema e se a
 * sua quantidade em stock é suficiente. Após passar as validações, o produto é adicionado ao cesto
 * (ou a sua quantidade é atualizada, caso já tenha sido adicionado anteriormente), e o stock do
 * produto é reduzido. Caso o cesto atinja a sua capacidade máxima, a memória alocada é expandida.
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém a tabela_produtos e o cesto
 * @param linha String com a linha de input completa introduzida pelo utilizador*/ 
void addCesto(Sys *sys, char linha[]){
    int quant = 0, input = 0, indice_tabela = 0, i, indice_cesto = NAO_EXISTE;
    char comando, ean[STR_MAX], arg1[STR_MAX], arg2[STR_MAX]; 
    double preco_com_iva;
    
    input = sscanf(linha, " %c %s %s", &comando, arg1, arg2);
    /*Caso apenas tenha sido inserido o 'a'*/
    if (input == 1){
        for (i = 0; i < sys->num_cesto; i++){
            /*Obter o indice do produto*/
            indice_tabela = procuraProduto(sys, sys->cesto[i].ean);
            /*Caso o indice exista calcular o preco com IVA*/
            if (indice_tabela != NAO_EXISTE){
                preco_com_iva = precoIVA(sys, indice_tabela, sys->cesto[i].quantidade);
                /*Apresentar as informações*/
                listaCesto(sys, indice_tabela, sys->cesto[i].quantidade, preco_com_iva);
            }
        } return;
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
    if (validaCesto(sys, ean, quant, &indice_tabela, &indice_cesto) == INVALIDO) return;
    /*-------------------------------------------*/
    /*Atualizar stock na tabela_produtos (remover quant) e a quantidade vendida*/
    sys->tabela_produtos[indice_tabela].quantidade -= quant;
    sys->tabela_produtos[indice_tabela].vendido += quant;

    /*Caso o item já esteja no cesto*/
    if (indice_cesto != NAO_EXISTE)
        atualizaCesto(sys, indice_tabela, indice_cesto, quant); /*Atualizar a quantidade*/
    /*Caso o item não exista no cesto*/
    else if (indice_cesto == NAO_EXISTE)
        insereCesto(sys, indice_tabela, ean, quant); /*Inserir no cesto*/
}

/**
 * @brief Comando: 'r' -> Imprime um resumo da faturação total ou de um produto específico.
 * *Se a função for invocada sem argumentos, imprime as estatísticas globais:
 *      <total de itens vendidos> <número de faturas emitidas> <valor total faturado>;
 *  Caso seja invocada com um argumento, esse será o EAN (caso seja inválido será apresentada
 * uma mensagem de erro) e a função vai imprimir: 
 *      <quantidade-disponível> <quantidade-vendida> <descrição>, relativo ao produto.
 *  Independentemente do número de argumentos, depois de cada instrução são apresentadas 
 * todas as taxas de IVAs
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém a tabela_produtos
 * @param linha String com a linha de input completa introduzida pelo utilizador*/ 
void resumoFat(Sys *sys, char linha[]){
    char comando, ean[STR_MAX];
    int input = 0, indice, i = 0;
    input = sscanf(linha, "%c %s", &comando, ean);

    /*Caso o utilizador não introduza argumentos */
    if (input == 1){
        /*num total itens comprados | numero total faturas emitidas | valor total faturado*/
        printf("%d %d %.2f\n", 
            sys->total_itens_vend, 
            sys->num_faturas_emit, 
            sys->total_faturado);
        /*Imprimir a tabela de ivas*/
        for (i = 0; i < sys->num_ivas; i++)
            printf("%c %d%%\n", sys->tabela_iva[i].c, sys->tabela_iva[i].num);
        return;
    }
    /*Caso o utilizador introduza argumentos*/
    else{
        /*TESTES*/
        if (testaEAN(ean) == INVALIDO){
            printf(EAN_ERR"\n"); return;
        }
        /*Obter o indice do produto*/
        indice = procuraProduto(sys, ean);
        /*Verificar se o produto existe*/
        if (indice == NAO_EXISTE){
            printf("%s"NO_PROD_ERR"\n", ean); return;
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

/**
 * @brief Comando: 'f' -> Fatura todos os produtos que estão no cesto.
 * *A função divide o input em <nif> e <nome-de-cliente> (em caso de omissão nome = "Cliente final" e 
 * nif = "999999999"). Valida o nif e o formato do nome. Se o nome introduzido seja "error", a 
 * operação é anulada: o conteúdo do cesto é apagado e o stock volta para a tabela_produtos.
 * Caso contrário, calcula a quantidade e valor total, expande (se necessário) o tamanho do array
 * dinâmico das faturas (realloc) e aloca a memória exata para o nome do cliente (malloc).
 * Por último, guarda a fatura, atualiza as variáveis globais e esvazia o cesto. 
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém a tabela_produtos e o cesto
 * @param linha String com a linha de input completa introduzida pelo utilizador*/
void fatura(Sys *sys, char linha[]){
    char nif_str[STR_MAX] = "999999999"; 
    char nome[STR_MAX], arg1[STR_MAX], arg2[STR_MAX];
    int i = 1; /* Começa no indíce 1 para ignorar o f*/
    int total_itens_fatura = 0, nif_num, num_args;
    double valor_fatura = 0.0;

    while (isspace(linha[i]))
        i++; /*Incrementar o i até encontrar algo que não seja espaço*/
    /*Verificar se o input tem algum valor*/
    if (linha[i] != '\0'){
        /*Obter o número de argumentos do input*/
        num_args = sscanf(&linha[i], "%s %s", arg1, arg2);
        /*Caso o nif não tenha sido omitido (não começa por aspas, não é error e há mais do que uma palavra)
          a primeira palavra lida é obrigatoriamente o NIF*/
        if (!((linha[i] == '"' || (strncmp(&linha[i], "error", 5) == 0 && (isspace(linha[i+5]) || linha[i+5] == '\0'))) || num_args == 1)){
            strcpy(nif_str, arg1);
            i += strlen(nif_str);
        }
    }
    /*Chamar a função auxiliar para extrair o nome*/
    extraiNome(linha, i, nome);
    /*Caso o nome seja omitido e o nif seja default assumir o nome: Cliente final*/
    if (strlen(nome) == 0 && strcmp(nif_str, "999999999") == 0)
        strcpy(nome, "Cliente final");

    /*Testar a validade do nif e do nome*/
    if (testaInputFatura(nif_str, nome) == INVALIDO) return;

    /*"Converter" o NIF para inteiro*/
    nif_num = atoi(nif_str);
    /*Caso o nome seja "error"*/
    if (strcmp(nome, "error") == 0)
        apagaCesto(sys);
    /*Calcular os valores totais*/

    for (i = 0; i < sys->num_cesto; i++){
        if (sys->cesto[i].quantidade > 0){
            total_itens_fatura += sys->cesto[i].quantidade;
            valor_fatura += sys->cesto[i].preco;
        }
    }
    emiteFatura(sys, nif_num, nome, total_itens_fatura, valor_fatura);
}

/**
 * @brief Comando: 'c' -> Lista as faturas já emitidas no sistema.
 * *A função percorre o array dinâmico lista_faturas e imprime os detalhes correspondentes às mesmas.
 * Se for invocada sem argumentos "c", lista todas as faturas emitidas por ordem alfabética (se o nome
 * for igual, o desempate é pela ordem cronológica).
 * Caso tenha um argumento, esse será o <nome> e vai pesquisar todas as faturas com esse nome.
 * A função verifica a validade do nome e se existe nas faturas. Por último imprime as informações a cerca
 * das faturas <número-factura> <valor-facturado> <nome-cliente>.
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém a lista_faturas e variáveis globais
 * @param linha String com a linha de input completa introduzida pelo utilizador*/
void listaFat(Sys *sys, char linha[]){
    char nome[STR_MAX];
    int argumento = NAO_EXISTE;

    /*Utilizar a função auxiliar para extrair o nome*/
    extraiNome(linha, 1, nome);

    /*Verificar se a função tem realmente nome*/
    if (strlen(nome) > 0){
        argumento = EXISTE;
    }

    /*Caso o utilizador tenha inserido o nome como argumento*/
    if (argumento == EXISTE){
        listaFatNome(sys, nome);
    }
    else{
        listaTodasFat(sys);
    }
    return;    
}

/**
 * @brief Comando: 'd' -> Apaga o produto ou fatura do sistema.
 * A função, se ler um ID (1 argumento), localiza a fatura, de seguida reverte os seus valores 
 * nos totais do sistema, liberta a memória dinâmica do nome do cliente e remove-a do array. 
 * Caso seja passado um EAN e uma quantidade (2 argumentos), a função valida os dados e reduz 
 * o stock do produto (garantindo que este não se encontra em uso no cesto). 
 * Se o stock atinjir zero, o produto é permanentemente apagado da tabela principal e os 
 * índices do array auxiliar ordenado são recalculados. 
 * @param sys Ponteiro para a estrutura principal do Sistema, que contém as listas de faturas e produtos
 * @param linha String com a linha de input completa introduzida pelo utilizador*/
void apagaRegisto(Sys *sys, char linha[]){
    int input, quant, id_fatura, arg2;
    char comando, arg1[STR_MAX], ean[STR_MAX];
    input = sscanf(linha, " %c %s %d", &comando, arg1, &arg2);

    if (input == 2){
        /*Remover item das faturas*/
        id_fatura = atoi(arg1);
        removeFatura(sys, id_fatura);
    }
    else if (input == 3){
        strcpy(ean, arg1);
        quant = arg2;
        removeProduto(sys, ean, quant);
    }
}
