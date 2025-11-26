#include "../include/cco/io_vtk.hpp"
#include <fstream> 
#include <vector> 
using namespace cco; 

// escreve uma representação VTK da árvore `t` em `path`
void cco::writeVTK(const Tree& t, const std::string& path){
  const auto& nodes = t.nodes(); // referencia constante ao vetor de nós da árvore

  // monta lista de arestas (parent -> id)
  std::vector<std::pair<int,int>> lines; // cada par representa (parent, id)
  lines.reserve(nodes.size()); // reserva capacidade para evitar realocações
  for(const auto& n: nodes){ // itera sobre cada nó
    if(n.parent >= 0){ // se nó tem parent válido (root tem parent < 0)
      lines.emplace_back(n.parent, n.id); // adiciona aresta (parent -> id)
    }
  }

  std::ofstream f(path); // abre arquivo de saída em `path`
  if(!f.is_open()){ 
    return;
  }

  // cabeçalho VTK básico
  f << "# vtk DataFile Version 3.0\n"; 
  f << "simple_cco\n";                 
  f << "ASCII\n";                      
  f << "DATASET POLYDATA\n";           

  // pontos (nós da árvore)
  f << "POINTS " << nodes.size() << " float\n";
  for(const auto& n: nodes){ // para cada nó, escreve coordenadas x,y,z
    f << n.p.x << " " << n.p.y << " 0.0\n"; // z = 0 (modelo 2D), escreve um ponto por linha
  }

  // linhas (segmentos) — cada linha é armazenada como: <n_points> <idx0> <idx1>
  f << "LINES " << lines.size() << " " << lines.size() * 3 << "\n"; // count e tamanho total (cada linha ocupa 3 valores: 2 + 1)
  for(const auto& e: lines){ // para cada aresta, escreve "2 idx_parent idx_child"
    f << "2 " << e.first << " " << e.second << "\n";
  }

  // atributo escalar por nó: radius (para colorir por raio no VTK)
  f << "POINT_DATA " << nodes.size() << "\n"; 
  f << "SCALARS radius float 1\n";              // nome do atributo, tipo e componentes
  f << "LOOKUP_TABLE default\n";               
  for(const auto& n: nodes){ // escreve um valor de raio por nó
    f << n.radius << "\n"; 
  }
}
