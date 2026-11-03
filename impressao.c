#include "impressao.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int selecionar_campos(const char *const campos[], int quantidade,
                             int selecionados[]) {
  char entrada[256];

  printf("\nCampos disponiveis:\n");
  for (int i = 0; i < quantidade; i++)
    printf(" %d - %s\n", i + 1, campos[i]);

  while (1) {
    printf("Digite os numeros dos campos desejados, separados por espaco ou virgula: ");
    if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
      printf("\nEntrada encerrada; relatorio cancelado.\n");
      return 0;
    }

    if (strchr(entrada, '\n') == NULL) {
      int caractere = getchar();
      if (caractere != EOF) {
        while (caractere != '\n' && caractere != EOF)
          caractere = getchar();
        printf("A lista e muito longa. Tente novamente.\n");
        continue;
      }
    }

    int total = 0;
    int invalido = 0;
    char *token = strtok(entrada, " ,\t\r\n");
    while (token != NULL) {
      char *fim;
      errno = 0;
      long numero = strtol(token, &fim, 10);
      if (fim == token || *fim != '\0' || errno == ERANGE || numero < 1 ||
          numero > quantidade) {
        invalido = 1;
        break;
      }

      for (int i = 0; i < total; i++)
        if (selecionados[i] == numero - 1)
          invalido = 1;
      if (invalido)
        break;

      selecionados[total++] = (int)numero - 1;
      token = strtok(NULL, " ,\t\r\n");
    }

    if (!invalido && total > 0)
      return total;

    printf("Selecione ao menos um campo, sem repetir numeros, entre 1 e %d.\n",
           quantidade);
  }
}

static int largura_tabela(const Impressao *impressao) {
  int total = 1;
  for (int i = 0; i < impressao->quantidade_selecionada; i++)
    total += impressao->larguras[i] + 3;
  return total;
}

static void borda_tabela(FILE *arquivo, const Impressao *impressao) {
  for (int i = 0; i < impressao->quantidade_selecionada; i++) {
    fputc('+', arquivo);
    for (int j = 0; j < impressao->larguras[i] + 2; j++)
      fputc('-', arquivo);
  }
  fprintf(arquivo, "+\n");
}

static void linha_tabela(FILE *arquivo, const Impressao *impressao,
                         const char *const valores[], int cabecalho) {
  for (int i = 0; i < impressao->quantidade_selecionada; i++) {
    if (!cabecalho && impressao->numericos[i])
      fprintf(arquivo, "| %*s ", impressao->larguras[i], valores[i]);
    else
      fprintf(arquivo, "| %-*s ", impressao->larguras[i], valores[i]);
  }
  fprintf(arquivo, "|\n");
}

static void imprimir_caixa(FILE *arquivo, const char *const textos[],
                           int quantidade, int largura_minima) {
  int largura = largura_minima;
  for (int i = 0; i < quantidade; i++)
    if ((int)strlen(textos[i]) > largura)
      largura = (int)strlen(textos[i]);

  fputc('+', arquivo);
  for (int i = 0; i < largura + 2; i++)
    fputc('-', arquivo);
  fprintf(arquivo, "+\n");
  for (int i = 0; i < quantidade; i++)
    fprintf(arquivo, "| %-*s |\n", largura, textos[i]);
  fputc('+', arquivo);
  for (int i = 0; i < largura + 2; i++)
    fputc('-', arquivo);
  fprintf(arquivo, "+\n");
}

