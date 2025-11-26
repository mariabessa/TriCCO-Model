#include "../include/cco/geometry.hpp"
#include <algorithm>
using namespace cco;

// comprimento do vetor
double cco::norm(const Vec2& v){ 
    return std::sqrt(v.x*v.x + v.y*v.y); 
}

// distância entre dois pontos
double cco::dist(const Vec2& a, const Vec2& b){ 
    return norm(Vec2{a.x-b.x, a.y-b.y});
}

// ponto linear entre a e b, com t em [0,1]
Vec2 cco::lerp(const Vec2& a, const Vec2& b, double t){ 
    return Vec2{a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t}; 
}

// distância do ponto p ao segmento ab
double cco::distPointToSegment(const Vec2& p, const Vec2& a, const Vec2& b){
  const double ax=a.x, ay=a.y, bx=b.x, by=b.y; // copias das coordenadas
  const double abx=bx-ax, aby=by-ay; // calcula vetor ab (b - a)
  const double denom = abx*abx + aby*aby; // |ab|^2
  double t = denom>0 ? ((p.x-ax)*abx + (p.y-ay)*aby)/denom : 0.0; // projeta p em ab
  // se denom == 0, a==b, segmento é um ponto, evita divisão por zero e deixa t=0
  // se t <0, proj é antes de a; se t>1, proj é depois de b; se 0<=t<=1, proj é entre a e b
  t = std::max(0.0, std::min(1.0, t)); // restringe t em [0,1], se a proj ficar fora do segmento usamos o ponto mais próximo (a ou b)
  Vec2 proj{ax + t*abx, ay + t*aby}; // ponto projetado em ab
  return dist(p, proj); 
}
