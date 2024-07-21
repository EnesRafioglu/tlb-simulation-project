from dataclasses import dataclass
from itertools import product
import subprocess
import re

request_sizes = [
  1000,
  10000,
  50000,
  100000,
]

tlb_sizes = [
  0,
  1,
  2,
  4,
  8,
  16,
  32,
  64,
  128,
  256,
  512,
  1024,
]

blocksize = 4096

tlb_latency = 3
memory_latency = 200

output_file = "results_with_writes_neu.csv"

@dataclass
class TLBResult:
  tlb_size: int
  request_size: int

  cycles: int
  tlb_misses: int
  tlb_hits: int

  primitive_gate_count: int

  def __str__(self) -> str:
    return f"TLB size {self.tlb_size} with {self.requests_path} took {self.cycles} cycles with {self.tlb_hits} TLB hits and {self.tlb_misses} TLB misses"

def run_tlb_simulation(request_size: int, tlb_size: int) -> TLBResult:
  requests_path = f"examples/ListSum_{request_size}_withWrites.csv"

  output = subprocess.check_output(
    [
      "./tlb",
      f"--blocksize={blocksize}",
      f"--tlb-size={tlb_size}",
      f"--tlb-latency={tlb_latency}",
      f"--memory-latency={memory_latency}",
      requests_path
    ])
  output = output.decode("utf-8")

  cycles = re.search(r"Simulation took (\d+) cycles", output).group(1)
  tlb_misses = re.search(r"Simulation had (\d+) misses", output).group(1)
  tlb_hits = re.search(r"Simulation had (\d+) hits", output).group(1)
  primitive_gate_count = re.search(r"Primitive gate count: (\d+)", output).group(1)

  return TLBResult(
    tlb_size=tlb_size,
    request_size=request_size,
    cycles=int(cycles),
    tlb_misses=int(tlb_misses),
    tlb_hits=int(tlb_hits),
    primitive_gate_count=int(primitive_gate_count)
  )

def main() -> None:
  with open(output_file, "w") as f:
    f.write("TLB Size,Num requests,Cycles,TLB Misses,TLB Hits,Primitive Gate Count\n")

    for request_size, tlb_size in product(request_sizes, tlb_sizes):
      print(f"Running {request_size} requests with TLB size {tlb_size}")

      result = run_tlb_simulation(request_size, tlb_size)
    
      f.write(f"{result.tlb_size},{result.request_size},{result.cycles},{result.tlb_misses},{result.tlb_hits},{result.primitive_gate_count}\n")

if __name__ == "__main__":
  main()
