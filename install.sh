#!/bin/sh

# Set up dependencies
sudo apt update
sudo apt install libssl-dev libxxhash-dev python3 python3-pip 
python3 -m pip install matplotlib seaborn

# Build without SIMD
cd build/
make clean
make

# Create random dataset
mkdir random_dataset && cd random_dataset/
base64 /dev/urandom | head -c 1000000000 > random_1.txt
base64 /dev/urandom | head -c 1000000000 > random_2.txt
base64 /dev/urandom | head -c 1000000000 > random_3.txt
