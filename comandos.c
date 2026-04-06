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
    /*Introduz um novo produto no sistema (key = p)
      p <ean> <iva> <preco> <quantidade> <descricao>  */

    /*Inicializar variáveis*/
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
    /*Imprime a info de todos os produtos (key = l)*/
    char *token, delimitadores[] = " \n\t";
    int i, verifica = 0, indice;

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

        /*Caso não haja wildcards*/
        if (strchr(token, '*') == NULL && strchr(token, '?') == NULL){
            /*Procurar o indice exato do ean*/
            indice = procuraProduto(sys, token);

            /*Imprimir o produto caso exista e a quantidade não seja 0*/
            if (indice != NAO_EXISTE && sys->tabela_produtos[indice].quantidade > 0){
                imprimeProduto(indice, sys->tabela_produtos);
                verifica = EXISTE;
            }
        }
        /*Caso haja wildcards*/
        else {
            for (i = 0; i < sys->num_produtos; i++){
                if (sys->tabela_produtos[i].quantidade > 0 && testaToken(token, sys->tabela_produtos[i].ean)){
                    imprimeProduto(i, sys->tabela_produtos);
                    verifica = EXISTE;
                }
            }
        }

        if (verifica == 0)
            printf("%s"NO_PROD_ERR"\n", token);
        
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
    /*Adiciona ao cesto a quantidade de produto indicado (key = a)*/
    int quant = 0, input = 0, indice_tabela = 0, i;
    int indice_cesto = NAO_EXISTE;
    char comando, ean[STR_MAX], arg1[STR_MAX], arg2[STR_MAX]; 
    double preco_com_iva;
    
    input = sscanf(linha, " %c %s %s", &comando, arg1, arg2);

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

    if (validaCesto(sys, ean, quant, &indice_tabela, &indice_cesto) == INVALIDO){
        return;
    };
    
    /*-------------------------------------------*/
    /*Atualizar stock na tabela_produtos (remover quant) e a quantidade vendida*/
    sys->tabela_produtos[indice_tabela].quantidade -= quant;
    sys->tabela_produtos[indice_tabela].vendido += quant;

    /*Caso o item já esteja no cesto*/
    if (indice_cesto != NAO_EXISTE){
        atualizaCesto(sys, indice_tabela, indice_cesto, quant); /*Atualizar a quantidade*/
    }
    /*Caso o item não exista no cesto*/
    else if (indice_cesto == NAO_EXISTE){
        insereCesto(sys, indice_tabela, ean, quant); /*Inserir no cesto*/
    }
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
    /*Resumo da Faturação (key = r)*/
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
        if (testaEAN(ean) == INVALIDO){
            printf(EAN_ERR"\n");
            return;
        }
        /*Obter o indice do produto*/
        indice = procuraProduto(sys, ean);

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
    /*Fatura os produtos no cesto (key = f)*/
    char nif_str[STR_MAX] = "999999999"; 
    char nome[STR_MAX], arg1[STR_MAX], arg2[STR_MAX];
    int i = 1; /* Começa no indíce 1 para ignorar o f*/
    int j, total_itens_fatura = 0, nif_num, num_args;
    double valor_fatura = 0.0;

    /*Encontrar os inputs corretos*/

    while (isspace(linha[i])){
        i++; /*Incrementar o i até encontrar algo que não seja espaço*/
    }
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

    if (strlen(nome) == 0 && strcmp(nif_str, "999999999") == 0){
        strcpy(nome, "Cliente final");
    }

    /*TESTES*/
    if (testanif(nif_str) == INVALIDO){
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
            int id_prod = procuraProduto(sys, sys->cesto[j].ean);
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
    sys->lista_faturas[sys->num_faturas].id_fatura = sys->num_faturas_emit + 1; /*Atualizar o id da fatura*/

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
    /*Lista as faturas do sistema (key = c)*/
    char nome[STR_MAX];
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
        if (sys->num_faturas == 0){
            return;
        }
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
    /*Apaga o registo de um produto ou fatura (key = d)*/
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
