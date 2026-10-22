#!/usr/bin/env bash

set -e

# Run from the repository root
cd "$(git rev-parse --show-toplevel)"

input_dir="data/raw"
output_dir="data/cleaned"

mkdir -p "$input_dir" "$output_dir"

# Extract the original datasets (datasets.tgz itself is never modified)
tar zxf datasets/datasets.tgz -C "$input_dir"

for file in "$input_dir"/*.csv
do
    name=$(basename "$file")
    echo "Processing: $name"

    # The header has a different length for each station, so find the "Datum" line
    start=$(grep -n -m1 '^Datum;' "$file" | cut -d: -f1)

    # Keep only the data lines and the first 4 columns
    tail -n +$((start + 1)) "$file" | cut -d';' -f1-4 > "$output_dir/$name"

    # Plausibility check: count values outside -55 to +40 C
    lines=$(wc -l < "$output_dir/$name")
    bad=$(awk -F';' '$3 < -55 || $3 > 40' "$output_dir/$name" | wc -l)
    echo "  $lines lines, $bad values outside -55 to +40 C"
done