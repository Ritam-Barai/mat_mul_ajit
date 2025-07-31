# Compiler settings
CC = gcc
CFLAGS = -O2 -static

# Programs
PROG_MAIN = main
PROG1 = init_A
PROG2 = init_B
PROGUP = init_Cup
PROGDOWN = init_Cdown
OUT0 = core0_out.txt
OUT1 = core1_out.txt
OUTR = core_rst.bin
MAIN_SRC = main.c
PROG_SRC = init_ab.c
RES_SRC = init_c.c
UTILS_SRC = utils.c
# Simulation parameter (override with `make run MATRIX_DIM=32`)
MATRIX_DIM ?= 32


# gem5 binary and config
#GEM5 = ./build/X86/gem5.opt
#CONFIG = dual_core_config.py
#OUT_DIR = ${CURDIR}/m5out

.PHONY: all build clean run 

all: run 

build: $(PROG_MAIN) 
	@echo "[EXEC] Running $(PROG_MAIN) on host..."
	./$(PROG_MAIN)

$(PROG_MAIN): $(MAIN_SRC)
	@echo "[BUILD] Compiling $(PROG_MAIN)..."
	$(CC) $(CFLAGS) -o $(PROG_MAIN) $(MAIN_SRC) $(UTILS_SRC) \
	$(if $(DEBUG),-DDEBUG) \
	$(if $(USE_CMAJOR),-DUSE_CMAJOR)

$(PROG1): $(PROG_SRC)
	@echo "[BUILD] Compiling $(PROG1)..."
	$(CC) $(CFLAGS) -D INIT_A -o $(PROG1) $(PROG_SRC) $(UTILS_SRC)


$(PROG2): $(PROG_SRC)
	@echo "[BUILD] Compiling $(PROG2)..."
	$(CC) $(CFLAGS) -D INIT_B -o $(PROG2) $(PROG_SRC) $(UTILS_SRC)
	
$(PROGUP): $(RES_SRC)
	@echo "[BUILD] Compiling $(PROGUP)..."
	$(CC) $(CFLAGS) -D INIT_C_UP -o $(PROGUP) $(RES_SRC) $(UTILS_SRC)
	
$(PROGDOWN): $(RES_SRC)
	@echo "[BUILD] Compiling $(PROGDOWN)..."
	$(CC) $(CFLAGS) -D INIT_C_DOWN -o $(PROGDOWN) $(RES_SRC) $(UTILS_SRC)


clean:
	@echo "[CLEAN] Removing old output..."
	#rm -rf $(OUT_DIR)
	rm -f $(PROG1) $(PROG2) $(PROG_MAIN) $(OUT0) $(OUT1) $(OUTR) $(PROGUP) $(PROGDOWN)

run:    clean build  
	@echo "[EXEC] Running $(PROGUP) and $(PROGDOWN) on host..."
	./$(PROGUP) $(MATRIX_DIM) &
	./$(PROGDOWN) $(MATRIX_DIM) & 
	wait 
	#@echo "[EXEC] Running $(PROG1) and $(PROG2) on host..."
	#./$(PROG1) $(MATRIX_DIM) &
	#./$(PROG2) $(MATRIX_DIM) &
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

