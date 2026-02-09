# -----------------------------------------------------
# Makefile auxiliar para o projeto TriCCO-Model
# -----------------------------------------------------

EXEC      = tp1-binary
BUILD_DIR = build
VTK_FILE  = $(BUILD_DIR)/data/cco_tp1_binary.vtk
DOT_FILE  = $(BUILD_DIR)/data/arvore.dot
PNG_FILE  = $(BUILD_DIR)/data/arvore.png

.PHONY: build run vtk paraview dot clean distclean help

# -----------------------------------------------------
# Criação e compilação
# -----------------------------------------------------

build:
	@echo ">>> Configurando e compilando com CMake..."
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Release
	cd $(BUILD_DIR) && cmake --build . -j

# -----------------------------------------------------
# Execução
# -----------------------------------------------------

run: build
	@echo ">>> Executando $(EXEC)..."
	mkdir -p $(BUILD_DIR)/data
	cd $(BUILD_DIR) && ./$(EXEC)

# -----------------------------------------------------
# Útil para verificar o VTK
# -----------------------------------------------------

vtk: run
	@echo ">>> Arquivos na pasta $(BUILD_DIR)/data/:"
	@ls -lh $(BUILD_DIR)/data/

paraview: run
	@echo ">>> Abrindo ParaView com: $(VTK_FILE)"
	paraview $(VTK_FILE)

# -----------------------------------------------------
# Geração da imagem PNG do arquivo DOT
# -----------------------------------------------------

dot: run
	@echo ">>> Gerando imagem PNG da árvore..."
	@if [ ! -f "$(DOT_FILE)" ]; then \
		echo "ERRO: $(DOT_FILE) não existe! Rode 'make run' primeiro."; \
		exit 1; \
	fi
	dot -Tpng "$(DOT_FILE)" -o "$(PNG_FILE)"
	@echo ">>> Arquivo gerado: $(PNG_FILE)"

# -----------------------------------------------------
# Limpeza
# -----------------------------------------------------

clean:
	@echo ">>> Limpando build/..."
	rm -rf $(BUILD_DIR)

distclean:
	@echo ">>> Limpando build/ + data/..."
	rm -rf $(BUILD_DIR) data/

help:
	@echo "Comandos disponíveis:"
	@echo "  make build      - Configura e compila"
	@echo "  make run        - Compila (se precisar) e executa"
	@echo "  make vtk        - Executa e lista o VTK"
	@echo "  make paraview   - Executa e abre o VTK no ParaView"
	@echo "  make dot        - Gera a imagem PNG da árvore"
	@echo "  make clean      - Remove build/"
	@echo "  make distclean  - Remove build/ e data/"
