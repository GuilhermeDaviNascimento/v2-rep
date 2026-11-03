#include "interface.h"
#include <stdio.h>
#include <string.h>

static int ler_opcao(void) {
  char linha[32];
  int opcao;

  while (1) {
    printf("Escolha uma opcao [9]: ");
    if (fgets(linha, sizeof(linha), stdin) == NULL)
      return 9;
    if (strchr(linha, '\n') == NULL) {
      int c;
      while ((c = getchar()) != '\n' && c != EOF)
        ;
    }
    if (sscanf(linha, "%d", &opcao) == 1 && opcao >= 1 && opcao <= 9)
      return opcao;
    printf("Digite um numero de 1 a 9.\n");
  }
}

int menu_principal(void) {
  printf("\n==============================================\n");
  printf("   GERADOR DE RELATORIOS - STREAMING\n");
  printf("==============================================\n");
  printf(" 1 - Relatorio de Usuarios e Assinaturas\n");
  printf(" 2 - Catalogo de Midias\n");
  printf(" 3 - Catalogo de Series\n");
  printf(" 4 - Catalogo de Filmes\n");
  printf(" 5 - Consumo de Midias por Usuario\n");
  printf(" 6 - Consumo de Series por Usuario\n");
  printf(" 7 - Consumo de Filmes por Usuario\n");
  printf(" 8 - Faturamento por Metodo de Pagamento\n");
  printf(" 9 - Sair\n");
  printf("==============================================\n");
  return ler_opcao();
}
