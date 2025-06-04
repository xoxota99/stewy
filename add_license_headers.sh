#!/bin/bash

# Script to add GPL-3.0 license headers to all source files in the project
# Usage: ./add_license_headers.sh

# Define the license header
LICENSE_HEADER=$(cat LICENSE_HEADER.txt)

# Extract just the license text portion (without the opening /** and closing */)
# to insert into existing Doxygen comments
LICENSE_TEXT=$(echo "$LICENSE_HEADER" | sed -n '2,$p' | sed '$d' | sed 's/^ \* / * /g')

# Function to add license header to a file if it doesn't already have one
add_license_header() {
  local file=$1
  
  # Check if file already has a complete license header (not just the @license tag)
  if grep -q "This program is free software: you can redistribute it and/or modify" "$file"; then
    echo "Complete license header already exists in $file"
    return
  fi
  
  echo "Adding license header to $file"
  
  # For files that already have a Doxygen header with @license tag
  if grep -q "@license GPL-3.0" "$file"; then
    # Find the line with @license and append the full license text after it
    line_num=$(grep -n "@license GPL-3.0" "$file" | cut -d: -f1)
    
    # Extract the existing header
    head -n "$line_num" "$file" > "$file.tmp"
    
    # Add the additional license text (without the first line which is /**) 
    echo " *" >> "$file.tmp"
    echo "$LICENSE_TEXT" | tail -n +2 >> "$file.tmp"
    
    # Find the end of the comment block
    end_comment=$(grep -n " \*/" "$file" | head -1 | cut -d: -f1)
    
    # Add the rest of the file
    tail -n +$end_comment "$file" >> "$file.tmp"
    
    # Replace the original file
    mv "$file.tmp" "$file"
  else
    # For .h files with #pragma once
    if [[ "$file" == *.h ]] && grep -q "#pragma once" "$file"; then
      # Keep #pragma once at the top
      sed -i.bak '1s/^/#pragma once\n'"$LICENSE_HEADER"'\n\n/' "$file"
      sed -i.bak '1,3d' "$file"  # Remove the duplicate #pragma once
    else
      # For other files
      sed -i.bak '1s/^/'"$LICENSE_HEADER"'\n\n/' "$file"
    fi
  fi
  
  # Remove backup files
  rm -f "$file.bak"
}

# Find all source files and add license headers
find . -type f \( -name "*.cpp" -o -name "*.h" \) -not -path "./lib/*" -not -path "./.pio/*" | while read -r file; do
  add_license_header "$file"
done

echo "License headers have been added to all source files."
