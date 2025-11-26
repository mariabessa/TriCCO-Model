#include "../include/cco/tree.hpp"
#include <algorithm> 
#include <cassert>   

using namespace cco; 


// nó 0 = raiz geométrica (fixo no topo do domínio)
// nó 1 = raiz física (primeira bifurcação real da árvore)


// construtor da árvore: inicializa domínio, RNG e cria dois nós iniciais
Tree::Tree(double R, double gamma)
: R_(R), gamma_(gamma), rng_(cco::RNG(cco::RNG_SEED)) {
  // configura domínio com raio e ponteiro pro gerador de números
  dom_.R = R_;
  dom_.rng = &rng_;

  // cria nó "topo" (id=0) posicionado no círculo superior
  Node rootTop; rootTop.id = 0; rootTop.parent = -1; rootTop.p = Vec2{0.0, R_};
  nodes_.push_back(rootTop); // insere topo como primeiro nó

  // cria nó central (id=1) como filho do topo
  Node center; center.id = 1; center.parent = 0; center.p = Vec2{0.0, 0.0};
  nodes_[0].children.push_back(1); 
  nodes_.push_back(center);

  // atualiza fluxos e raios iniciais após construir nós base
  recomputeFlowAndRadius();
}

// adiciona um novo nó em posição p com parent dado, retornando seu id
NodeId Tree::addNode(const Vec2& p, NodeId parent){
  NodeId nid = (NodeId)nodes_.size(); // novo id é o tamanho atual do vetor
  Node nd; nd.id = nid; nd.parent = parent; nd.p = p; 
  nodes_.push_back(nd); 
  if(parent >= 0) nodes_[parent].children.push_back(nid); 
  return nid; 
}

// encontra o segmento (representado pelo nó filho) mais próximo do ponto `q`
NodeId Tree::nearestSegment(const Vec2& q) const {
    NodeId best = -1; // melhor candidato (id do nó filho)
    double bd = std::numeric_limits<double>::infinity(); // melhor distância encontrada
    for(const auto& n: nodes_){
    // ignora a aresta especial topo<->centro (id 0 e 1) se presente
    if ((n.id == 1 && n.parent == 0) || (n.id == 0 && n.parent == 1))
        continue;
    if(n.parent < 0) continue; 
    const Vec2 a = n.p; // extremidade do segmento (nó filho)
    const Vec2 b = nodes_[n.parent].p; // outra extremidade (pai)
    double d = distPointToSegment(q, a, b); // distância de q ao segmento ab
    if(d < bd){ bd = d; best = n.id; } // atualiza melhor candidato
  }
  return best < 0 ? 1 : best;  // se nenhum achado, retorna 1 (centro) como fallback
}

// distância mínima do ponto q a qualquer segmento da árvore
double Tree::minDistToAnySegment(const Vec2& q) const {
  double md = std::numeric_limits<double>::infinity(); // mínimo global
  for(const auto& n: nodes_){
    if(n.parent < 0) continue; // pula nós sem pai
    double d = distPointToSegment(q, n.p, nodes_[n.parent].p); // dist ao segmento
    md = std::min(md, d); // atualiza mínimo
  }
  return md; // retorna menor distância encontrada
}

// adiciona um terminal amostrado no domínio, respeitando distância mínima a segmentos
NodeId Tree::addTerminal(){
  Vec2 q{}; // ponto candidato
  for(int tries = 0; tries < 5000; ++tries){
    q = dom_.samplePoint(); // amostra ponto no domínio
    if(minDistToAnySegment(q) >= cco::MIN_DIST_PT) break; // aceita se longe o suficiente
  }

  // escolhe segmento mais próximo e prepara bifurcação no meio
  NodeId child = nearestSegment(q);
  Vec2  a = nodes_[child].p; // ponto filho do segmento
  Vec2  b = nodes_[nodes_[child].parent].p; // ponto pai do segmento

  Vec2 bif = lerp(a, b, 0.5); // ponto médio -> local da bifurcação
  NodeId parentOld = nodes_[child].parent; // pai original do `child`

  NodeId bifId = addNode(bif, parentOld); // cria nó de bifurcação como filho do pai antigo
  {
    auto& vec = nodes_[parentOld].children; // referência ao vetor de filhos do pai antigo
    auto it = std::find(vec.begin(), vec.end(), child);
    if (it != vec.end()) {
      vec.erase(it);
    }
  }
  nodes_[child].parent = bifId; // atualiza parent do child para a bifurcação
  nodes_[bifId].children.push_back(child); // registra child como filho da bifurcação

  // decide onde anexar o terminal: na bifurcação (se espaço) ou no primeiro filho existente
  NodeId term = -1;
  if((int)nodes_[bifId].children.size() < MAX_CHILDREN)
    term = addNode(q, bifId); // adiciona terminal como filho da bifurcação
  else {
    NodeId tgt = nodes_[bifId].children.front(); // direciona para o primeiro filho
    term = addNode(q, tgt); // adiciona terminal como filho desse alvo
  }

  // atualiza fluxos e raios após modificação topológica
  recomputeFlowAndRadius();
  return term; // retorna id do terminal criado
}

// DFS recursivo para propagar fluxos bottom-up; outFlow acumula fluxo total da subárvore
void Tree::dfsFlow(NodeId u, double& outFlow){
  if(nodes_[u].children.empty()){
    nodes_[u].flow = 1.0; // folha tem fluxo 1
    outFlow += 1.0; // incrementa acumulador
    return; // retorna ao chamador
  }
  double s = 0.0; // soma de fluxos dos filhos
  for(auto v: nodes_[u].children){
    double acc = 0.0; dfsFlow(v, acc); // recursão para filho v
    s += acc; // acumula fluxo retornado
  }
  nodes_[u].flow = s; // define fluxo do nó como soma dos filhos
  outFlow += s; // propaga soma para o nível acima
}

// recalcula os fluxos em toda árvore e atualiza o raio visual/operacional de cada nó
void Tree::recomputeFlowAndRadius(){
  // 1) zera fluxos de todos os nós (garantia)
  for (auto& n : nodes_) {
    n.flow = 0.0;
  }

  // 2) encontra a raiz "verdadeira" (nó com parent < 0). por segurança, default=0
  NodeId root = 0;
  for (const auto& n : nodes_) {
    if (n.parent < 0) {
      root = n.id;
      break;
    }
  }

  // 3) propaga fluxo bottom-up a partir da raiz real
  double acc = 0.0;
  dfsFlow(root, acc);

  // calcula constantes para normalização do raio visual
  const double Qroot = std::max(nodes_[root].flow, 1e-9); // evita divisão por zero
  const double rroot = cco::RROOT_VISUAL; // raio visual de referência
  const double k = rroot / std::pow(Qroot, 1.0/gamma_); // fator escala

  // aplica fórmula de raio a todos os nós (evita n.flow muito pequeno com max)
  for (auto& n : nodes_) {
    n.radius = k * std::pow(std::max(n.flow, 1e-9), 1.0/gamma_);
  }
}


// custo total da árvore: soma sobre arestas de (r^4 * L)
double Tree::totalCost() const {
  double c = 0.0;
  for(const auto& n: nodes_){
    if(n.parent < 0) continue; // pula nós sem pai
    const double L = dist(n.p, nodes_[n.parent].p); // comprimento da aresta
    c += std::pow(n.radius, 4.0) * L; // soma custo local
  }
  return c; // retorna custo total
}

// conta folhas (nós sem filhos)
int Tree::leafCount() const {
  int k = 0; for(auto& n: nodes_) if(n.children.empty()) ++k; return k;
}
