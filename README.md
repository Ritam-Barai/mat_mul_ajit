# mat_mul_ajit
Testing Matrix Multiplication as a emulation workload on a dual core 32 bit Ajit processor running on Sparc ISA. The objective is used to study the various performance parameters and enhance the efficiency through smart storage and cache line usage. A new algorithm based on fast data addressing might be tested. 

The testing is done using gem5 simulator where the architecture of the Ajit processor was emulated then various performance metrics are studied.

##Steps for installing and configuring gem5
- git clone https://gem5.googlesource.com/public/gem5
- cd gem5
- scons build/<arch>/gem5.opt -j$(nproc) --ignore-style

