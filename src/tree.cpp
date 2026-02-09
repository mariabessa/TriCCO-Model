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

  // mudanca da logica de escolha de segmentou para seguir cco classico:
  //  Otimização Global
  // Para cada novo terminal q:
  // 1. Testa-se todos os segmentos candidatos (Bifurcação)
  // 2. Testa-se todos os nós candidatos com grau < MAX_CHILDREN (Trifurcação)
  // 3. Escolhe a opção que minimiza o custo global

  NodeId bestSegmentNode = -1; 
  double bestT = 0.5;
  NodeId bestAttachNode = -1; // para trifurcação
  double minGlobalCost = std::numeric_limits<double>::infinity();

  // Snapshot do estado atual para permitir simulações
  std::vector<Node> originalNodes = nodes_;

  // LOOP 1: TESTE DE BIFURCAÇÃO (Quebrar um segmento) 
  for(size_t i = 0; i < originalNodes.size(); ++i){
      const auto& n = originalNodes[i];
      if(n.parent < 0) continue; // Raiz sem pai
      
      // Candidato: segmento (n, parent)
      const auto& pNode = originalNodes[n.parent];
      double t = cco::getProjectionT(q, n.p, pNode.p);

      // Restaura estado
      nodes_ = originalNodes; 
      
      // Simula inserção (Bifurcação)
      NodeId child = (NodeId)i; 
      Vec2 a = nodes_[child].p;
      Vec2 b = nodes_[nodes_[child].parent].p;
      Vec2 bif = lerp(a, b, t);
      
      NodeId parentOld = nodes_[child].parent;
      NodeId bifId = addNode(bif, parentOld); 
      
      {
        auto& vec = nodes_[parentOld].children;
        auto it = std::find(vec.begin(), vec.end(), child);
        if (it != vec.end()) vec.erase(it);
      }
      nodes_[child].parent = bifId;
      nodes_[bifId].children.push_back(child);

      if((int)nodes_[bifId].children.size() < MAX_CHILDREN){
         addNode(q, bifId);
      } else {
         NodeId tgt = nodes_[bifId].children.front();
         addNode(q, tgt);
      }

      recomputeFlowAndRadius();
      double currentCost = totalCost();

      if(currentCost < minGlobalCost){
          minGlobalCost = currentCost;
          bestSegmentNode = child;
          bestT = t;
          bestAttachNode = -1; // Invalida trifurcação pois achou melhor bifurcação
      }
  }

  // LOOP 2: TESTE DE TRIFURCAÇÃO (Conectar a nó existente) 
  for(size_t i = 0; i < originalNodes.size(); ++i){
      const auto& n = originalNodes[i];
      // Verifica se pode receber mais filhos (MAX_CHILDREN controla se pode ter 3, 4, etc)
      if(n.children.size() < MAX_CHILDREN) {
          // Restaura estado
          nodes_ = originalNodes;

          // Simula inserção (Trifurcação/Anexação direta)
          addNode(q, n.id);

          recomputeFlowAndRadius();
          double currentCost = totalCost();

          if(currentCost < minGlobalCost){
              minGlobalCost = currentCost;
              bestAttachNode = (NodeId)i;
              bestSegmentNode = -1; // Invalida bifurcação pois achou melhor trifurcação
          }
      }
  }

  // Restaura estado original antes de aplicar a melhor escolha definitiva
  nodes_ = originalNodes;

  // Se nada foi encontrado (raro), fallback para conectar à raiz ou centro
  if(bestSegmentNode == -1 && bestAttachNode == -1) {
      if(nodes_[1].children.size() < MAX_CHILDREN) bestAttachNode = 1;
      else bestSegmentNode = 1; // Força bifurcação no centro se não der
  }

  // aplicacao da melhpr escolha
  
  if (bestAttachNode != -1) {
      // Opção vencedora: TRIFURCAÇÃO (Conectar direto a um nó existente)
      return addNode(q, bestAttachNode);
  } 
  else {
      // Opção vencedora: BIFURCAÇÃO (Quebrar segmento): mantém a lógica antiga de bifurcação
      NodeId child = bestSegmentNode;
      Vec2 a = nodes_[child].p;
      Vec2 b = nodes_[nodes_[child].parent].p;
      Vec2 bif = lerp(a, b, bestT); 

      NodeId parentOld = nodes_[child].parent; 
      NodeId bifId = addNode(bif, parentOld); 

      {
        auto& vec = nodes_[parentOld].children; 
        auto it = std::find(vec.begin(), vec.end(), child);
        if (it != vec.end()) {
          vec.erase(it);
        }
      }
      nodes_[child].parent = bifId; 
      nodes_[bifId].children.push_back(child); 

      // Conecta o terminal na nova bifurcação
      NodeId term = -1;
      // Nota: bifId acabou de ser criado, tem 1 filho (child). Deve ter espaço.
      if(nodes_[bifId].children.size() < MAX_CHILDREN)
        term = addNode(q, bifId); 
      else {
        // Caso extremo, não deve ocorrer se MAX_CHILDREN >= 2
        NodeId tgt = nodes_[bifId].children.front(); 
        term = addNode(q, tgt); 
      }

      recomputeFlowAndRadius();
      return term;
  }
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


// custo total da árvore: Volume = soma sobre arestas de (r^2 * L) (CCO Clássico)
double Tree::totalCost() const {
  double c = 0.0;
  for(const auto& n: nodes_){
    if(n.parent < 0) continue; // pula nós sem pai
    const double L = dist(n.p, nodes_[n.parent].p); // comprimento da aresta
    c += std::pow(n.radius, 2.0) * L; // soma custo local (Volume)
  }
  return c; // retorna custo total
}

// conta folhas (nós sem filhos)
int Tree::leafCount() const {
  int k = 0; for(auto& n: nodes_) if(n.children.empty()) ++k; return k;
}
