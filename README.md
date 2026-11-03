# Gerador de relatórios - streaming

Lê 6 arquivos de texto (`input/`), monta listas encadeadas e grava relatórios em TXT (`output/`).
Ao gerar cada relatório, o programa mostra os campos disponíveis numerados. Digite em uma linha
os números dos campos que deseja incluir, separados por espaço ou vírgula (por exemplo: `1 3 5`).

## Compilar e rodar

```
gcc -Wall -Wextra main.c leitura.c interface.c impressao.c relatorio.c -o main
./main          (Linux)
.\main.exe      (Windows)
```

Rode de dentro desta pasta, para o programa achar `input/` e `output/`.

## Módulos

| Arquivo | O que faz |
|---|---|
| `entidades.h` | uma struct por tabela (usuário, assinatura, mídia, filme, série, mídia consumida) |
| `leitura.c` | lê cada arquivo (campos separados por `;` ou tab) e monta a lista; libera as listas |
| `interface.c` | menu de seleção dos relatórios |
| `impressao.c` | solicita os campos e imprime a tabela com bordas e colunas alinhadas |
| `relatorio.c` | define os campos e reúne os dados dos 8 relatórios |
| `main.c` | lê os arquivos, mostra o menu e libera a memória no fim |
