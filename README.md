# Information
DedupBench is a benchmarking tool for data chunking techniques used in data deduplication. DedupBench is designed for extensibility, allowing new chunking techniques to be implemented with minimal additional code. DedupBench is also designed to be used with generic datasets, allowing for the comparison of a large number of data chunking techniques on similar grounds. 

DedupBench currently supports many state-of-the-art data chunking and hashing algorithms used in data deduplication. It has been used in the following publications:

```
  [1] Udayashankar, S., Baba, A. and Al-Kiswany, S., 2024, December. SeqCDC: Hashless Content-Defined Chunking for Data Deduplication. In 2024 ACM/IFIP 25th International Conference on Middleware (MIDDLEWARE). ACM
  [2] Jarah, MA., Udayashankar, S., Baba, A. and Al-Kiswany, S., 2024, July. The Impact of Low-Entropy on Chunking Techniques for Data Deduplication. In 2024 IEEE 17th International Conference on Cloud Computing (CLOUD) (pp. 134-140). IEEE.
  [3] Liu, A., Baba, A., Udayashankar, S. and Al-Kiswany, S., 2023, September. DedupBench: A Benchmarking Tool for Data Chunking Techniques. In 2023 IEEE Canadian Conference on Electrical and Computer Engineering (CCECE) (pp. 469-474). IEEE.
```

# Installation 
1. Install prerequisites. Note that these commands are for Ubuntu 22.04.
   ```
     sudo apt update
     sudo apt install libssl-dev
   ```
2. Clone and build the repository.
   ```
     git clone https://github.com/UWASL/dedup-bench.git
     cd dedup-bench/build/
     make
   ```
3. Generate a dataset consisting of random data for testing. This generates three 1GB files with random ASCII characters on Ubuntu 22.04.
   ```
     mkdir random_dataset
     cd random_dataset/
     base64 /dev/urandom | head -c 1000000000 > random_1.txt
     base64 /dev/urandom | head -c 1000000000 > random_2.txt
     base64 /dev/urandom | head -c 1000000000 > random_3.txt
   ```
  Alternatively, download and use the VM Dataset from DedupBench (details below).

# Running dedup-bench
1. Edit the configuration file with the required chunking, hashing techniques, and chunk sizes. Supported parameter values are given below.
   ```
     cd dedup-bench/build/
     vim config.txt
   ```
2. Run dedup-bench. Note that the output is generated in a file `hash.out` by default. Throughput and avg chunk size are printed to stdout.
   ```
     ./dedup.exe <path_to_random_dataset> config.txt
   ```
3. Measure space savings. Note that space savings will be zero if the random dataset is used.
   ```
     ./measure-dedup.exe hash.out
   ```

# Supported Chunking and Hashing Techniques

The following chunking techniques are currently supported by DedupBench. Note that the `chunking_algo` parameter in the configuration file needs to be edited before a run to switch techniques.

| Chunking Technique | chunking_algo |
|--------------------|---------------|
| AE                 | ae            |
| FastCDC            | fastcdc       |
| Gear Chunking      | gear          |
| Rabin's Chunking   | rabins        |
| RAM                | ram           |
| SeqCDC             | seq           |
| TTTD               | tttd          |

The following hashing techniques are currently supported by DedupBench. Note that the `hashing_algo` parameter in the configuration file needs to be edited before a run to switch techniques.

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

