#include "../include/cco/io_csv.hpp"
#include "../include/cco/geometry.hpp"
#include <fstream>

using namespace cco;

void cco::writeNodesCSV(const Tree& t, const std::string& path){
  const auto& N = t.nodes();

  std::ofstream f(path);
  if(!f.is_open()){
    return;
  }

  f << "id,parent,x,y,flow,radius,children\n";
  for(const auto& n : N){
    f << n.id << ","
      << n.parent << ","
      << n.p.x << ","
      << n.p.y << ","
      << n.flow << ","
      << n.radius << ","
      << n.children.size()
      << "\n";
  }
}

void cco::writeEdgesCSV(const Tree& t, const std::string& path){
  const auto& N = t.nodes();

  std::ofstream f(path);
  if(!f.is_open()){
    return;
  }

  f << "src,dst,length\n";
  for(const auto& n : N){
    if(n.parent < 0) continue;
    double L = dist(n.p, N[n.parent].p);
    f << n.parent << ","
      << n.id << ","
      << L
      << "\n";
  }
}
