#!/usr/bin/env python3
# Pass the resulting config files to deduplication script to run the parameter search. Example: ./dedup_script.sh -c <config_dir_name>
# Usage: python generate_param_search_configs.py <input_config_limits_file> <config_dir_name> [buffer_size=16384]
import os, shutil
import sys

def generate_config_files(input_file, chunking_algo_value, buffer_size, suffix):
    output_dir = f"config_{suffix}"
    
    if(os.path.exists(output_dir)):
        print(f"Output directory {output_dir} already exists. Removing it.")
        shutil.rmtree(output_dir)

    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

    with open(input_file, 'r') as f:
        lines = f.readlines()

    # Extract the param names and their values
    params = {}
    for line in lines:
        parts = line.strip().split(',')
        param_name = parts[0]
        param_values = parts[1:]
        params[param_name] = param_values

    def generate_combinations(params, index, current_combination, all_combinations):
        if index == len(params):
            all_combinations.append(current_combination.copy())
            return
        param_name = list(params.keys())[index]
        for value in params[param_name]:
            current_combination[param_name] = value
            generate_combinations(params, index + 1, current_combination, all_combinations)

    all_combinations = []
    generate_combinations(params, 0, {}, all_combinations)

    for i, combination in enumerate(all_combinations):
        with open(f"{output_dir}/config_{i+1}.txt", 'w') as f:
            for param, value in combination.items():
                f.write(f"{param}={value}\n")
            f.write(f"chunking_algo={chunking_algo_value}\n")
            f.write("hashing_algo=xxhash128\n")
            f.write("simd_mode=none\n")
            f.write(f"output_file=./hashes_{suffix}/config_{i+1}.out\n")
            f.write(f"buffer_size={buffer_size}\n")
    
    print(f"\nGenerated {len(all_combinations)} configuration files in directory: {output_dir}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python generate_configs.py <input_config_limits_file> <config_dir_name> [buffer_size=16384]")
        sys.exit(1)
    input_file = sys.argv[1]
    chunking_algo_value = "seq"
    suffix = sys.argv[2]
    buffer_size = sys.argv[3] if len(sys.argv) == 5 else "16384"

    generate_config_files(input_file, chunking_algo_value, buffer_size, suffix)
