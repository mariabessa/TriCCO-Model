#pragma once
#include <vector>
#include <random>
#include "geometry.hpp"

namespace cco {

constexpr double MIN_DIST_PT = 0.03;
constexpr double RROOT_VISUAL = 0.05;
constexpr unsigned int MAX_CHILDREN = 3;
constexpr unsigned int RNG_SEED = 42;

using NodeId = int;

struct Node {
  NodeId id = -1;
  NodeId parent = -1;
  Vec2 p;
  double flow = 0.0;
  double radius = 0.0;
  std::vector<NodeId> children;
};

struct RNG {
  std::mt19937 gen;
  std::uniform_real_distribution<double> dist;
  RNG(unsigned seed): gen(seed), dist(0.0,1.0){}
  double operator()(){ return dist(gen); }
};

struct Domain {
  double R;
  RNG* rng;
  Vec2 samplePoint() const {
    double r = std::sqrt((*rng)()) * R;
    double theta = 2*M_PI*(*rng)();
    return {r*std::cos(theta), r*std::sin(theta)};
  }
};

class Tree {
public:
  Tree(double R, double gamma);
  NodeId addNode(const Vec2& p, NodeId parent);
  NodeId addTerminal();
  NodeId nearestSegment(const Vec2& q) const;
  double minDistToAnySegment(const Vec2& q) const;
  void recomputeFlowAndRadius();
  double totalCost() const;
  int leafCount() const;
  const std::vector<Node>& nodes() const { return nodes_; }

private:
  void dfsFlow(NodeId u, double& outFlow);

  double R_;
  double gamma_;
  RNG rng_;
  Domain dom_;
  std::vector<Node> nodes_;
};

}
