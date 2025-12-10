TriCCO-Model

Implementação básica de um modelo CCO (Constrained Constructive Optimization) em C++ para geração de árvores arteriais bidimensionais, com suporte a validação, exportação VTK/DOT e visualização no ParaView.

1. Estrutura do Projeto
TriCCO-Model/
├── include/
│   └── cco/
│       ├── tree.hpp
│       ├── geometry.hpp
│       ├── metrics.hpp
│       ├── io_vtk.hpp
│       ├── io_dot.hpp
│       ├── io_csv.hpp
│       ├── validate.hpp
├── src/
│   ├── tree.cpp
│   ├── geometry.cpp
│   ├── metrics.cpp
│   ├── io_vtk.cpp
│   ├── io_dot.cpp
│   ├── io_csv.cpp
│   ├── validate.cpp
│   └── main_tp1.cpp
├── CMakeLists.txt
└── Makefile

2. Requisitos

C++17 ou superior

CMake 3.10+

Para visualizar:

ParaView (arquivos .vtk)

graphviz (dot para gerar PNG a partir de .dot)

3. Como compilar

Na raiz do projeto:

make build


Isto cria a pasta build/, executa o CMake e compila o binário tp1-binary.

4. Como executar
make run


O programa irá:

gerar automaticamente uma árvore CCO com 25 folhas

imprimir os nós, fluxos e raios

gerar arquivos:

build/data/cco_tp1_binary.vtk

build/data/arvore.dot

5. Visualizar no ParaView
make paraview


Ou manualmente:

paraview build/data/cco_tp1_binary.vtk

6. Gerar imagem PNG da árvore (DOT → PNG)
make dot


Saída:

build/data/arvore.png

7. Limpeza

Remover apenas a pasta build/:

make clean


Remover tudo (build/ e data/):

make distclean

8. Descrição do Modelo

O programa implementa:

Amostragem de pontos em domínio circular

Adição incremental de terminais

Inserção de bifurcações

Cálculo de fluxo (bottom-up)

Cálculo de raios pela Lei de Murray

Cálculo de custo total

Validação estrutural da árvore (grau máximo, conectividade, fluxo, domínio)

A árvore gerada é binária ou até MAX_CHILDREN (configurável).
O algoritmo é uma versão simplificada do método CCO original.