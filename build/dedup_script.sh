#!/bin/bash

# Default values
DIRECTORY=""
COMPRESS=false
SILENT=false
now=$(date +%F_%T)


function display_help() {
    echo "Usage: $0 [OPTIONS] <DIRECTORY>"
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -c, config directory name (without config_ included) Eg: -c simd_8kb"
    exit 1
}

# Process parameters
while (( "$#" )); do
  case "$1" in
    -h|--help)
        display_help
        ;;
    -c)
      now="$2"
      shift 2
      ;;
    *)
      if [[ "$1" =~ ^- ]]; then
        echo "Error: Unknown option $1"
        exit 1
      else
        DIRECTORY="$1"
        shift
      fi
      ;;
  esac
done

if [[ -z "$DIRECTORY" ]]; then
  echo "Error: No directory provided"
  display_help
  exit 1
fi

# Execute dedup.exe with each config file
if [[ $SILENT == false ]]; then echo "Running dedup.exe and ./measure-dedup.exe for each configuration file"; fi
rm -f results.txt
rm -rf "./hashes_${now}/"
mkdir "./hashes_${now}/"
echo "Dataset path: $DIRECTORY" >> ./results.txt
for config_file in $(ls config_${now}); do
  echo "==================" >> ./results.txt
  echo $config_file >> ./results.txt
  filename=$(basename $config_file | cut -f 1 -d ".")
  ./dedup.exe "$DIRECTORY" "./config_${now}/$config_file" | sed -n '/^Avg/,$p' >> results.txt
  ./measure-dedup.exe "./hashes_${now}/${filename}.out" >> results.txt
done


if [[ $SILENT == false ]]; then echo "Finished running dedup script. results are stored in results.txt"; fi
