#pragma once
#include "tree.hpp"
#include <string>
#include <vector>

namespace cco {

struct ValidationReport {
  bool ok = true;
  std::vector<std::string> errors;

  int nodes = 0;
  int edges = 0;
  int leaves = 0;
  int maxDegree = 0;
  int depth = 0;
  double totalCost = 0.0;
};

// Valida propriedades básicas da árvore:
// - edges = nodes-1
// - conectividade a partir da raiz (nó 1)
// - grau <= MAX_CHILDREN
// - fluxo na raiz ≈ número de folhas
// - nós dentro do domínio circular de raio Rdomain
ValidationReport validateTree(const Tree& T, int MAX_CHILDREN, double Rdomain);

} 
