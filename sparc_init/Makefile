# Compiler settings
CC = sparc-linux-gcc
CFLAGS = -O2 -static 

# Programs
PROG1 = init_A
PROG2 = init_B
CLEANUP = cleanup
CORE0_OUT = core0_out.txt
CORE1_OUT = core1_out.txt
IO_OUT = my_output.txt
PROG_SRC = init_ab.c
CLEANUP_SRC = cleanup_mem.c
# Simulation parameter (override with `make run MATRIX_DIM=32`)
MATRIX_DIM ?= 32


# gem5 binary and config
GEM5 = ./build/SPARC/gem5.opt
CONFIG = dual_core_config.py
GEMFLAGS = --debug-flags=SyscallVerbose,VtoPhys
OUT_DIR = ${CURDIR}/m5out

.PHONY: all build clean run metrics

all: run metrics

build: $(PROG1) $(PROG2) $(CLEANUP)

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
	
$(CLEANUP): $(CLEANUP_SRC)
	@echo "[BUILD] Compiling $(CLEANUP)..."
	$(CC) $(CFLAGS) -o $(CLEANUP) $(CLEANUP_SRC)
	chmod +x $(CLEANUP)
	
	

clean:  
	@echo "[CLEAN] Removing old output..."
	rm -rf $(OUT_DIR)
	./$(CLEANUP) $(MATRIX_DIM)
	rm -f $(PROG1) $(PROG2) $(CORE0_OUT) $(CORE1_OUT) $(IO_OUT) $(CLEANUP)
	

run:    clean build 
	@echo "[RUN] Starting gem5 dual-core sim..."
	$(GEM5) $(GEMFLAGS) $(CONFIG) --matrix-dim=$(MATRIX_DIM) > $(CURDIR)/run.log 2>&1

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

