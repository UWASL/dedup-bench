#!/bin/sh

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 NUM_FILES OUTPUT_DIR"
    exit 1
fi

NUM_FILES=$1
FILE_SIZE=1073741824 # 1 GB default
OUTPUT_DIR=$2

for i in $(seq 1 $NUM_FILES); do
    OUTPUT_FILE="${OUTPUT_DIR}/random_file_${i}.bin"
    dd if=/dev/random of=$OUTPUT_FILE bs=$FILE_SIZE count=1 status=none
    echo "Generated file: $OUTPUT_FILE of size $FILE_SIZE bytes"
done
    echo "Output file directory = $2"

