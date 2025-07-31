from m5.objects import *
from m5.util import convert
from optparse import OptionParser
import m5
import os

# Define L1 Instruction Cache
class L1ICache(Cache):
    def __init__(self, size='32kB', assoc=4):
        super().__init__()
        self.size = size
        self.assoc = assoc
        self.tag_latency = 2
        self.data_latency = 2
        self.response_latency = 2
        self.mshrs = 4
        self.tgts_per_mshr = 20
        self.is_read_only = True

# Define L1 Data Cache
class L1DCache(Cache):
    def __init__(self, size='32kB', assoc=4):
        super().__init__()
        self.size = size
        self.assoc = assoc
        self.tag_latency = 2
        self.data_latency = 2
        self.response_latency = 2
        self.mshrs = 4
        self.tgts_per_mshr = 20

# Define L2 Cache
class L2Cache(Cache):
    def __init__(self, size='128kB', assoc=8):
        super().__init__()
        self.size = size
        self.assoc = assoc
        self.tag_latency = 10
        self.data_latency = 10
        self.response_latency = 10
        self.mshrs = 20
        self.tgts_per_mshr = 12

# Parse command-line arguments
parser = OptionParser()
parser.add_option("--matrix-dim", type="int", default=32,
                  help="Dimension of the matrix A/B")
(options, args) = parser.parse_args()

binary = './tests/test-progs/hello/bin/x86/linux/hello'
#binary1 = './tests/test-progs/hello/bin/x86/linux/hello32'

# Create system
system = System()
system.clk_domain = SrcClockDomain(clock='100MHz', voltage_domain=VoltageDomain())
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('128MB')]

# CPUs
system.cpu = [X86TimingSimpleCPU(cpu_id=0), X86TimingSimpleCPU(cpu_id=1)]
#system.cpu = [TimingSimpleCPU() for i in range(2)]

# Caches
system.l1i = [L1ICache() for _ in range(2)]
system.l1d = [L1DCache() for _ in range(2)]
system.l2bus = L2XBar()
system.l2cache = L2Cache()
system.membus = SystemXBar()

# Connect CPU to L1
for i in range(2):
    system.cpu[i].icache_port = system.l1i[i].cpu_side
    system.cpu[i].dcache_port = system.l1d[i].cpu_side
    system.l1i[i].mem_side = system.l2bus.cpu_side_ports
    system.l1d[i].mem_side = system.l2bus.cpu_side_ports

# L2 to memory
system.l2cache.cpu_side = system.l2bus.mem_side_ports
system.l2cache.mem_side = system.membus.cpu_side_ports

# Memory controller
#system.mem_ctrl = MemCtrl()
#system.mem_ctrl.dram = DDR3_1600_8x8()
#system.mem_ctrl.dram.range = system.mem_ranges[0]
#system.mem_ctrl.dram.device_size = '128MB'
system.mem_ctrl = SimpleMemory()
system.mem_ctrl.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

# Interrupts
for cpu in system.cpu:
    cpu.createInterruptController()
    if isinstance(cpu, X86CPU):
        cpu.interrupts[0].pio = system.membus.mem_side_ports
        cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
        cpu.interrupts[0].int_responder = system.membus.mem_side_ports
    #cpu.createThreads()
# Set up processes with matrix_dim env var
#env = str(options.matrix_dim)
#print(env)
system.workload = SEWorkload.init_compatible('./init_A')
process1 = Process()
process1.executable = './init_A'
process1.cmd=['./init_A', str(options.matrix_dim)]
process1.cwd = os.getcwd()
process1.pid = 100
#system.cpu[0].createThreads()
process2 = Process()
process2.executable = './init_B'
process2.cmd=['./init_B', str(options.matrix_dim)]
process2.cwd = os.getcwd()
process2.pid = 101

system.cpu[0].workload = process1
system.cpu[1].workload = process2
system.cpu[0].createThreads()
system.cpu[1].createThreads()


# SE workload init (any one, just needed for syscall emulation)
#system.workload = SEWorkload.init_compatible(binary)
#system.remote_gdb_port = 0


# Final connections
system.system_port = system.membus.cpu_side_ports
root = Root(full_system=False, system=system)

m5.instantiate()

print("Beginning simulation!")
exit_event = m5.simulate()
print("Exiting @ tick %i because %s" % (m5.curTick(), exit_event.getCause()))

#system.cpu.createThreads()




