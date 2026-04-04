#ifndef COMANDOS_H
#define COMANDOS_H

#include "estruturas.h"
#include "auxiliares.h"

void addProduto(Sys *sys, char linha[]);
void listaProduto(Sys *sys, char linha[]);
void addCesto(Sys *sys, char linha[]);
void resumoFat(Sys *sys, char linha[]);
void fatura(Sys *sys, char linha[]);
void listaFat(Sys *sys, char linha[]);
void apagaRegisto(Sys *sys, char linha[]);

#endif