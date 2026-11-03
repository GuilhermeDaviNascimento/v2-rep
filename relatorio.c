#include "relatorio.h"
#include "impressao.h"
#include <stdio.h>

static void formatar_decimal(char *destino, size_t tamanho, double valor,
                             int casas) {
  snprintf(destino, tamanho, "%.*f", casas, valor);
  for (char *p = destino; *p != '\0'; p++)
    if (*p == '.')
      *p = ',';
}

static void formatar_moeda(char *destino, size_t tamanho, double valor) {
  snprintf(destino, tamanho, "R$ %.2f", valor);
  for (char *p = destino; *p != '\0'; p++)
    if (*p == '.')
      *p = ',';
}

static Assinatura *buscar_assinatura(Assinatura *lista, int id) {
  for (; lista != NULL; lista = lista->prox)
    if (lista->id == id)
      return lista;
  return NULL;
}

static Usuario *buscar_usuario(Usuario *lista, int id) {
  for (; lista != NULL; lista = lista->prox)
    if (lista->id == id)
      return lista;
  return NULL;
}

static Midia *buscar_midia(Midia *lista, int id) {
  for (; lista != NULL; lista = lista->prox)
    if (lista->id == id)
      return lista;
  return NULL;
}

static Filme *buscar_filme(Filme *lista, int id_midia) {
  for (; lista != NULL; lista = lista->prox)
    if (lista->id_midia == id_midia)
      return lista;
  return NULL;
}

static Serie *buscar_serie(Serie *lista, int id_midia) {
  for (; lista != NULL; lista = lista->prox)
    if (lista->id_midia == id_midia)
      return lista;
  return NULL;
}

static void imprimir_midia(Impressao *impressao, const Midia *midia,
                           const char *tipo, const char *bilheteria,
                           const char *episodios, const char *temporadas,
                           const int mapa_campos[]) {
  char id[16], duracao[16], avaliacao[16];
  snprintf(id, sizeof(id), "%d", midia->id);
  snprintf(duracao, sizeof(duracao), "%d", midia->duracao);
  formatar_decimal(avaliacao, sizeof(avaliacao), midia->avaliacao_critica, 1);
  const char *valores[] = {
      id,          midia->nome,  midia->categoria, midia->diretor,
      duracao,     midia->data_lancamento, avaliacao, tipo,
      bilheteria,  episodios,    temporadas};
  imprimir_linha_mapeada(impressao, valores, mapa_campos);
}

void gerarRelatorioUsuariosAssinaturas(Usuario *usuarios,
                                      Assinatura *assinaturas,
                                      const char *caminho_saida) {
  const char *campos[] = {"ID",       "Nome",    "Email",      "CPF",
                          "Plano",    "Valor",   "Pagamento",  "Vencimento"};
  const int numericos[] = {1, 0, 0, 0, 0, 1, 0, 0};
  Impressao impressao;
  if (!iniciar_relatorio(&impressao, caminho_saida,
                         "RELATORIO DE USUARIOS E ASSINATURAS", campos,
                         numericos, 8))
    return;

  for (Usuario *u = usuarios; u != NULL; u = u->prox) {
    Assinatura *a = buscar_assinatura(assinaturas, u->id_assinatura);
    char id[16], valor[40];
    snprintf(id, sizeof(id), "%d", u->id);
    if (a != NULL)
      formatar_moeda(valor, sizeof(valor), a->valor);
    else
      snprintf(valor, sizeof(valor), "-");
    const char *valores[] = {
        id, u->nome, u->email, u->cpf, a != NULL ? a->tipo : "Nenhuma",
        valor, a != NULL ? a->metodo_pagamento : "-",
        a != NULL ? a->vencimento : "-"};
    imprimir_linha(&impressao, valores);
  }
  finalizar_relatorio(&impressao);
}

void gerarRelatorioCatalogoMidias(Midia *midias, Filme *filmes,
                                  Serie *series, const char *caminho_saida) {
  const char *campos[] = {"ID",          "Nome",      "Categoria",
                          "Diretor",     "Duracao (min)", "Lancamento",
                          "Avaliacao",   "Tipo",      "Bilheteria",
                          "Episodios",   "Temporadas"};
  const int numericos[] = {1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1};
  Impressao impressao;
  if (!iniciar_relatorio(&impressao, caminho_saida, "CATALOGO DE MIDIAS",
                         campos, numericos, 11))
    return;

  for (Midia *m = midias; m != NULL; m = m->prox) {
    Filme *f = buscar_filme(filmes, m->id);
    Serie *s = buscar_serie(series, m->id);
    char bilheteria[40], episodios[16], temporadas[16];
    snprintf(bilheteria, sizeof(bilheteria), "-");
    snprintf(episodios, sizeof(episodios), "-");
    snprintf(temporadas, sizeof(temporadas), "-");
    if (f != NULL)
      formatar_moeda(bilheteria, sizeof(bilheteria), f->bilheteria);
    if (s != NULL) {
      snprintf(episodios, sizeof(episodios), "%d", s->numero_episodios);
      snprintf(temporadas, sizeof(temporadas), "%d", s->numero_temporadas);
    }
    imprimir_midia(&impressao, m, m->tipo == 'S' ? "Serie" : "Filme",
                   bilheteria, episodios, temporadas, NULL);
  }
  finalizar_relatorio(&impressao);
}

