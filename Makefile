# -----------------------------------------------------
# Makefile auxiliar para o projeto TriCCO-Model
# Comandos principais:
#   make build      -> cria a pasta build e compila
#   make run        -> executa o tp1-binary
#   make clean      -> limpa a pasta build/
#   make vtk        -> gera a saída VTK e lista data/
#   make paraview   -> abre o VTK no ParaView
# -----------------------------------------------------

# Nome do executável criado pelo CMake
EXEC = tp1-binary

# Pasta de compilação
BUILD_DIR = build

# Caminho do arquivo VTK gerado
VTK_FILE = build/data/cco_tp1_binary.vtk

# -----------------------------------------------------
# Criação e compilação
# -----------------------------------------------------

.PHONY: build run

build:
	@echo ">>> Configurando e compilando com CMake..."
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Release
	cd $(BUILD_DIR) && $(MAKE)

# -----------------------------------------------------
# Execução
# -----------------------------------------------------

run:
	@echo ">>> Executando $(EXEC)..."
	mkdir -p data
	./$(BUILD_DIR)/$(EXEC)

# -----------------------------------------------------
# Útil para verificar o VTK
# -----------------------------------------------------

vtk: run
	@echo ">>> Arquivos na pasta build/data/:"
	@ls -lh build/data/

paraview:
	@echo ">>> Abrindo ParaView com: $(VTK_FILE)"
	paraview $(VTK_FILE)


# -----------------------------------------------------
# Geração da imagem PNG do arquivo DOT
# -----------------------------------------------------

dot: run
	@echo ">>> Gerando imagem PNG da árvore..."
	@if [ ! -f build/data/arvore.dot ]; then \
		echo "ERRO: build/data/arvore.dot não existe!"; \
		echo "Rode 'make run' primeiro."; \
		exit 1; \
	fi
	dot -Tpng build/data/arvore.dot -o build/data/arvore.png
	@echo ">>> Arquivo gerado: build/data/arvore.png"
	
# -----------------------------------------------------
# Limpeza
# -----------------------------------------------------

clean:
	@echo ">>> Limpando build/..."
	rm -rf $(BUILD_DIR)

# -----------------------------------------------------
# Limpeza total (build + data)
# -----------------------------------------------------

distclean:
	@echo ">>> Limpando build/ + data/..."
	rm -rf $(BUILD_DIR) data/

# -----------------------------------------------------
# Ajuda
# -----------------------------------------------------

help:
	@echo "Comandos disponíveis:"
	@echo "  make build      - Configura e compila"
	@echo "  make run        - Executa o TP1"
	@echo "  make vtk        - Executa e lista o VTK"
	@echo "  make paraview   - Abre o VTK no ParaView"
	@echo "  make clean      - Remove build/"
	@echo "  make distclean  - Remove build/ e data/"
