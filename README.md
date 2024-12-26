# Information
DedupBench is a benchmarking tool for data chunking techniques used in data deduplication. DedupBench is designed for extensibility, allowing new chunking techniques to be implemented with minimal additional code. DedupBench is also designed to be used with generic datasets, allowing for the comparison of a large number of data chunking techniques. 

DedupBench currently supports many state-of-the-art data chunking and hashing algorithms. Please cite the relevant publications from this list if you use the code from this repository:

```
  [1] Udayashankar, S., Baba, A. and Al-Kiswany, S., 2025, February. VectorCDC: Accelerating Data Deduplication with SSE/AVX Instructions. In 2025 USENIX 23rd Conference on File and Storage Technologies (FAST). USENIX
  [2] Udayashankar, S., Baba, A. and Al-Kiswany, S., 2024, December. SeqCDC: Hashless Content-Defined Chunking for Data Deduplication. In 2024 ACM/IFIP 25th International Middleware Conference (MIDDLEWARE). ACM
  [3] Jarah, MA., Udayashankar, S., Baba, A. and Al-Kiswany, S., 2024, July. The Impact of Low-Entropy on Chunking Techniques for Data Deduplication. In 2024 IEEE 17th International Conference on Cloud Computing (CLOUD) (pp. 134-140). IEEE.
  [4] Liu, A., Baba, A., Udayashankar, S. and Al-Kiswany, S., 2023, September. DedupBench: A Benchmarking Tool for Data Chunking Techniques. In 2023 IEEE Canadian Conference on Electrical and Computer Engineering (CCECE) (pp. 469-474). IEEE.
```

# Installation 
1. Install prerequisites. Note that these commands are for Ubuntu 22.04.
   ```
     sudo apt update
     sudo apt install libssl-dev
     sudo apt install python3
     sudo apt install python3-pip
     python3 -m pip install matplotlib
     python3 -m pip install seaborn
   ```
2. Clone and build the repository.
   ```
     git clone https://github.com/UWASL/dedup-bench.git
     cd dedup-bench/build/
     make clean
     make
   ```
3. If AVX-512 support is required, these are the alternative build commands. **_Note that building with this option on a machine without AVX-512 support will result in runtime errors._**
   ```
     make clean
     make EXTRA_COMPILER_FLAGS='-mavx512f -mavx512vl -mavx512bw'
   ```
4. Generate a dataset consisting of random data for testing. This generates three 1GB files with random ASCII characters on Ubuntu 22.04.
   ```
     mkdir random_dataset
     cd random_dataset/
     base64 /dev/urandom | head -c 1000000000 > random_1.txt
     base64 /dev/urandom | head -c 1000000000 > random_2.txt
     base64 /dev/urandom | head -c 1000000000 > random_3.txt
   ```
  Alternatively, download and use the VM Dataset from DedupBench (details below).

# Running dedup-bench
This section describes how to run dedup-bench. You can run dedup-bench using our preconfigured scripts for 8KB chunks or manually if you want custom techniques/chunk sizes.

## Preconfigured Run - 8 KB chunks
We have created scripts to run dedup-bench with an 8KB average chunk size on any given dataset. These commands run all the CDC techniques shown in the VectorCDC paper from FAST 2025. 
1. Go into the dedup-bench build directory.
```
  cd <dedup_bench_root_dir>/build/
```
2. Run dedup-script with your chosen dataset. Replace `<path_to_dataset>` with the directory of the random dataset you previously created / any other dataset of your choice.
```
  ./dedup_script.sh -t 8kb_fast25 <path_to_dataset>
```
3. Plot a graph with the throughput results from all CDC algorithms (including VRAM) on your dataset. The graph is saved in `results_graph.png`.
```
  python3 plot_throughput_graph.py results.txt
```


## Manual Runs - Custom techniques/chunk sizes
1. Choose the required chunking, hashing techniques, and chunk sizes by modifying `config.txt`. The default configuration runs SeqCDC with an average chunk size of 8 KB. Supported parameter values are given in the next section and sample config files are available in `build/config_8kb_fast25/`.
   ```
     cd <dedup_bench_repo_dir>/build/
     vim config.txt
   ```
2. Run dedup-bench. Note that the path to be passed is a directory and that the output is generated in a file `hash.out`. Throughput and avg chunk size are printed to stdout.
   ```
     ./dedup.exe <path_to_random_dataset_dir> config.txt
   ```
3. Measure space savings. Note that space savings will be zero if the random dataset is used.
   ```
     ./measure-dedup.exe hash.out
   ```

# Supported Chunking and Hashing Techniques

Here are some hints using which `config.txt` can be modified.

### Chunking Techniques
The following chunking techniques are currently supported by DedupBench. Note that the `chunking_algo` parameter in the configuration file needs to be edited to switch techniques.

| Chunking Technique | chunking_algo |
|--------------------|---------------|
| AE                 | ae            |
| FastCDC            | fastcdc       |
| Gear Chunking      | gear          |
| Rabin's Chunking   | rabins        |
| RAM                | ram           |
| SeqCDC             | seq           |
| TTTD               | tttd          |

After choosing a `chunking_algo`, make sure to check and adjust its parameters (e.g. chunk sizes). _Note that each `chunking_algo` has a separate parameter section in the config file_. For example, SeqCDC's minimum and maximum chunk sizes are called `seq_min_block_size` and `seq_max_block_size` respectively.

### SSE / AVX Acceleration
To use VectorCDC's RAM (VRAM), set `chunking_algo` to point to RAM and change `simd_mode` to one of the following values:
| SIMD Mode | simd_mode |
|-----------|-----------|
| SSE128    | sse128    |
| AVX256    | avx256    |
| AVX512    | avx512    |

Note that only RAM currently supports SSE/AVX acceleration. dedup-bench must be compiled with AVX-512 support to use the `avx512` mode.

### Hashing Techniques
The following hashing techniques are currently supported by DedupBench. Note that the `hashing_algo` parameter in the configuration file needs to be edited to switch techniques.

| Hashing Technique | hashing_algo |
|-------------------|--------------|
| MD5               | md5          |
| SHA1              | sha1         |
| SHA256            | sha256       |
| SHA512            | sha512       |
  

# VM Dataset from DedupBench 2023:

The following images from Bitnami were used in the original DedupBench paper at CCECE 2023:

### Image URLs
```
  https://marketplace.cloud.vmware.com/services/details/tomcatstack?slug=true
  https://marketplace.cloud.vmware.com/services/details/mysql?slug=true
  https://marketplace.cloud.vmware.com/services/details/rubystack?slug=true
  https://marketplace.cloud.vmware.com/services/details/jenkins?slug=true
  https://marketplace.cloud.vmware.com/services/details/ejbca-singlevm?slug=true
  https://marketplace.cloud.vmware.com/services/details/kafka?slug=true
  https://marketplace.cloud.vmware.com/services/details/elasticsearch?slug=true
  https://marketplace.cloud.vmware.com/services/details/airflow-singlevm?slug=true
  https://marketplace.cloud.vmware.com/services/details/opencart?slug=true
  https://marketplace.cloud.vmware.com/services/details/grafana?slug=true
  https://marketplace.cloud.vmware.com/services/details/redis?slug=true
```

Note that the following images were also used in the paper but are unavailable as of Sept 2024.
```
  https://marketplace.cloud.vmware.com/services/details/phplist?slug=true
  https://marketplace.cloud.vmware.com/services/details/seopanel?slug=true
  https://marketplace.cloud.vmware.com/services/details/publify?slug=true
  https://marketplace.cloud.vmware.com/services/details/canvaslms?slug=true
```