void gerarRelatorioCatalogoSeries(Midia *midias, Serie *series,
                                  const char *caminho_saida) {
  const char *campos[] = {"ID",          "Nome",       "Categoria",
                          "Diretor",     "Duracao (min)", "Lancamento",
                          "Avaliacao",   "Episodios",  "Temporadas"};
  const int numericos[] = {1, 0, 0, 0, 1, 0, 1, 1, 1};
  Impressao impressao;
  if (!iniciar_relatorio(&impressao, caminho_saida, "CATALOGO DE SERIES",
                         campos, numericos, 9))
    return;

  const int mapa_campos[] = {0, 1, 2, 3, 4, 5, 6, 9, 10};
  for (Midia *m = midias; m != NULL; m = m->prox) {
    Serie *s = buscar_serie(series, m->id);
    if (s == NULL)
      continue;

    char episodios[16], temporadas[16];
    snprintf(episodios, sizeof(episodios), "%d", s->numero_episodios);
    snprintf(temporadas, sizeof(temporadas), "%d", s->numero_temporadas);
    imprimir_midia(&impressao, m, "Serie", "-", episodios, temporadas,
                   mapa_campos);
  }
  finalizar_relatorio(&impressao);
}

void gerarRelatorioCatalogoFilmes(Midia *midias, Filme *filmes,
                                  const char *caminho_saida) {
  const char *campos[] = {"ID",          "Nome",      "Categoria",
                          "Diretor",     "Duracao (min)", "Lancamento",
                          "Avaliacao",   "Bilheteria"};
  const int numericos[] = {1, 0, 0, 0, 1, 0, 1, 1};
  Impressao impressao;
  if (!iniciar_relatorio(&impressao, caminho_saida, "CATALOGO DE FILMES",
                         campos, numericos, 8))
    return;

  const int mapa_campos[] = {0, 1, 2, 3, 4, 5, 6, 8};
  for (Midia *m = midias; m != NULL; m = m->prox) {
    Filme *f = buscar_filme(filmes, m->id);
    if (f == NULL)
      continue;

    char bilheteria[40];
    formatar_moeda(bilheteria, sizeof(bilheteria), f->bilheteria);
    imprimir_midia(&impressao, m, "Filme", bilheteria, "-", "-", mapa_campos);
  }
  finalizar_relatorio(&impressao);
}

void gerarRelatorioConsumo(Usuario *usuarios, Midia *midias,
                           MidiaConsumida *consumidas, char tipo,
                           const char *caminho_saida) {
  const char *campos[] = {"Usuario", "Midia", "Favorito", "Avaliacao"};
  const int numericos[] = {0, 0, 0, 1};
  const char *titulo = "CONSUMO DE MIDIAS POR USUARIO";
  if (tipo == 'F')
    titulo = "CONSUMO DE FILMES POR USUARIO";
  else if (tipo == 'S')
    titulo = "CONSUMO DE SERIES POR USUARIO";

  Impressao impressao;
  if (!iniciar_relatorio(&impressao, caminho_saida, titulo, campos, numericos,
                         4))
    return;

  for (MidiaConsumida *c = consumidas; c != NULL; c = c->prox) {
    Usuario *u = buscar_usuario(usuarios, c->id_usuario);
    Midia *m = buscar_midia(midias, c->id_midia);
    if (tipo != 0 && (m == NULL || m->tipo != tipo))
      continue;

    char avaliacao[16];
    formatar_decimal(avaliacao, sizeof(avaliacao), c->avaliacao_usuario, 1);
    const char *valores[] = {
        u != NULL ? u->nome : "Desconhecido",
        m != NULL ? m->nome : "Desconhecida",
        c->favorito ? "Sim" : "Nao",
        avaliacao};
    imprimir_linha(&impressao, valores);
  }
  finalizar_relatorio(&impressao);
}

void gerarRelatorioFaturamentoPagamento(Usuario *usuarios,
                                        Assinatura *assinaturas,
                                        const char *caminho_saida) {
  const char *campos[] = {"Metodo", "Usuario", "Plano", "Valor"};
  const int numericos[] = {0, 0, 0, 1};
  Impressao impressao;
  if (!iniciar_relatorio(&impressao, caminho_saida,
                         "FATURAMENTO POR METODO DE PAGAMENTO", campos,
                         numericos, 4))
    return;

  for (Usuario *u = usuarios; u != NULL; u = u->prox) {
    Assinatura *a = buscar_assinatura(assinaturas, u->id_assinatura);
    if (a == NULL)
      continue;

    char valor[40];
    formatar_moeda(valor, sizeof(valor), a->valor);
    const char *valores[] = {a->metodo_pagamento, u->nome, a->tipo, valor};
    imprimir_linha(&impressao, valores);
  }
  finalizar_relatorio(&impressao);
}
