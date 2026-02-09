#pragma once
#include <cmath>

namespace cco {

// um ponto no plano 2D
struct Vec2 { double x, y; };

// comprimento do vetor
double norm(const Vec2& v);

// distância entre dois pontos
double dist(const Vec2& a, const Vec2& b);

// ponto linear entre a e b, com t em [0,1]
Vec2 lerp(const Vec2& a, const Vec2& b, double t);

// distância do ponto p ao segmento ab
double distPointToSegment(const Vec2& p, const Vec2& a, const Vec2& b);

// fator de projeção t para o ponto p no segmento ab
double getProjectionT(const Vec2& p, const Vec2& a, const Vec2& b);

}
