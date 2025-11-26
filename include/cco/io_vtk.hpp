#pragma once
#include "tree.hpp"
#include <string>

namespace cco {

// Exporta a árvore em formato VTK (POLYDATA) para visualizar no ParaView.
void writeVTK(const Tree& t, const std::string& path);

} // namespace cco
