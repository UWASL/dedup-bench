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
    echo "  -c, Compress all files in the given directory"
    echo "  -s, Silent mode (no output to console)"
    echo "  -t, current time or suffix for output naming"
    exit 1
}

# Process parameters
while (( "$#" )); do
  case "$1" in
    -h|--help)
        display_help
        ;;
    -c)
      COMPRESS=true
      shift
      ;;
    -s)
      SILENT=true
      shift
      ;;
    -t)
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

if [[ $COMPRESS == true ]]; then
  # Compress all files in the given directory
  if [[ $SILENT == false ]]; then echo "Compressing files in directory $DIRECTORY"; fi
  gzip $DIRECTORY/*
fi

COUNTER=0

# Execute dedup.exe with each config file
if [[ $SILENT == false ]]; then echo "Running dedup.exe and ./measure-dedup.exe for each configuration file"; fi
for config_file in $(ls config_${now}); do
  (echo "==================" >> "./results/results_$config_file.txt"
  echo $config_file >> "./results/results_$config_file.txt"
  cat "./config_${now}/$config_file" >> "./results/results_$config_file.txt"
  filename=$(basename $config_file | cut -f 1 -d ".")
  ./dedup.exe "$DIRECTORY" "./config_${now}/$config_file" | sed -n '/^Avg/,$p' >> "./results/results_$config_file.txt"
  ./measure-dedup.exe "./hashes_${now}/${filename}.out" >> "./results/results_$config_file.txt") &
  let COUNTER=COUNTER+1
  # Every 20 processes, wait
  if [ $COUNTER -ge 20 ]; then
      wait   # This will wait until all background processes are done
      COUNTER=0
  fi
done

wait

if [[ $SILENT == false ]]; then echo "Finished running dedup script. results are stored in results.txt"; fi
