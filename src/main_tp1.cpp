#include "../include/cco/tree.hpp"
#include "../include/cco/metrics.hpp"
#include "../include/cco/io_vtk.hpp"
#include "../include/cco/io_dot.hpp"
#include "../include/cco/validate.hpp"
#include <iostream>

int main() {
    const double R = 1.0;       // raio do domínio
    const double gamma = 3.0;   // expoente de Murray
    const int TARGET_LEAVES = 25;

    cco::Tree T(R, gamma);  
    while (cco::countLeaves(T) < TARGET_LEAVES)
        T.addTerminal();

    std::cout << "[TP1] leaves=" << cco::countLeaves(T)
            << " nodes=" << T.nodes().size()
            << " cost=" << T.totalCost()
            << " depth≈" << cco::maxDepthApprox(T)
            << "\n";

    for(const auto& n : T.nodes()){
        std::cout << "node " << n.id
                << " parent=" << n.parent
                << " children=" << n.children.size()
                << " flow=" << n.flow
                << " radius=" << n.radius
                << "\n";
    }
    auto rep = cco::validateTree(T, 3, R);

    std::cout << "\n[VALIDATION]\n";
    std::cout << " ok?          " << (rep.ok ? "YES" : "NO") << "\n";
    std::cout << " nodes        " << rep.nodes << "\n";
    std::cout << " edges        " << rep.edges << "\n";
    std::cout << " leaves       " << rep.leaves << "\n";
    std::cout << " maxDegree    " << rep.maxDegree << "\n";
    std::cout << " depth≈       " << rep.depth << "\n";
    std::cout << " totalCost    " << rep.totalCost << "\n";

    if (!rep.errors.empty()) {
        std::cout << " errors:\n";
        for (const auto& msg : rep.errors) {
            std::cout << "  - " << msg << "\n";
        }
    }
  cco::writeVTK(T, "../build/data/cco_tp1_binary.vtk");
  cco::writeDOT(T, "../build/data/arvore.dot");

  return 0;
}
