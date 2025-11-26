#pragma once
#include "tree.hpp"
#include <string>

namespace cco {

// Exporta a árvore em formato Graphviz DOT.
// Dá pra visualizar com: dot -Tpng cco.dot -o cco.png
void writeDOT(const Tree& t, const std::string& path);

} // namespace cco
