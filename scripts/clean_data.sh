#!/usr/bin/env bash

set -e

# Specifies the input and output directories

input_dir="data/raw"
output_dir="data/cleaned"

# Begin pre-processing datasets

for file in "$input_dir"/*.csv
do 
    echo "Processing: $(basename "$file")"

    # Skips the first 10 lines and extracts the first 4 columns
    tail -n +11 "$file" | cut -d';' -f1-4 > "$output_dir/$(basename "$file")"
done 
