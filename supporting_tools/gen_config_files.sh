#!/bin/bash

# Default values
AVG_SIZE=8192
MIN_SIZE=""
MAX_SIZE=""
TECHNIQUES=("fixed" "rabins" "ae" "gear" "fastcdc" "ram" "crc" "experiment" "seq")
VALID_TECHNIQUES=("fixed" "rabins" "ae" "gear" "fastcdc" "ram" "crc" "experiment" "seq")

SILENT=false
now=$(date +%F_%T)


function display_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -c, Compress all files in the given directory"
    echo "  --avg-size SIZE, Average size (default: 8192)"
    echo "  --min-size SIZE, Minimum size"
    echo "  --max-size SIZE, Maximum size"
    echo "  --techniques TECHNIQUES, List of techniques separated by commas NO SPACES (default: fixed,rabins,ae,gear,fastcdc,ram,crc)"
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
    -s)
      SILENT=true
      shift
      ;;
    -t)
      now="$2"
      shift 2
      ;;
    --avg-size)
      AVG_SIZE="$2"
      shift 2
      ;;
    --min-size)
      MIN_SIZE="$2"
      shift 2
      ;;
     --techniques)
      # Convert comma-separated techniques into array
      IFS=',' read -r -a TECHNIQUES <<< "$2"
      for tech in "${TECHNIQUES[@]}"; do
        if [[ ! " ${VALID_TECHNIQUES[@]} " =~ " ${tech} " ]]; then
          echo "Error: Invalid technique $tech"
          exit 1
        fi
      done
      shift 2
      ;;
    --max-size)
      MAX_SIZE="$2"
      shift 2
      ;;
    --techniques)
      # Convert comma-separated techniques into array
      IFS=',' read -r -a TECHNIQUES <<< "$2"
      for tech in "${TECHNIQUES[@]}"; do
        if [[ ! " ${VALID_TECHNIQUES[@]} " =~ " ${tech} " ]]; then
          echo "Error: Invalid technique $tech"
          exit 1
        fi
      done
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

# Generate config files
if [[ $SILENT == false ]]; then echo "Generating configuration files"; fi
mkdir -p config_${now}
mkdir -p hashes_${now}
for technique in "${TECHNIQUES[@]}"; do
  CONFIG_FILE="./config_${now}/${technique}_${AVG_SIZE}.conf"
  OUTPUT_FILE="./hashes_${now}/${technique}_${AVG_SIZE}.out"
  {
    echo "chunking_algo = $technique"
    echo "hashing_algo = sha1"
    echo "output_file = $OUTPUT_FILE"
    if [ ! -z "$MAX_SIZE" ]; then
        echo "buffer_size = $MAX_SIZE"
    else
        echo "buffer_size = 1048576"
    fi
    if [[ "$technique" == "fixed" ]]; then
      echo "fc_size = $AVG_SIZE"
    elif [[ "$technique" == "fastcdc" ]]; then
      if [ ! -z "$MIN_SIZE" ]; then
        echo "fastcdc_min_block_size = $MIN_SIZE"
      else
        echo "fastcdc_min_block_size = $(($AVG_SIZE / 4))"
      fi
      echo "fastcdc_avg_block_size = $AVG_SIZE"
      if [ ! -z "$MAX_SIZE" ]; then
        echo "fastcdc_max_block_size = $MAX_SIZE"
      else
        echo "fastcdc_max_block_size = $(($AVG_SIZE * 4))"
      fi
      echo "fastcdc_normalization_level = 2"
      echo "fastcdc_disable_normalization = false"
    elif [[ "$technique" == "gear" ]]; then
      if [ ! -z "$MIN_SIZE" ]; then
        echo "gear_min_block_size = $MIN_SIZE"
      else
        echo "gear_min_block_size = $(($AVG_SIZE / 4))"
      fi
      echo "gear_avg_block_size = $AVG_SIZE"
      if [ ! -z "$MAX_SIZE" ]; then
        echo "gear_max_block_size = $MAX_SIZE"
      else
        echo "gear_max_block_size = $(($AVG_SIZE * 4))"
      fi
    elif [[ "$technique" == "crc" ]]; then
      echo "crc_avg_block_size = $AVG_SIZE"
      if [[ "$AVG_SIZE" == "262144" ]]; then
        echo "crc_hash_bits = 6"
        echo "crc_window_size = 4096"
        echo "crc_window_step_size = 4096"
        echo "crc_min_block_size = $MIN_SIZE"
        echo "crc_max_block_size = $MAX_SIZE"
      elif [[ "$AVG_SIZE" == "4096" ]]; then
        echo "crc_hash_bits = 3"
        echo "crc_window_size = 512"
        echo "crc_window_step_size = 512"
        echo "crc_min_block_size = 1024"
        echo "crc_max_block_size = 8192"
      elif [[ "$AVG_SIZE" == "8192" ]]; then
        echo "crc_hash_bits=3"
        echo "crc_window_size=512"
        echo "crc_window_step_size=512"
        echo "crc_min_block_size = 4096"
        echo "crc_max_block_size = 16384"
      fi
    elif [[ "$technique" == "rabins" ]]; then
      if [ ! -z "$MIN_SIZE" ]; then
        echo "rabinc_min_block_size = $MIN_SIZE"
      else
        echo "rabinc_min_block_size = $(($AVG_SIZE / 4))"
      fi
      echo "rabinc_avg_block_size = $AVG_SIZE"
      if [ ! -z "$MAX_SIZE" ]; then
        echo "rabinc_max_block_size = $MAX_SIZE"
      else
        echo "rabinc_max_block_size = $(($AVG_SIZE * 4))"
      fi
      echo "rabinc_window_size = 48"
    elif [[ "$technique" == "ae" ]]; then
      echo "ae_extreme_mode = max"
      echo "ae_avg_block_size = $AVG_SIZE"
    elif [[ "$technique" == "ram" ]]; then
      if [ ! -z "$MAX_SIZE" ]; then
        echo "ram_max_block_size = $MAX_SIZE"
      else
        echo "ram_max_block_size = $(($AVG_SIZE * 4))"
      fi
      echo "ram_avg_block_size = $AVG_SIZE"
    elif [[ "$technique" == "seq" ]]; then
      echo "seq_slope_changes=50"
      echo "seq_sequence_threshold=5"
      echo "seq_jump_sizes=256,256,256,256,256"
      if [ ! -z "$MIN_SIZE" ]; then
        echo "seq_min_block_size = $MIN_SIZE"
      else
        echo "seq_min_block_size = $(($AVG_SIZE / 4))"
      fi
      if [ ! -z "$MAX_SIZE" ]; then
        echo "seq_max_block_size = $MAX_SIZE"
      else
        echo "seq_max_block_size = $(($AVG_SIZE * 4))"
      fi
      echo "seq_avg_block_size = $AVG_SIZE"
    fi
  } > $CONFIG_FILE
done

if [[ $SILENT == false ]]; then echo "Finished running config script"; fi
