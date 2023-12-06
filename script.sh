#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <caracter> <file>"
    exit 1
fi

caracter="$1"
file="$2"
propozitii_corecte=0

if [ ! -f "$file" ]; then
    echo "File not found!"
    exit 1
fi

while IFS= read -r line || [[ -n "$line" ]]; do
    if echo "$line" | grep -Eq "^[A-Z][a-zA-Z0-9 ,]*[\.\?\!]$"; then
        if ! echo "$line" | grep -Eq "(,|^)[ ]*si"; then
            if echo "$line" | grep -qi "$caracter"; then
                ((propozitii_corecte++))
            fi
        fi
    fi
done < "$file"

echo "$propozitii_corecte"