# Compiler settings
CC = gcc
CFLAGS = -O2 -static

# Programs
PROG1 = init_A
PROG2 = init_B
PROG_SRC = init_ab.cpp
# Simulation parameter (override with `make run MATRIX_DIM=32`)
MATRIX_DIM ?= 32


# gem5 binary and config
GEM5 = ./build/X86/gem5.opt
CONFIG = dual_core_config.py
OUT_DIR = ${CURDIR}/m5out

.PHONY: all build clean run metrics

all: run metrics

build: $(PROG1) $(PROG2)

$(PROG1): $(PROG_SRC)
	@echo "[BUILD] Compiling $(PROG1)..."
	$(CC) $(CFLAGS) -D INIT_A -o $(PROG1) $(PROG_SRC)
	chmod +x $(PROG1)
	ls -l $(PROG1)

$(PROG2): $(PROG_SRC)
	@echo "[BUILD] Compiling $(PROG2)..."
	$(CC) $(CFLAGS) -D INIT_B -o $(PROG2) $(PROG_SRC)
	chmod +x $(PROG2)
	ls -l $(PROG2)

clean:
	@echo "[CLEAN] Removing old output..."
	rm -rf $(OUT_DIR)
	rm -f $(PROG1) $(PROG2)

run:    clean build 
	@echo "[RUN] Starting gem5 dual-core sim..."
	$(GEM5) $(CONFIG) --matrix-dim=$(MATRIX_DIM) > $(CURDIR)/run.log

metrics:
	@echo "[RESULT] Simulation tick count:"
	@grep "simTicks" $(OUT_DIR)/stats.txt || echo "No sim_ticks found."

	@echo "[METRICS] L1 DCache Load Hit and Misses per core:"
	@for i in 0 1; do \
		echo "Core $$i:"; \
		grep "system.cpu$$i.numCycles " $(OUT_DIR)/stats.txt || echo "NumCycles not found"; \
		grep "system.l1d$$i.ReadReq.hits::total " $(OUT_DIR)/stats.txt || echo "ReadReq not found"; \
		grep "system.l1d$$i.overallMisses::total" $(OUT_DIR)/stats.txt || echo "Misses not found"; \
	done

