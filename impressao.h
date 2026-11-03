#ifndef IMPRESSAO_H
#define IMPRESSAO_H

#include <stdio.h>

#define MAX_CAMPOS 12

typedef struct {
  FILE *arquivo;
  FILE *linhas;
  const char *caminho;
  const char *titulo;
  const char *const *campos;
  int selecionados[MAX_CAMPOS];
  int larguras[MAX_CAMPOS];
  int numericos[MAX_CAMPOS];
  int quantidade_selecionada;
  int erro;
} Impressao;

int iniciar_relatorio(Impressao *impressao, const char *caminho,
                      const char *titulo, const char *const campos[],
                      const int campos_numericos[], int quantidade);
void imprimir_linha(Impressao *impressao, const char *const valores[]);
void imprimir_linha_mapeada(Impressao *impressao,
                            const char *const valores[],
                            const int mapa_campos[]);
void finalizar_relatorio(Impressao *impressao);

#endif
