#!/usr/bin/env python3
import os, sys

def parse_results(results_file, out_file):
    with open(results_file, 'r') as f:
        lines = f.readlines()

    config_name = None
    space_savings = None
    throughput = None
    avg_chunk_size = None
    der = None

    with open(out_file, 'w') as f:            
        f.write("Config, Space Savings (%), Throughput (MB/s), Avg Chunk Size (bytes), DER\n")
        for line in lines:
            if("config_" in line and ".txt" in line):
                if(config_name is not None):
                        f.write(f"{config_name}, {space_savings}, {throughput}, {avg_chunk_size}, {der}\n")
            
                config_name = line.strip()
                space_savings = None
                throughput = None
                avg_chunk_size = None
                der = None
            
            elif("Space savings" in line):
                space_savings = float(line.split(":")[1].strip().replace("%", ""))
            elif("Throughput" in line):
                throughput = float(line.split(":")[1].strip())
            elif("Avg Chunk size" in line):
                avg_chunk_size = int(line.split(":")[1].strip())
            elif("DER" in line):
                der = float(line.split(":")[1].strip())
    

if(__name__ == "__main__"):
    if(len(sys.argv) != 3):
        print("Usage: python parse_dedup_script_results.py <results_file> <out_file>")
        sys.exit(1)
    results_file = sys.argv[1]
    out_file = sys.argv[2]
    parse_results(results_file, out_file)
