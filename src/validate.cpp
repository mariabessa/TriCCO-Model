#include "../include/cco/validate.hpp" 
#include "../include/cco/metrics.hpp" 
#include "../include/cco/geometry.hpp" 
#include <queue> 
#include <cmath>  

using namespace cco; 

// verifica se o ponto p está dentro ou sobreposto ao círculo de raio R
static bool insideCircle(const Vec2& p, double R) {
  // calcula p.x^2 + p.y^2 <= R^2 com folga de 1e-9
  return (p.x*p.x + p.y*p.y) <= R*R + 1e-9;
}

// rotina de validação da árvore `T` com limites em filhos por nó e domínio
ValidationReport cco::validateTree(const Tree& T, int MAX_CHILDREN, double Rdomain) {
  ValidationReport rep;            // relatório de validação 
  const auto& N = T.nodes();     
  rep.nodes = (int)N.size();       // num total de nós

  // 0) verificação mínima: árvore não vazia
  if (rep.nodes == 0) {
    rep.ok = false;
    rep.errors.push_back("Tree has no nodes.");
    return rep; 
  }

  // 1) verifica propriedade de árvore: edges == nodes - 1
  int edges = 0;
  for (auto& n : N) 
    if (n.parent >= 0) 
        ++edges; // conta arestas 
  rep.edges = edges;
    // como o índice 0 é reservado (topo geométrico), espera-se ver rep.nodes-1 nós
  if (edges != rep.nodes - 1) {
    rep.ok = false;
    rep.errors.push_back("Edges != nodes-1 (structure may not be a tree).");
  }

  // 2) verifica conectividade a partir da raiz física (nó 1)
  if ((int)N.size() > 1) {
    std::vector<char> vis(N.size(), 0); // vetor de visitados 
    std::queue<int> q;                  // fila para BFS
    q.push(0);                           // inicia a BFS a partir do nó 1 (raiz física)
    vis[1] = 1;                          // marca raiz como visitada
    int seen = 0;                        // contador de nós alcançados
    while (!q.empty()) {
      int u = q.front(); q.pop();
      ++seen;                            // visita u
      for (auto v : N[u].children) {     // percorre filhos de u
        if (!vis[v]) {                   // se ainda não visitado
          vis[v] = 1;                    // marca e enfileira
          q.push(v);
        }
      }
    }
    // como o índice 0 é reservado (topo geométrico), espera-se ver N.size()-1 nós
    if (seen != (int)N.size()-1) {
      rep.ok = false;
      rep.errors.push_back("Not all nodes reachable from root (1).");
    }
  }

  // 3) grau máximo e contagem de folhas; também checa limite MAX_CHILDREN
  int maxDeg = 0;
  int leaves = 0;
  for (auto& n : N) {
    int deg = (int)n.children.size();         // grau = número de filhos
    maxDeg = std::max(maxDeg, deg);          // atualiza grau máximo
    if (deg == 0) ++leaves;                   // conta folhas (sem filhos)
    if (deg > MAX_CHILDREN) {                 // valida limite de filhos
      rep.ok = false;
      rep.errors.push_back("Node " + std::to_string(n.id) + " exceeds MAX_CHILDREN.");
    }
  }
  rep.maxDegree = maxDeg;                     // guarda grau máximo no relatório
  rep.leaves    = leaves;                     // guarda número de folhas

  // 4) verifica fluxo na raiz aproximadamente igual ao número de folhas
  double flowAtRoot = (N.size() > 0) ? N[0].flow : 0.0; // pega fluxo em 1 se existir
  if (std::fabs(flowAtRoot - (double)leaves) > 1e-6) {
    rep.ok = false;
    rep.errors.push_back("Flow at root != number of leaves.");
  }

  // 5) verifica se todos os nós estão dentro do domínio circular de raio Rdomain
  for (auto& n : N) {
    if (!insideCircle(n.p, Rdomain)) { // usa a função helper com tolerance
      rep.ok = false;
      rep.errors.push_back("Node " + std::to_string(n.id) + " lies outside domain radius.");
      break; // basta um nó fora para falhar, interrompe checagem
    }
  }

  // 6) preenche custo total e profundidade aproximada
  rep.totalCost = T.totalCost();   // obtém custo total calculado pela árvore
  rep.depth     = maxDepthApprox(T); // usa função de métricas para profundidade

  return rep; // retorna relatório (rep.ok será false se qualquer checagem falhou)
}
