#ifndef RELATORIO_H
#define RELATORIO_H

#include "entidades.h"

void gerarRelatorioUsuariosAssinaturas(Usuario *usuarios, Assinatura *assinaturas, const char *caminho_saida);

void gerarRelatorioCatalogoMidias(Midia *midias, Filme *filmes, Serie *series, const char *caminho_saida);

void gerarRelatorioCatalogoSeries(Midia *midias, Serie *series, const char *caminho_saida);

void gerarRelatorioCatalogoFilmes(Midia *midias, Filme *filmes, const char *caminho_saida);

void gerarRelatorioConsumo(Usuario *usuarios, Midia *midias, MidiaConsumida *consumidas, char tipo, const char *caminho_saida);

void gerarRelatorioFaturamentoPagamento(Usuario *usuarios, Assinatura *assinaturas, const char *caminho_saida);

#endif
