#!/bin/bash

# Read configuration from archive_ctl_path.cfg
source archive_ctl_path.cfg

# Function to display usage information
function display_usage() {
    echo "Usage: $0 [-s] [--log <log_file>] [-o <output_directory> | --output <output_directory>] <csv_file>"
    echo "Options:"
    echo "  -s                 Run in silent mode (suppress informative messages, only show errors)"
    echo "  --log <log_file>   Redirect both informative messages and errors to the specified log file"
    echo "  -o <output_directory>, --output <output_directory>   Specify the output directory for extracted files"
    echo "  -h, --help         Display this help menu"
}

# Check for the '-h' or '--help' option
if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
    display_usage
    exit 0
fi

# Check for the '-s' option, '--log' option, and '-o' or '--output' option
silent_mode=false
log_file="/dev/null"  # By default, log to /dev/null (discard logs)
output_directory="output_data"  # Default output directory
while [ $# -gt 0 ]; do
    case "$1" in
        -s)
            silent_mode=true
            ;;
        --log)
            shift
            log_file="$1"
            ;;
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

# Check the number of arguments
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

# Function to display log messages
function display_log() {
    echo "$1" >> "$log_file"
}

# Function to display informative messages (if not in silent mode)
function display_info() {
    if [ "$silent_mode" == "false" ]; then
        echo "$1"
    fi
    display_log "$1"
}

# Function to display error messages
function display_error() {
    echo "Error: $1"
    display_log "Error: $1"
    exit 1
}

# Function to process the archive and call archive_ctl
function process_archive() {
    local archive_path=$1
    local password=$2

    display_info "Processing archive: $archive_path"

    # Step 2: Create the output directory if it doesn't exist
    mkdir -p "$output_directory"

    # Step 3: Run the first archive_ctl command
    if [ "$password" == "NP" ]; then
        output=$("$ARCHIVE_CTL_PATH" -f "$archive_path" -l)
    else
        output=$("$ARCHIVE_CTL_PATH" -f "$archive_path" -P "$password" -l)
    fi

    # Step 4: Parse the JSON object from the output using jq
    items=$(echo "$output" | awk '/{"items": \[$/,/}$/ {print}' | jq '."items"')

    # Step 5: Check if "items" field exists
    if [ "$items" == "null" ]; then
        display_error "'items' field not found in the output JSON for archive: $archive_path"
    fi

    names=$(echo "$items" | jq -r ".[] | select(.type != 16) | .name")

    while read -r name; do
        display_info "Extracting $name from archive: $archive_path"
        file_name=$(uuidgen)
        file_name="$output_directory/${file_name}"
        if [ "$password" == "NP" ]; then
            "$ARCHIVE_CTL_PATH" -f "$archive_path" -g "$name" -o "$file_name"
        else
            "$ARCHIVE_CTL_PATH" -f "$archive_path" -P "$password" -g "$name" -o "$file_name"
        fi
    done <<< "$names"
    
    display_info "Finished processing archive: $archive_path"
    display_info "--------------------------------"
}

display_info "Starting the CSV processing..."
display_info "--------------------------------"

# Step 1: Read the CSV file
while IFS=',' read -r archive_path password; do
    process_archive "$archive_path" "$password"
done < "$csv_file"

display_info "CSV processing completed successfully!"
