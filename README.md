# TriCCO-Model
Modeling and simulation of trifurcated arterial trees using Constrained Constructive Optimization.


raios maiores perto da raiz,

raios menores nas folhas,

e valores intermediários em nós internos,

Folha (terminal): fluxo = 1

Nó interno: fluxo = soma do fluxo dos filhos

ri = rroot * (flowi / floowroot)^(1/gamma)

gamma=3
rroot = 0.05
flow1= 1
flowroot  = 1030
ri = 0.05 * (1 / 1030)^(1/3)
ri = 0.00495098


node 0  → topo (fingido, só um nó geométrico)
node 1  → raiz fisiológica real