int iniciar_relatorio(Impressao *impressao, const char *caminho,
                      const char *titulo, const char *const campos[],
                      const int campos_numericos[], int quantidade) {
  memset(impressao, 0, sizeof(*impressao));
  if (quantidade < 1 || quantidade > MAX_CAMPOS) {
    printf("Erro: quantidade invalida de campos no relatorio.\n");
    return 0;
  }

  impressao->quantidade_selecionada =
      selecionar_campos(campos, quantidade, impressao->selecionados);
  if (impressao->quantidade_selecionada == 0)
    return 0;

  impressao->linhas = tmpfile();
  if (impressao->linhas == NULL) {
    printf("Erro ao preparar os dados da tabela.\n");
    return 0;
  }

  impressao->arquivo = fopen(caminho, "w");
  if (impressao->arquivo == NULL) {
    printf("Erro ao criar o arquivo '%s' (a pasta output existe?)\n", caminho);
    fclose(impressao->linhas);
    impressao->linhas = NULL;
    return 0;
  }

  impressao->caminho = caminho;
  impressao->titulo = titulo;
  impressao->campos = campos;
  for (int i = 0; i < impressao->quantidade_selecionada; i++) {
    int indice = impressao->selecionados[i];
    impressao->larguras[i] = (int)strlen(campos[indice]);
    impressao->numericos[i] = campos_numericos[indice];
  }
  return 1;
}

void imprimir_linha_mapeada(Impressao *impressao,
                            const char *const valores[],
                            const int mapa_campos[]) {
  if (impressao->erro)
    return;

  for (int i = 0; i < impressao->quantidade_selecionada; i++) {
    int indice = impressao->selecionados[i];
    const char *valor = valores[mapa_campos != NULL ? mapa_campos[indice]
                                                    : indice];
    int largura = (int)strlen(valor);
    if (largura > impressao->larguras[i])
      impressao->larguras[i] = largura;
    if (fprintf(impressao->linhas, "%s\n", valor) < 0) {
      impressao->erro = 1;
      return;
    }
  }
}

void imprimir_linha(Impressao *impressao, const char *const valores[]) {
  imprimir_linha_mapeada(impressao, valores, NULL);
}

void finalizar_relatorio(Impressao *impressao) {
  FILE *arquivo = impressao->arquivo;
  FILE *linhas = impressao->linhas;
  int erro = impressao->erro;

  if (linhas == NULL || arquivo == NULL)
    return;
  if (fflush(linhas) != 0 || fseek(linhas, 0, SEEK_SET) != 0)
    erro = 1;

  if (!erro) {
    int largura_interna = largura_tabela(impressao) - 4;
    const char *cabecalho[] = {impressao->titulo};
    imprimir_caixa(arquivo, cabecalho, 1, largura_interna);

    borda_tabela(arquivo, impressao);
    const char *nomes[MAX_CAMPOS];
    for (int i = 0; i < impressao->quantidade_selecionada; i++)
      nomes[i] = impressao->campos[impressao->selecionados[i]];
    linha_tabela(arquivo, impressao, nomes, 1);
    borda_tabela(arquivo, impressao);

    char valores[MAX_CAMPOS][1024];
    const char *ponteiros[MAX_CAMPOS];
    for (int i = 0; i < impressao->quantidade_selecionada; i++)
      ponteiros[i] = valores[i];

    int tem_registros = 0;
    while (fgets(valores[0], sizeof(valores[0]), linhas) != NULL) {
      valores[0][strcspn(valores[0], "\n")] = '\0';
      for (int coluna = 1; coluna < impressao->quantidade_selecionada;
           coluna++) {
        if (fgets(valores[coluna], sizeof(valores[coluna]), linhas) == NULL) {
          erro = 1;
          break;
        }
        valores[coluna][strcspn(valores[coluna], "\n")] = '\0';
      }
      if (erro)
        break;
      linha_tabela(arquivo, impressao, ponteiros, 0);
      tem_registros = 1;
    }
    if (ferror(linhas))
      erro = 1;

    borda_tabela(arquivo, impressao);
    if (!tem_registros) {
      const char *vazio[] = {"Nenhum registro encontrado"};
      imprimir_caixa(arquivo, vazio, 1, largura_interna);
    }
    if (ferror(arquivo) || ferror(linhas))
      erro = 1;
  }

  if (fclose(linhas) != 0)
    erro = 1;
  if (fclose(arquivo) != 0)
    erro = 1;
  if (erro)
    printf("Erro ao gravar o arquivo '%s'\n", impressao->caminho);
  else
    printf("Relatorio gerado em '%s'\n", impressao->caminho);
}
