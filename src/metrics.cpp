#include "../include/cco/metrics.hpp"
#include <functional>

using namespace cco; 

// conta o número de folhas (nós sem filhos) na árvore `t`
int cco::countLeaves(const Tree& t){
  int k = 0; // contador
  for(auto& n: t.nodes()) // itera sobre todos os nós da árvore
    if(n.children.empty()) ++k; // se vetor de filhos está vazio, é folha => incrementa
  return k;
}

// calcula uma aproximação da profundidade max da árvore
int cco::maxDepthApprox(const Tree& t){
  const auto& N = t.nodes(); // referência aos nós para acesso por índice

  // lambda recursivo que recebe índice de nó u e retorna a profundidade max
  std::function<int(int)> depth = [&](int u) -> int {
    if(N[u].children.empty()) return 0; // no folha tem profundidade 0 (base da recursão)
    int md = 0; // armazena a max profundidade entre os filhos
    for(auto v: N[u].children) // percorre cada filho v de  u
      md = std::max(md, 1 + depth(v)); // calcula 1 + profundidade do filho e atualiza máximo
    return md; // retorna a maior profundidade encontrada para o nó u
  };

  return depth(0); // raiz é nó 1
}
