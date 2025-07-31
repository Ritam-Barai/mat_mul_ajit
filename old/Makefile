# Compiler settings
CC = gcc
CFLAGS = -O2 -static

# Programs
PROG1 = init_A
PROG2 = init_B
PROG_SRC = init_ab.c
# Simulation parameter (override with `make run MATRIX_DIM=32`)
MATRIX_DIM ?= 32


# gem5 binary and config
GEM5 = ./build/X86/gem5.opt
CONFIG = dual_core_config.py
OUT_DIR = m5out

.PHONY: all build clean run metrics

all: run metrics

build: $(PROG1) $(PROG2)

$(PROG1): $(PROG_SRC)
	@echo "[BUILD] Compiling $(PROG1)..."
	$(CC) $(CFLAGS) -D INIT_A -o $(PROG1) $(PROG_SRC)

$(PROG2): $(PROG_SRC)
	@echo "[BUILD] Compiling $(PROG2)..."
	$(CC) $(CFLAGS) -D INIT_B -o $(PROG2) $(PROG_SRC)

clean:
	@echo "[CLEAN] Removing old output..."
	rm -rf $(OUT_DIR)
	rm -f $(PROG1) $(PROG2)

run:    clean build
	@echo "[RUN] Starting gem5 dual-core sim..."
	$(GEM5) $(CONFIG) --matrix-dim=$(MATRIX_DIM) > $(OUT_DIR)/run.log

metrics:
	@echo "[RESULT] Simulation tick count:"
	@grep "sim_ticks" $(OUT_DIR)/stats.txt || echo "No sim_ticks found."

	@echo "[METRICS] L1 DCache Loads and Misses per core:"
	@for i in 0 1; do \
		echo "Core $$i:"; \
		grep "cpu$$i.dcache.ReadReq" $(OUT_DIR)/stats.txt || echo "ReadReq not found"; \
		grep "cpu$$i.dcache.overallMisses::total" $(OUT_DIR)/stats.txt || echo "Misses not found"; \
	done

