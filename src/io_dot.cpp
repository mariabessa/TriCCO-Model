#include "../include/cco/io_dot.hpp"
#include <fstream>
#include <iomanip>

using namespace cco;

void cco::writeDOT(const Tree& t, const std::string& path){
  const auto& N = t.nodes();

  std::ofstream f(path);
  if(!f.is_open()){
    return; 
  }

  f << "digraph cco {\n";
  f << "  node [shape=circle, fontsize=10];\n";

  // declara nós com alguns atributos na label
  for(const auto& n : N){
    f << "  " << n.id << " [label=\""
      << "id=" << n.id
      << "\\nQ=" << std::setprecision(3) << n.flow
      << "\\nr=" << std::setprecision(3) << n.radius
      << "\"];\n";
  }

  // arestas parent -> child
  for(const auto& n : N){
    if(n.parent < 0) continue;
    f << "  " << n.parent << " -> " << n.id << ";\n";
  }

  f << "}\n";
}
