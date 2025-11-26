#pragma once
#include "tree.hpp"
#include <string>

namespace cco {

// Exporta os nós em CSV: id, parent, x, y, flow, radius, children_count
void writeNodesCSV(const Tree& t, const std::string& path);

// Exporta as arestas em CSV: src, dst, length
void writeEdgesCSV(const Tree& t, const std::string& path);

} // namespace cco
