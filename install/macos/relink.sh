#!/bin/bash

TARGET_DIR="${1:-.}"

find "$TARGET_DIR" -name "*.dylib" | while read -r dylib; do
    otool -L "$dylib" 2>/dev/null | grep '/opt/homebrew' | while read -r line; do
        oldlink=$(echo "$line" | awk '{print $1}')
        libname=$(basename "$oldlink")
        newlink="@executable_path/../Frameworks/$libname"
        
        echo "In: $dylib"
        echo "    Replace $oldlink"
        echo "    with $newlink"  
        install_name_tool -change "$oldlink" "$newlink" "$dylib"
        echo "-----------------------"
    done
done
