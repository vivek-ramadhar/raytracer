#!/bin/bash

# Script to convert PPM files to PNG for easier viewing
# Usage: ./convert_ppm.sh [path_to_ppm_file]

set -e  # Exit on any error

# Check for ImageMagick
if ! command -v convert &> /dev/null; then
    echo "Error: ImageMagick is not installed."
    echo "Please install it with: sudo apt-get update && sudo apt-get install -y imagemagick"
    exit 1
fi

# If a file is specified, convert just that file
if [ $# -eq 1 ]; then
    if [ ! -f "$1" ]; then
        echo "Error: File $1 not found."
        exit 1
    fi
    
    # Check if it's a PPM file
    if [[ "$1" != *.ppm ]]; then
        echo "Error: $1 is not a PPM file."
        exit 1
    fi
    
    # Convert file
    OUTPUT="${1%.ppm}.png"
    echo "Converting $1 to $OUTPUT..."
    convert "$1" "$OUTPUT"
    echo "Conversion complete!"
    exit 0
fi

# If no file is specified, look for the most recent PPM file
LATEST_PPM=$(find . -type f -name "*.ppm" -print0 | xargs -0 ls -t | head -n1)

if [ -z "$LATEST_PPM" ]; then
    echo "No PPM files found in the current directory."
    exit 1
fi

# Convert the most recent PPM file
OUTPUT="${LATEST_PPM%.ppm}.png"
echo "Converting most recent PPM file: $LATEST_PPM to $OUTPUT..."
convert "$LATEST_PPM" "$OUTPUT"
echo "Conversion complete!"
