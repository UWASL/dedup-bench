#!/bin/bash

# Read configuration from archive_ctl_path.cfg
source archive_ctl_path.cfg

# Default output directory
DEFAULT_OUTPUT_DIR="output_data"
now=$(date +%F_%T)

# Function to display usage information
function display_usage() {
    echo "Usage: $0 [-o <output_directory> | --output <output_directory>] <csv_file>"
    echo "Options:"
    echo "  -o <output_directory>, --output <output_directory>   Specify the output directory for extracted files (default: $DEFAULT_OUTPUT_DIR)"
    echo "  -h, --help         Display this help menu"
}

# Check for the '-h' or '--help' option
if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
    display_usage
    exit 0
fi

# Parse the -o or --output option for the output directory
output_directory="$DEFAULT_OUTPUT_DIR"
while [ $# -gt 0 ]; do
    case "$1" in
        -o|--output)
            shift
            output_directory="$1"
            ;;
        *)
            break
            ;;
    esac
    shift
done


# Check if the CSV file is provided
if [ $# -ne 1 ]; then
    display_usage
    exit 1
fi

csv_file=$1

# Check if the CSV file exists
if [ ! -f "$csv_file" ]; then
    echo "Error: CSV file not found: $csv_file"
    exit 1
fi

# Check if the archive ctl is configured
if [ ! -f "$ARCHIVE_CTL_PATH" ]; then
    echo "Error: could not open "$ARCHIVE_CTL_PATH". do you have execution permissions?"
    exit 1
fi


# Check if output_directory already contains files
if [ "$(ls -A $output_directory)" ]; then
    echo "Error: The directory $output_directory already contains files. Exiting..."
    exit 1
fi

echo ${now} > results.txt

./archive_extract.sh --output "$output_directory" "$csv_file"

./gen_config_files.sh --min-size 65536 --avg-size 262144 --max-size 524288 --techniques fastcdc,crc -t "$now"
./gen_config_files.sh --min-size 2048 --avg-size 8192 --max-size 16384 --techniques fastcdc,crc -t "$now" 

./dedup_script.sh -t "$now" "$output_directory"
