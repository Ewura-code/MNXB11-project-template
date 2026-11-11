#!/usr/bin/env bash

set -e

cd "$(git rev-parse --show-toplevel)"

input_dir="data/raw"
output_dir="data/cleaned"

mkdir -p "$input_dir" "$output_dir"

tar zxf datasets/datasets.tgz -C "$input_dir"

# *.csv preprocessing script

for file in "$input_dir"/*.csv
do
    name=$(basename "$file")
    echo "Processing: $name"

    # Header length differs between stations
    start=$(grep -n -m1 '^Datum;' "$file" | cut -d: -f1)
    tail -n +$((start + 1)) "$file" | cut -d';' -f1-4 > "$output_dir/$name"

    lines=$(wc -l < "$output_dir/$name")
    bad=$(awk -F';' '$3 < -55 || $3 > 40' "$output_dir/$name" | wc -l)
    echo "  $lines lines, $bad values outside -55 to +40 C"
done

# Uppsala preprocessing script

# Generates 2 output files for general compatibility 
uppsala="uppsala_tm_1722-2022.dat" 
filtered_uppsala="filtered_uppsala_tm_1722-2022.dat"

echo "Processing: $uppsala"

# Create the unfiltered cleaned dataset (contains measurements outside of Uppsala)
# Uppsala file has Windows line endings
tr -d '\r' < "$input_dir/$uppsala" > "$output_dir/$uppsala" 

# Validate the unfiltered dataset
lines=$(wc -l < "$output_dir/$uppsala")
bad=$(awk '$4 < -55 || $4 > 40 || $5 < -55 || $5 > 40' "$output_dir/$uppsala" | wc -l)

echo "  Unfiltered: $lines lines, $bad values outside -55 to +40 C"

# Create the filtered cleaned dataset (contains only measurements from Uppsala)
awk '$6 == 1' "$output_dir/$uppsala" > "$output_dir/$filtered_uppsala"

# Validate the filtered dataset
filtered_lines=$(wc -l < "$output_dir/$filtered_uppsala")
filtered_bad=$(awk '$4 < -55 || $4 > 40 || $5 < -55 || $5 > 40' "$output_dir/$filtered_uppsala" | wc -l)

echo "  Filtered: $filtered_lines lines, $filtered_bad values outside -55 to +40 C"
