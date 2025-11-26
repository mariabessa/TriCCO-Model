#include "../include/cco/validate.hpp"
#include "../include/cco/metrics.hpp"
#include "../include/cco/geometry.hpp"
#include <queue>
#include <cmath>

using namespace cco;

static bool insideCircle(const Vec2& p, double R) {
  return (p.x*p.x + p.y*p.y) <= R*R + 1e-9;
}

ValidationReport cco::validateTree(const Tree& T, int MAX_CHILDREN, double Rdomain) {
  ValidationReport rep;
  const auto& N = T.nodes();
  rep.nodes = (int)N.size();

  if (rep.nodes == 0) {
    rep.ok = false;
    rep.errors.push_back("Tree has no nodes.");
    return rep;
  }

  // 1) edges = nodes - 1 (árvore)
  int edges = 0;
  for (auto& n : N) if (n.parent >= 0) ++edges;
  rep.edges = edges;
  if (edges != rep.nodes - 1) {
    rep.ok = false;
    rep.errors.push_back("Edges != nodes-1 (structure may not be a tree).");
  }

  // 2) conectividade a partir da raiz física (nó 1)
  if ((int)N.size() > 1) {
    std::vector<char> vis(N.size(), 0);
    std::queue<int> q;
    q.push(1);
    vis[1] = 1;
    int seen = 0;
    while (!q.empty()) {
      int u = q.front(); q.pop();
      ++seen;
      for (auto v : N[u].children) {
        if (!vis[v]) {
          vis[v] = 1;
          q.push(v);
        }
      }
    }
    // ignora nó 0 (topo geométrico), então espera ver N.size()-1
    if (seen != (int)N.size()-1) {
      rep.ok = false;
      rep.errors.push_back("Not all nodes reachable from root (1).");
    }
  }

  // 3) grau e contagem de folhas
  int maxDeg = 0;
  int leaves = 0;
  for (auto& n : N) {
    int deg = (int)n.children.size();
    maxDeg = std::max(maxDeg, deg);
    if (deg == 0) ++leaves;
    if (deg > MAX_CHILDREN) {
      rep.ok = false;
      rep.errors.push_back("Node " + std::to_string(n.id) +
                           " exceeds MAX_CHILDREN.");
    }
  }
  rep.maxDegree = maxDeg;
  rep.leaves    = leaves;

  // 4) fluxo na raiz ≈ nº de folhas
  double flowAtRoot = (N.size() > 1) ? N[1].flow : 0.0;
  if (std::fabs(flowAtRoot - (double)leaves) > 1e-6) {
    rep.ok = false;
    rep.errors.push_back("Flow at root != number of leaves.");
  }

  // 5) nós dentro do domínio circular
  for (auto& n : N) {
    if (!insideCircle(n.p, Rdomain)) {
      rep.ok = false;
      rep.errors.push_back("Node " + std::to_string(n.id) +
                           " lies outside domain radius.");
      break;
    }
  }

  // 6) custo total e profundidade
  rep.totalCost = T.totalCost();
  rep.depth     = maxDepthApprox(T);

  return rep;
}
