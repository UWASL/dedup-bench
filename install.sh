#!/bin/bash

# Set up dependencies

echo -e "\nInstalling dependencies....\n"

sudo apt update
sudo apt -y install libssl-dev libxxhash-dev python3 python3-pip 
python3 -m pip install matplotlib seaborn

# Build choices

declare -i CHOICE=0

while [ $CHOICE -eq 0 ]
do
    echo -e "\nChoose a DedupBench build:"
    echo -e "\t1. Unaccelerated (No SIMD)"
    echo -e "\t2. SIMD-256 (SSE-128 and AVX-256)"
    echo -e "\t3. SIMD-512 (Above and AVX-512)"
    echo -e "\t4. ARM NEON-128"
    echo -e "\t5. IBM Power VSX-128"
    read -p "Choice? " BUILD

    
    if [ $BUILD -eq 1 ]; then
        cd build/
        make clean   
        make

        CHOICE=1
    
    elif [[ $BUILD -eq 2 ]]; then
        cd build/
        make clean   
        make simd_all

        CHOICE=1
    
    elif [[ $BUILD -eq 3 ]]; then
        cd build/
        make clean   
        make simd512_all

        CHOICE=1
    
    elif [[ $BUILD -eq 4 ]]; then
        cd build/
        make clean   
        make arm_neon128

        CHOICE=1
    
    elif [[ $BUILD -eq 5 ]]; then
        cd build/
        make clean   
        make ibm_altivec128

        CHOICE=1
    
    else
        clear
        echo -e "\tChoose the required option correctly."
    fi

done

echo -e "\nBuild completed. Creating random dataset..."

# Create random dataset
rm -rf random_dataset
mkdir random_dataset && cd random_dataset/
base64 /dev/urandom | head -c 1000000000 > random_1.txt
base64 /dev/urandom | head -c 1000000000 > random_2.txt
base64 /dev/urandom | head -c 1000000000 > random_3.txt

echo -e "\nInstallation and random dataset creation completed in build/.\n"
